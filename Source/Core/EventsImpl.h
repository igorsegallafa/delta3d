#pragma once

namespace Delta3D::Core
{
template<typename T>
class EventsImpl
{
public:
	/**
	* Register an Event Handler
	* @param event Event Type
	* @param handler Event Handler
	 */
	void RegisterEvent( T event, std::function<void()> handler ){ events.push_back( std::make_pair( event, handler ) ); }

	//! Fire a Specified Event.
	void FireEvent( T event )
	{
		for( const auto& registeredEvent : events )
			if( registeredEvent.first == event )
				registeredEvent.second();
	}
private:
	std::vector<std::pair<T, std::function<void()>>> events;	//!< Registered Events
};
}