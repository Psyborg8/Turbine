//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

enum class DebugType {
	None = 0,
	Info,
	Warning,
	Error
};

namespace Debug {

//--------------------------------------------------------------------------------

void startTimer( string name );

void stopTimer( string name );

float getAverageTime( string name );

void addMessage( string message, DebugType type = DebugType::None );

//--------------------------------------------------------------------------------

class DebugWindow : public Object {
private:

public:
	DebugWindow() = default;

public:
	// Events
	void onStart() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;

private:
	array< sf::Time, 100 > m_deltaTimes;
	array< sf::Time, 100 >::iterator m_iterator;
	sf::Text m_text;
	bool m_first{ true };
};

//--------------------------------------------------------------------------------

}

//================================================================================