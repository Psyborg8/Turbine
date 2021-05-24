//================================================================================

#pragma once

//================================================================================

namespace Observers {

//================================================================================

enum class ObserverType {
	KeyPress, // KeyCallback
	KeyRelease, // KeyCallback

	MouseMove, // AxisCallback
	MousePress, // KeyCallback
	MouseRelease, // KeyCallback

	WindowResize, // Vec2Callback

	None
};

//--------------------------------------------------------------------------------

struct ObserverID {
	chronoClockPoint timeStamp;
	ObserverType type;
	int ID;

	bool operator==( const ObserverID& rh ) {
		return ID == rh.ID && timeStamp == rh.timeStamp;
	}

	explicit operator bool() const {
		return ID != -1;
	}

	void reset() {
		ID = -1;
	}

	virtual ~ObserverID() {}
};

//================================================================================

void initObservers();

ObserverID addObserver( ObserverType type, Callback callback );
ObserverID addObserver( ObserverType type, KeyCallback callback );
ObserverID addObserver( ObserverType type, AxisCallback callback );
ObserverID addObserver( ObserverType type, Vec2Callback callback );

void removeObserver( ObserverID ID );

//================================================================================

} // namespace Observers