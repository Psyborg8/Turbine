//================================================================================

#include "ColorSwatchWorld.h"

//--------------------------------------------------------------------------------

#include "Debug.h"
#include "System.h"
#include "Random.h"

#include "Particle.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onUpdate( sf::Time deltaTime ) {

}

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onPostUpdate( sf::Time deltaTime ) {

}

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onRender() {
	sf::RectangleShape shape;
	shape.setPosition( sf::Vector2f( -256.0f, 22.5f ) );
	shape.setSize( sf::Vector2f( 512.0f, 16.0f ) );
	shape.setFillColor( sf::Color( 50u, 50u, 50u, 255u ) );

	System::getWindow()->draw( shape );
}

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onPostRender() {

}

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onSpawnChildren() {
	m_camera.setDistance( 150.0f );
	m_camera.setPosition( Math::Vec2( 0.0f, 0.0f ) );

	shared_ptr< Debug::PerformanceWindow > performancePage = makeObject< Debug::PerformanceWindow >( this );
	performancePage->setVisibility( false );

	m_visibility = true;

	m_collider = make_shared< Game::RigidRect >();
	m_collider->setPosition( Math::Vec2( -256.0f, 300.0f ) );
	m_collider->setSize( Math::Vec2( 512.0f, 16.0f ) );
	m_collider->setCollisionType( CollisionType::Static );
	m_collider->setColor( Colors::WHITE );
	m_collider->setVisibility( true );

	using namespace Gfx::Particle;
	Pattern snowPattern = Pattern()
		.setLifetime( Pattern::Lifetime().set( 1ms ) )
		.addEmitter( Pattern::Emitter()
					 .addPattern(
						 Pattern()
						 .setPosition( Pattern::Position()
									   .setRelative( true )
									   .set( 4.0f, 0.0f ) )
						 .setVelocity( Pattern::Velocity()
									   .setFade( true, true )
									   .set( Math::Vec2( 16.0f, 0.0f ), Math::Vec2( -16.0f, -4.0f ) ) )
						 .setNumber( Pattern::Number()
									 .set( 5u, 10u ) )
						 .setSize( Pattern::Size()
								   .setFade( true )
								   .set( 2.0f, 4.0f ) )
						 .setColor( Pattern::Color()
									.setFade( true )
									.set( Math::Color( 0.2f, 0.2f, 0.2f, 0.5f ), Math::Color( 0.4f, 0.4f, 0.4f, 0.2f ), Random::RandomColorType::MixHSV ) )
						 .setLifetime( Pattern::Lifetime()
									   .set( 750ms, 1500ms ) ) ) );

	Gfx::Particle::savePattern( snowPattern, Folders::Bullets + "Player\\Jump.json" );
}

//--------------------------------------------------------------------------------

void ColorSwatchWorld::onEvent( sf::Event e ) {

}

//--------------------------------------------------------------------------------

}

//================================================================================