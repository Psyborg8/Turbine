//================================================================================

#pragma once

//================================================================================

#include "global.h"

#include "object.h"
#include "camera.h"

//================================================================================

namespace Game { class Player; }
namespace Gfx { class Camera; }

//================================================================================

class World : public Object {
public:
	World();

	// Events
public:
	virtual void onSpawnChildren() override;
	virtual void reset() {}
	virtual void loadWorld( string name ) {};
	virtual void unloadWorld() {};
	virtual void setCheckpoint( size_t checkpoint ) {};

	void update( sf::Time deltaTime );
	void postUpdate( sf::Time deltaTime );

	void render( sf::RenderTarget* target );

public:
	inline Gfx::Camera& getCamera() { return m_camera; }
	inline Math::Color getBackgroundColor() const { return m_backgroundColor; }
	inline string getCurrentMap() { return m_currentMap; }
	inline shared_ptr< Game::Player > getPlayer() { return m_player; }

protected:
	Gfx::Camera m_camera;
	Math::Color m_backgroundColor;
	string m_currentMap{ "" };
	shared_ptr< Game::Player > m_player;
	float m_speed{ 1.0f };
};

//================================================================================