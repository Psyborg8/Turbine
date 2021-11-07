//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "object.h"

#include "particle-emitter.h"

//================================================================================

namespace Gfx::Particle {

//--------------------------------------------------------------------------------

class System : public Object {
public:
	System();

public:
	void onUpdate( sf::Time deltaTime ) override;

public:
	void start();
	void stop();
	void kill();

	void addEmitter( EmitterPattern emitter );

	const list< Emitter >& getEmitters() const { return m_emitters; }
	list< Emitter >& getEmitters() { return m_emitters; }

	sf::Transformable getTransform() const { return m_transform; }
	void setTransform( sf::Transformable transform ) { m_transform = transform; }

private:
	list< Emitter > m_emitters;
	sf::Transformable m_transform;
};

//--------------------------------------------------------------------------------

}

//================================================================================