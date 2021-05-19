//==================================================================================================

#include "Timer.h"

//--------------------------------------------------------------------------------------------------

// std
#include <stack>
#include <unordered_set>

//==================================================================================================

namespace Timer
{

//==================================================================================================

struct Timer
{
	TimerID ID;

	std::chrono::time_point< std::chrono::steady_clock > startTime;
	std::chrono::milliseconds duration;
	std::chrono::nanoseconds expirationTime; // ms turned out to not be accurate enough in some cases.

	bool loop;

	UpdateCallback onUpdate;
	FinishCallback onFinish;

	bool operator==( const Timer& rh )
	{
		return ID == rh.ID;
	}
};

//==================================================================================================

std::vector< Timer > timers;
std::stack< int > freeIDs; // Recycle old IDs so nextID doesn't keep growing.
int nextID{ 0 };
std::vector< TimerID > markedForRemoval; // Handle removal timing so timers can be safely deleted by eachothers.

bool threadLock{ false }; // Prevent unsafe functions being used during iteration.

// Used to determine a correct deltaTime that is also useable during debugging.
std::chrono::time_point< std::chrono::steady_clock > previousUpdateTime;

//==================================================================================================

void update()
{
	// Lock unsafe functions.
	threadLock = true;

	const std::chrono::time_point< std::chrono::steady_clock > time = std::chrono::high_resolution_clock::now();

	//  This is useful for debugging, as time will 'stop' during a breakpoint.
	std::chrono::nanoseconds dt;
	if( std::chrono::duration_cast< std::chrono::milliseconds >( time - previousUpdateTime ) > std::chrono::milliseconds( 100 ) )
		dt = std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::milliseconds( 100 ) );

	else
		dt = time - previousUpdateTime;

	previousUpdateTime = time;

	size_t timersSize = timers.size(); // Get the size now, we don't want to process timers created during the loop.
	for( size_t idx = 0u; idx < timersSize; ++idx )
	{
		Timer& timer = timers[ idx ];

		const auto it = find( markedForRemoval.begin(), markedForRemoval.end(), timer.ID );
		if( it != markedForRemoval.end() )
		{
			continue; // This timer isn't supposed to exist, ignore it.
		}

		timer.expirationTime -= dt;

		if( timer.expirationTime <= std::chrono::nanoseconds( 0 ) )
		{
			if( timer.loop )
			{
				timer.expirationTime = std::chrono::duration_cast< std::chrono::nanoseconds >( timer.duration );
				timer.startTime = time;
			}
			else
				removeTimer( timer.ID );

			if( timer.onUpdate != nullptr )
				timer.onUpdate( 1.0f ); // onUpdate won't ever reach 1.0f otherwise
			if( timer.onFinish != nullptr )
				timer.onFinish();
		}
		else
		{
			if( timer.onUpdate != nullptr )
			{
				const float timeLeft = static_cast< float >( timer.expirationTime.count() );
				const float totalTime
					= static_cast< float >( std::chrono::duration_cast< std::chrono::nanoseconds >( timer.duration ).count() );
				const float alpha = 1.0f - timeLeft / totalTime;

				timer.onUpdate( alpha );
			}
		}
	}

	// Delete all timers that were marked for removal. Including those manually deleted before this loop.
	for( TimerID ID : markedForRemoval )
	{
		const auto it = find_if( timers.begin(), timers.end(), [ID]( Timer timer ) { return timer.ID == ID; } );
		if( it != timers.end() )
		{
			timers.erase( it );
			freeIDs.push( ID.ID ); // Free the ID so it can be used again.
		}
	}

	markedForRemoval.clear();

	// Unlock unsafe functions.
	threadLock = false;
}

//--------------------------------------------------------------------------------------------------

void init()
{
	timers = std::vector< Timer >();
	timers.reserve( 32u );

	freeIDs = std::stack< int >();
	nextID  = 0;

	markedForRemoval = std::vector< TimerID >();
	markedForRemoval.reserve( 32u );

	threadLock = false;

	previousUpdateTime = std::chrono::time_point< std::chrono::steady_clock >();

	clearTimers();
}

//--------------------------------------------------------------------------------------------------

TimerID addTimer( int msDuration, UpdateCallback onUpdate, FinishCallback onFinish, bool loop )
{
	const std::chrono::milliseconds duration = std::chrono::milliseconds( msDuration );

	return addTimer( duration, onUpdate, onFinish, loop );
}

//--------------------------------------------------------------------------------------------------

TimerID addTimer( std::chrono::milliseconds duration,
							   UpdateCallback onUpdate,
							   FinishCallback onFinish,
							   bool loop )
{
	const std::chrono::time_point< std::chrono::steady_clock > time = std::chrono::high_resolution_clock::now();

	Timer timer;

	// Recycle old IDs before using new ones.
	int ID;
	if( freeIDs.empty() )
	{
		ID = nextID;
		nextID++;
	}
	else
	{
		ID = freeIDs.top();
		freeIDs.pop();
	}

	TimerID timerID;
	timerID.ID		  = ID;
	timerID.timeStamp = std::chrono::high_resolution_clock::now();

	timer.ID			 = timerID;
	timer.duration		 = duration;
	timer.expirationTime = std::chrono::duration_cast< std::chrono::nanoseconds >( duration );
	timer.startTime		 = time;
	timer.loop			 = loop;
	timer.onUpdate		 = onUpdate;
	timer.onFinish		 = onFinish;

	timers.push_back( timer );

	return timer.ID;
}

//--------------------------------------------------------------------------------------------------

void removeTimer( TimerID ID )
{
	// Don't bother trying to delete an invalid ID.
	if( !ID )
		return;

	// Don't delete the timer now, just mark it for removal on the next update.
	// This makes it safe for timers to delete each other.
	markedForRemoval.push_back( ID );
}

//--------------------------------------------------------------------------------------------------

bool timerStillActive( TimerID ID )
{
	// Don't bother looking for an invalid ID.
	if( !ID )
		return false;

	const auto it = find_if( timers.begin(), timers.end(), [ID]( Timer timer ) { return timer.ID == ID; } );
	return it != timers.end();
}

//--------------------------------------------------------------------------------------------------

void clearTimers()
{
	if( threadLock )
	{
		assert( !L"Don't use clearTimers in a timer callback!" );
		return;
	}

	timers.clear();
	freeIDs = std::stack< int >();
	nextID  = 0;
}

//==================================================================================================

} // namespace Timer
