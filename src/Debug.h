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

class MessageWindow : public Object {
public:
	MessageWindow() = default;

public:
	// Events
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;
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