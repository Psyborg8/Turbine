//================================================================================

#include "particle-system.h"

//--------------------------------------------------------------------------------

#include "system.h"
#include "debug.h"
#include "sprite.h"
#include "shader.h"

#include "particle.h"
#include "particle-emitter.h"
#include "particle-affector-manager.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

System::System() : Object() {
	//
}

//--------------------------------------------------------------------------------

void System::onUpdate( sf::Time deltaTime ) {
	for( Emitter& emitter : m_emitters )
		emitter.update( deltaTime );

	Affector::apply( this, deltaTime );
}

//--------------------------------------------------------------------------------

void System::start() {
	for( Emitter& emitter : m_emitters )
		emitter.start();
}

//--------------------------------------------------------------------------------

void System::stop() {
	for( Emitter& emitter : m_emitters )
		emitter.stop();
}

//--------------------------------------------------------------------------------

void System::kill() {
	for( Emitter& emitter : m_emitters )
		emitter.kill();
}

//--------------------------------------------------------------------------------

void System::addEmitter( EmitterPattern emitter ) {
	Emitter e = emitter.process();
	e.initial.transform.move( m_transform.getPosition() );
	e.initial.transform.scale( m_transform.getScale() );
	e.initial.transform.rotate( m_transform.getRotation() );
	e.current = e.initial;

	m_emitters.push_back( e );
}

//--------------------------------------------------------------------------------

}

//================================================================================