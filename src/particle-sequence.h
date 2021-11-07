//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "object.h"

#include "particle-system.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

struct SequenceControlCommand {
	microseconds time{ 0 };
	int ID{ -1 };

	enum class Type {
		Start,
		Stop,
		Kill
	} type;

	bool done{ false };
};

//--------------------------------------------------------------------------------

struct SequenceTransformCommand {
	microseconds time{ 0 };
	int ID{ -1 };
	sf::Transformable transform;
	bool done{ false };
};

//--------------------------------------------------------------------------------

struct SequenceAffectorCommand {
	microseconds time{ 0 };
	int ID{ -1 };
	string affector;
	bool done{ false };
};

//--------------------------------------------------------------------------------

class Sequence : public Object {
public:
	Sequence() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;

public:
	void addSystem( shared_ptr< System > system, int ID );

	void addCommand( SequenceControlCommand command );
	void addCommand( SequenceTransformCommand command );
	void addCommand( SequenceAffectorCommand command );

	void setLength( microseconds length ) { m_length = length; }

public:
	void start( bool loop = false );
	void stop();
	void reset();
	void kill();

private:
	map< int, shared_ptr< System > > m_systems;

	list< SequenceControlCommand > m_controlCommands;
	list< SequenceTransformCommand > m_transformCommands;
	list< SequenceAffectorCommand > m_affectorCommands;

	microseconds m_duration{ 0 };
	microseconds m_length{ 0 };

	bool m_active{ false };
	bool m_looping{ false };
};

//--------------------------------------------------------------------------------

}

//================================================================================