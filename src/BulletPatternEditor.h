//================================================================================

#pragma once

//================================================================================

#include "Global.h"
#include "World.h"
#include "Particle.h"
#include "Random.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

class BulletPatternEditor : public World {
private:
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

			bool fadeX{ true };
			bool fadeY{ true };
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

public:
	BulletPatternEditor() = default;

public:
	void onSpawnChildren() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;

public:
	void renderPattern( Pattern& pattern );
	void renderPosition( Pattern& pattern );
	void renderVelocity( Pattern& pattern );
	void renderGravity( Pattern& pattern );
	void renderColor( Pattern& pattern );
	void renderNumber( Pattern& pattern );
	void renderSize( Pattern& pattern );
	void renderLifetime( Pattern& pattern );

	Gfx::Particle::Pattern convertPattern( Pattern& pattern );
	Pattern& convertPattern( const Gfx::Particle::Pattern& pattern );

private:
	Pattern m_basePattern;
};

//--------------------------------------------------------------------------------

}

//================================================================================