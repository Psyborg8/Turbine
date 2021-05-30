//================================================================================

#pragma once

//================================================================================

#include "World.h"
#include "GameTimer.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

class SFMLWorld : public World {
public:
	SFMLWorld() = default;

public:
	void onSpawnChildren() override;
	void onStart() override;
	void onRender() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onEvent( sf::Event e ) override;
	void onMessage( string message ) override;

	void reset() override;

protected:
	Math::Vec2 m_levelStart;
	shared_ptr< Gfx::GameTimer > m_timer;

};

//--------------------------------------------------------------------------------

}

//================================================================================