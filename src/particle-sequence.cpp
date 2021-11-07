//================================================================================

#include "particle-sequence.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

void Sequence::onUpdate( sf::Time deltaTime ) {
	m_duration += microseconds( deltaTime.asMicroseconds() );

	for( SequenceControlCommand& command : m_controlCommands ) {
		if( command.done || m_duration < command.time || !m_systems.count( command.ID ) )
			continue;

		shared_ptr< System > system = m_systems.at( command.ID );

		switch( command.type ) {
		case SequenceControlCommand::Type::Start:
			system->start();
			break;
		case SequenceControlCommand::Type::Stop:
			system->stop();
			break;
		case SequenceControlCommand::Type::Kill:
			system->kill();
			break;
		}

		command.done = true;
	}

	for( SequenceTransformCommand& command : m_transformCommands ) {
		if( command.done || m_duration < command.time || !m_systems.count( command.ID ) )
			continue;

		shared_ptr< System > system = m_systems.at( command.ID );
		system->setTransform( command.transform );

		command.done = true;
	}

	if( m_duration > m_length ) {
		if( m_looping )
			start( true );
	}
}

//--------------------------------------------------------------------------------

void Sequence::addSystem( shared_ptr< System > system, int ID ) {
	m_systems[ ID ] = system;
}

//--------------------------------------------------------------------------------

void Sequence::addCommand( SequenceControlCommand command ) {
	m_controlCommands.push_back( command );
}

//--------------------------------------------------------------------------------

void Sequence::addCommand( SequenceTransformCommand command ) {
	m_transformCommands.push_back( command );
}

//--------------------------------------------------------------------------------

void Sequence::addCommand( SequenceAffectorCommand command ) {
	m_affectorCommands.push_back( command );
}

//--------------------------------------------------------------------------------

void Sequence::start( bool loop ) {
	reset();
	m_active = true;
	m_looping = loop;
}

//--------------------------------------------------------------------------------

void Sequence::stop() {
	for( pair< int, shared_ptr< System > > system : m_systems )
		system.second->stop();

	m_active = false;
}

//--------------------------------------------------------------------------------

void Sequence::reset() {
	for( pair< int, shared_ptr< System > > system : m_systems )
		system.second->stop();

	for( SequenceControlCommand& command : m_controlCommands )
		command.done = false;
	for( SequenceTransformCommand& command : m_transformCommands )
		command.done = false;
	for( SequenceAffectorCommand& command : m_affectorCommands )
		command.done = false;

	m_duration = microseconds( 0 );
	m_active = false;
}

//--------------------------------------------------------------------------------

void Sequence::kill() {
	for( pair< int, shared_ptr< System > > system : m_systems )
		system.second->kill();
}

//--------------------------------------------------------------------------------

}

//================================================================================