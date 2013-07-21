/**
 * LINK	:	http://blog.csdn.net/langresser/article/details/8632028
 * FILE	:	mtimer.h
 * DATE	:	2013-07-21
 */

#pragma once

#include "CommandQueue.h"	// ?
#include "GlobalFunction.h"	// ?

//
// ��ʱ���ص�����.
//
typedef function<void(int)> TimerCallback;

class Timer
{
public:
	Timer(int timerId, int ms, bool isLoop, const TimerCallback& func);
	virtual ~Timer() {};
	int getId() const
	{
		return m_timerId;
	}
	
	// return true if need remove
	bool process();
private:
	int m_timerId;
	bool m_isLoop;
	unsigned int m_startTime;
	unsigned int m_delayTime;
	TimerCallback m_function;
};

#ifdef USE_FOR_COCOS2D
class TimerObject : public Timer
{
public:
	TimerObject(int timerId, int ms, bool isLoop, const TimerCallback& func, CCObject* target);
	virtual ~TimerObject();
private:
	CCObject* m_target;
};
#endif

class TimerMgr : public MySingleton<TimerMgr>
{
public:
	TimerMgr() 
	{
		m_currentTimerId = 0;
	};

	~TimerMgr() {};

	int addTimer(int ms, bool loop, TimerCallback func)
	{
		Timer* timer = new Timer(++m_currentTimerId, ms, loop, func);
		
		m_allTimers.push_back(std::move(timer));
		return m_currentTimerId;
	}

#ifdef USE_FOR_COCOS2D
	int addTimerObject(int ms, bool loop, TimerCallback func, CCObject* target)
	{
		TimerObject* timer = new TimerObject(++m_currentTimerId, ms, loop, func, target);

		m_allTimers.push_back(timer);
		return m_currentTimerId;
	}
#endif

	void deleteTimer(int timerId)
	{
		auto itr = std::find_if(m_allTimers.begin(), m_allTimers.end(), [timerId](Timer* timer){
			return (timer && timer->getId() == timerId);
		});
		delete *itr;
		m_allTimers.erase(itr);
	}

	void processTimer();
private:
	unsigned int m_currentTimerId;
	unsigned int m_timerMgrStart;

	std::vector<Timer*> m_allTimers;
};

inline int delay_call(int ms, const TimerCallback& func, bool isLoop = false)
{
	return TimerMgr::getSingleton().addTimer(ms, isLoop, func);
}

inline int delay_call(CCObject* target, int ms, const TimerCallback& func, bool isLoop = false)
{
	return TimerMgr::getSingleton().addTimerObject(ms, isLoop, func, target);
}

#ifdef USE_FOR_COCOS2D

// �÷�PERFORM_SELECTOR(�ӳ�ʱ�����, �ص�����, ����)���м�����������PERFORM_SELECTORn
#define PERFORM_SELECTOR(ms, func, ...) delay_call(this, static_cast<int>(ms), bind(&func, this, ##__VA_ARGS__))
#endif

inline void remove_timer(int timerId)
{
	TimerMgr::getSingleton().deleteTimer(timerId);
}

/*

��ʱ��˵��Ҳ�ܼ򵥣����Ǽ�¼��ʼʱ�䣬ÿ���⣬������ӳ�ʱ��
����ִ�ж�Ӧ�Ļص�������  ������Ϸ�зǳ������Ĺ����ࡣ�������
function��bindʵ���˸������Ļص��󶨡�  �ص�����û���κ����ƣ�
Ҳ����Ҫ�ص��߼̳��κζ���������������һ��ս�������ߣ���ʼս����
ʱ�򲥷�һ����Ч����ʱ1�뿪ʼս�����̡���ô�Ϳ�����ôд��

```
class FightMgr
{
public:
    void play()
    {
        // play effect here
        delay_call(1000, bind(&FightMgr::startFight, this, param1, param2), false);
    }
    void startFight(int playerId, int data)
    {
    }
}
```

��ʱ��delay_call�ڶ���������һ��function��ͨ��bind���԰����⺯����
�������Դ�lambda��  
�����ʱ�������Ƕ��߳��첽�����ģ���Ҫ��Ϸÿ�����е�ʱ�����
TimerMgr::getSingleton().process();  �����ĺô�������д�����ʱ��
��Ҫ�����߳�ͬ������Ȼ����ʱִ��ĳЩ���������Ƕ��������߳���ɵĵ��ã�
Ҳ���õ���opengl��contentʧЧ�����⡣


**/
