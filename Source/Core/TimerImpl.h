#pragma once

namespace Delta3D::Core
{
class TimerImpl
{
public:
	TimerImpl();

	virtual void Update( float timeElapsed ) = 0;
};

class Timer
{
public:
	//! Default Constructor for Timers Handler.
	Timer(){ }

	//! Update Timers.
	static void Update( float timeElapsed );

	//! Register Timer.
	static void RegisterTimer( TimerImpl* timer ) { timersImpl.push_back( timer ); }

	//! Delete Timer.
	static void DeleteTimer( TimerImpl* timer );

	static std::vector<TimerImpl*> timersImpl;	//!< Timers Allocate
};
}