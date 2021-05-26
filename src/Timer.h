//==================================================================================================

#pragma once

//==================================================================================================

#include <chrono>

//==================================================================================================

namespace Timers {

//==================================================================================================

// Timer identifier used to reference timers.
struct TimerID {
	// Use a time stamp in addition to an ID since we reuse old IDs.
	std::chrono::time_point< std::chrono::steady_clock > timeStamp;
	int ID = -1;

	bool operator==( const TimerID& rh ) const {
		return timeStamp == rh.timeStamp && ID == rh.ID;
	}

	explicit operator bool() const {
		return ID != -1;
	}

	// Removing timers is fairly costly. So using this when a timer is already removed to prevent repeated removal
	// attempts of the same timer is recommended.
	void reset() {
		ID = -1;
	}
};

//==================================================================================================

// Called on update
// Float argument is the lerp alpha from 0 - 1
using UpdateCallback = std::function< void( double ) >;
// Called on timer finish
using FinishCallback = std::function< void() >;

//==================================================================================================

// Must be called on update.
void update();

// Must be called on init.
void init();

// Add a new timer. onUpdate and onFinish can be nullptr. Returns timer ID for removal.
TimerID addTimer( int msDuration, UpdateCallback onUpdate, FinishCallback onFinish, bool loop );
TimerID addTimer( std::chrono::milliseconds duration, UpdateCallback onUpdate, FinishCallback onFinish, bool loop );

// Remove timer using ID given when timer is created.
void removeTimer( TimerID ID );

// Triggers a timer's onFinish function, and restarts it if it's looping.
void triggerTimer( TimerID ID );

/// Returns true if timer is still valid.
bool timerStillActive( TimerID ID );

/// Remove all timers.
void clearTimers();

//==================================================================================================

} // namespace Timers