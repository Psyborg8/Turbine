//================================================================================

#pragma once

//================================================================================

#include "camera.h"
#include "global.h"
#include "object.h"

//================================================================================

namespace Gfx {
class Camera;
}

//================================================================================

class World : public Object {
public:
	World();

	// Events
public:
	virtual void onSpawnChildren() override;

	void update( sf::Time deltaTime );
	void postUpdate( sf::Time deltaTime );

	void render( sf::RenderTarget* target );

public:
	inline Gfx::Camera& getCamera() { return m_camera; }
	inline Math::Color getBackgroundColor() const { return m_backgroundColor; }
	inline string getCurrentMap() { return m_currentMap; }

protected:
	Gfx::Camera m_camera;
	Math::Color m_backgroundColor;
	string m_currentMap{ "" };
	float m_speed{ 1.0f };
};

//================================================================================
