//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "MathTypes.h"
#include "Object.h"
#include "Patterns.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

namespace Particle {
class Particle;
}

//--------------------------------------------------------------------------------

namespace Emitter {

//--------------------------------------------------------------------------------

class Emitter : public Object {
public:
	Emitter() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;
	void onDestroy() override;

public:
	void init( const Pattern& pattern, milliseconds lifetime );

	void start();
	void stop();
	void kill();

	bool isDead() const { return m_dead; }

	const Pattern& getPattern() const { return m_pattern; }
	Pattern& getPattern() { return m_pattern; }

private:
	void calculateSpawnTime();
	void spawnPattern( const Particle::Pattern& pattern );

private:
	Pattern m_pattern;

	microseconds m_startTime{ 0ms };
	microseconds m_endTime{ 0ms };

	microseconds m_duration{ 0ms };
	microseconds m_spawnTime{ 0ms };

	float m_alpha{ 0.0f };
	bool m_active{ true };
	bool m_dead{ false };
};

//--------------------------------------------------------------------------------

shared_ptr< Emitter > spawn( Object* parent, Pattern pattern, Math::Vec2 position = Math::Vec2(), milliseconds lifetime = -1ms );

void stopAll();

void killAll();

void destroyAll();

vector< shared_ptr< Emitter > > getAll();

void cleanup();

//--------------------------------------------------------------------------------

}
}

//================================================================================