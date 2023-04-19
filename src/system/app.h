// ======================================================================

#ifndef turbine_app_h
#define turbine_app_h

// ======================================================================

#include "camera.h"
#include "global.h"
#include "object.h"

// ======================================================================

class App : public Object {
public:
	App();
	virtual ~App() = default;

public:
	void update( sf::Time delta );
	void postUpdate( sf::Time delta );

	void render( sf::RenderTarget* target );

public:
	inline Gfx::Camera& getCamera() { return m_camera; }
	inline const Gfx::Camera& getCamera() const { return m_camera; }

	inline Math::Color getBackgroundColor() const { return m_background_color; }

protected:
	Gfx::Camera m_camera;
	Math::Color m_background_color;
	float m_speed{ 1.f };
};

// ======================================================================

#endif

// ======================================================================
