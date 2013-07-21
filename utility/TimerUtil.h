/**
 * LINK	:	http://blog.csdn.net/langresser/article/details/8632028
 * FILE	:	mtimer.h
 * DATE	:	2013-07-21
 */

#pragma once

#include "CommandQueue.h"	// ?
#include "GlobalFunction.h"	// ?

//
// 计时器回调函数.
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

// 用法PERFORM_SELECTOR(延迟时间毫秒, 回调函数, 参数)，有几个参数调用PERFORM_SELECTORn
#define PERFORM_SELECTOR(ms, func, ...) delay_call(this, static_cast<int>(ms), bind(&func, this, ##__VA_ARGS__))
#endif

inline void remove_timer(int timerId)
{
	TimerMgr::getSingleton().deleteTimer(timerId);
}

/*

计时器说简单也很简单，就是纪录开始时间，每桢监测，如果到延迟时间
了则执行对应的回调函数。  算是游戏中非常基础的工具类。这里借助
function和bind实现了更加灵活的回调绑定。  回调函数没有任何限制，
也不需要回调者继承任何东西。比如我们有一个战斗管理者，开始战斗的
时候播放一个特效，延时1秒开始战斗流程。那么就可以这么写。

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

计时器delay_call第二个参数是一个function，通过bind可以绑定任意函数，
甚至可以传lambda。  
这个计时器并不是多线程异步操作的，需要游戏每桢运行的时候调用
TimerMgr::getSingleton().process();  这样的好处是我们写代码的时候不
需要考虑线程同步，虽然我延时执行某些函数，但是都是在主线程完成的调用，
也不用担心opengl的content失效等问题。


**/
