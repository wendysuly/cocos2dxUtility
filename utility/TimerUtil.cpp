#include "TimerUtil.h"

TimerObject::TimerObject(int timerId, int ms, bool isLoop, const TimerCallback& func, CCObject* target):Timer(timerId, ms, isLoop, func)
{
	m_target = target;
	m_target->retain();
}

TimerObject::~TimerObject()
{ 
	if (m_target) {
		m_target->release();
		m_target = NULL;
	}
}


Timer::Timer(int timerId, int ms, bool isLoop, const TimerCallback& func)
{
	m_timerId = timerId;
	m_isLoop = isLoop;

	// 可以循环，第一次直接调用
	if (isLoop) {
		m_startTime = 0;
	} else {
		m_startTime = TimeGet();
	}
	m_delayTime = ms;
	m_function = std::move(func);
}

bool Timer::process()
{
	if (TimeGet() < m_startTime + m_delayTime) {
		return false;
	}

	m_function(m_timerId);

	if (m_isLoop) {
		m_startTime = TimeGet();
		return false;
	} else {
		return true;
	}
}






void TimerMgr::processTimer()
{
	for (int i = 0; i < (int)m_allTimers.size(); ++i) {
		Timer* timer = m_allTimers[i];
		if (timer && timer->process()) {
			delete timer;
			m_allTimers[i] = NULL;
		}
	}

	for (auto itr = m_allTimers.begin(); itr != m_allTimers.end();) {
		if (*itr == NULL) {
			itr = m_allTimers.erase(itr);
		} else {
			++itr;
		}
	}
}

