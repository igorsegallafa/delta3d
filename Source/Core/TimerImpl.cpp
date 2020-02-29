#include "PrecompiledHeader.h"
#include "TimerImpl.h"

namespace Delta3D::Core
{

std::vector<TimerImpl*> Timer::timersImpl;

TimerImpl::TimerImpl()
{
	Timer::RegisterTimer( this );
}

void Timer::Update( float timeElapsed )
{
	for( const auto& timer : timersImpl )
		timer->Update( timeElapsed );
}

void Timer::DeleteTimer( TimerImpl* timer )
{
	for( auto it = timersImpl.begin(); it != timersImpl.end(); ++it )
	{
		if( (*it) == timer )
		{
			timersImpl.erase( it );
			break;
		}
	}
}

}