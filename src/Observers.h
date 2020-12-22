//================================================================================

#pragma once

//================================================================================

enum class ObserverType
{
	KeyPress, // KeyCallback
	KeyRelease, // KeyCallback

	MouseMove,
	MousePress,
	MouseRelease,

	None
};

//--------------------------------------------------------------------------------

struct ObserverID
{
	chronoClockPoint timeStamp;
	ObserverType type;
	int ID;

	bool operator==( const ObserverID& rh )
	{
		return ID == rh.ID && timeStamp == rh.timeStamp;
	}

	explicit operator bool() const
	{
		return ID != -1;
	}

	void reset()
	{
		ID = -1;
	}

	virtual ~ObserverID() {}
};


//================================================================================

namespace Observer
{

//================================================================================

void initObservers();

ObserverID addObserver( ObserverType type, Callback callback );
ObserverID addObserver( ObserverType type, KeyCallback callback );
ObserverID addObserver( ObserverType type, AxisCallback callback );

void removeObserver( ObserverID ID );

//================================================================================

} // namespace Observer