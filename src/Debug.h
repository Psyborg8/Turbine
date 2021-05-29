//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

void startTimer( string name );

void stopTimer( string name );

float getAverageTime( string name );

//--------------------------------------------------------------------------------

class DebugWindow : public Object {
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