//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

namespace Debug {

//--------------------------------------------------------------------------------

enum class DebugType {
	None = 0,
	Info,
	Warning,
	Error,
	Input,
	Performance,
};

//--------------------------------------------------------------------------------

void startTimer( string name );

void stopTimer( string name );

float getAverageTime( string name );

void addMessage( string message, DebugType type = DebugType::None );

//--------------------------------------------------------------------------------

class PerformanceWindow : public Object {
public:
	PerformanceWindow() = default;

public:
	// Events
	void onStart() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onEvent( sf::Event e ) override;

private:
	array< sf::Time, 100 > m_deltaTimes;
	array< sf::Time, 100 >::iterator m_iterator;
	sf::Text m_text;
	bool m_first{ true };

	sf::Clock m_renderTimer;
};

//--------------------------------------------------------------------------------

class JoystickWindow : public Object {
public:
	JoystickWindow() = default;

public:
	// Events
	void onStart() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onEvent( sf::Event e ) override;

private:
	sf::Text m_text;
	set< int > buttons;
};

//--------------------------------------------------------------------------------

class PhysicsWindow : public Object {
public:
	PhysicsWindow() = default;

public:
	// Events
	void onStart() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;

private:
	sf::Text m_text;
};

//--------------------------------------------------------------------------------

}

//================================================================================