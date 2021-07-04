//================================================================================

#pragma once

//================================================================================

#include "World.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

class ColorSwatchWorld : public World {
public:
	ColorSwatchWorld() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;
	void onPostUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onPostRender() override;
	void onSpawnChildren() override;
	void onEvent( sf::Event e ) override;

protected:
	shared_ptr< Game::RigidRect > m_collider;
};

//--------------------------------------------------------------------------------

}

//================================================================================