//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "Object.h"
#include "Random.h"
#include "Timer.h"
#include "RigidRect.h"

//================================================================================

namespace Gfx {
namespace Particle {

//--------------------------------------------------------------------------------

enum class ValueType {
	Static,
	Random,
	Fade,
};

//--------------------------------------------------------------------------------

struct Pattern {
	string name;

	struct {
		ValueType type{ ValueType::Static };
		int min{ 0 };
		int max{ 0 };
	} lifetime;

	struct {
		ValueType type{ ValueType::Static };
		array< float, 2u > min{ 0.0f, 0.0f };
		array< float, 2u > max{ 0.0f, 0.0f };
	} position;

	struct {
		ValueType type{ ValueType::Static };
		ValueType startType{ ValueType::Static };
		ValueType endType{ ValueType::Static };

		array< float, 2u > startMin{ 0.0f, 0.0f };
		array< float, 2u > startMax{ 0.0f, 0.0f };
		array< float, 2u > endMin{ 0.0f, 0.0f };
		array< float, 2u > endMax{ 0.0f, 0.0f };

		pair< bool, bool > fade{ false, false };
	} direction;

	struct {
		ValueType type{ ValueType::Static };
		ValueType startType{ ValueType::Static };
		ValueType endType{ ValueType::Static };

		float startMin{ 0.0f };
		float startMax{ 0.0f };
		float endMin{ 0.0f };
		float endMax{ 0.0f };

		pair< bool, bool > fade{ false, false };
	} velocity;

	struct {
		ValueType type{ ValueType::Static };
		ValueType startType{ ValueType::Static };
		ValueType endType{ ValueType::Static };

		float startMin{ 0.0f };
		float startMax{ 0.0f };
		float endMin{ 0.0f };
		float endMax{ 0.0f };
	} size;

	struct {
		ValueType type{ ValueType::Static };
		Random::RandomColorType randomType{ Random::RandomColorType::ShuffleRGB };
		ValueType startType{ ValueType::Static };
		Random::RandomColorType startRandomType{ Random::RandomColorType::ShuffleRGB };
		ValueType endType{ ValueType::Static };
		Random::RandomColorType endRandomType{ Random::RandomColorType::ShuffleRGB };

		ImVec4 startMin{ 1.0f, 1.0f, 1.0f, 1.0f };
		ImVec4 startMax{ 1.0f, 1.0f, 1.0f, 1.0f };
		ImVec4 endMin{ 1.0f, 1.0f, 1.0f, 1.0f };
		ImVec4 endMax{ 1.0f, 1.0f, 1.0f, 1.0f };

		bool fadeR{ true };
		bool fadeG{ true };
		bool fadeB{ true };
		bool fadeA{ true };
	} color;

	struct {
		ValueType type{ ValueType::Static };

		int min{ 0 };
		int max{ 0 };
	} number;
};

//================================================================================

class Particle : public Object {
public:
	Particle() = default;

public:
	void onUpdate( sf::Time deltaTime ) override;
	void onPostUpdate( sf::Time deltaTime ) override;
	void onRender() override;
	void onDestroy() override;

	void setSize( Math::Vec2 size ) override { setSize( size.x ); }
	void setSize( float radius ) { m_shape.setRadius( radius ); }

	Math::Vec2 getSize() const override { return Math::Vec2( m_shape.getRadius(), 0.0f ); } 

public:
	void init( const Pattern& pattern );

private:
	milliseconds m_lifetime;
	sf::CircleShape m_shape;

	pair< Math::Vec2, Math::Vec2 > m_direction;
	pair< float, float > m_power;
	pair< float, float > m_size;
	pair< Math::Color, Math::Color > m_color;

	Timers::TimerID m_timer;
};

//================================================================================

void spawnParticle( Object* parent, Pattern pattern );
void spawnParticle( Object* parent, Pattern pattern, Math::Vec2 position );

// Pattern loadPattern( string filepath );

// void savePattern( Pattern pattern, string filepath );

//--------------------------------------------------------------------------------

}
}

//================================================================================