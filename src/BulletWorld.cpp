//================================================================================

#include "BulletWorld.h"

//--------------------------------------------------------------------------------

#include "Bullet.h"
#include "Debug.h"
#include "Random.h"

//================================================================================

namespace Worlds {

//--------------------------------------------------------------------------------

void BulletWorld::onSpawnChildren() {
	m_camera.setDistance( 512.0f );

	shared_ptr< Debug::PerformanceWindow > performancePage = makeObject< Debug::PerformanceWindow >( this );
	performancePage->setVisibility( false );

	m_visibility = true;
}

//--------------------------------------------------------------------------------

void BulletWorld::onEvent( sf::Event e ) {
	using namespace Game::Bullet;

	if( e.type == sf::Event::KeyPressed ) {
		if( e.key.code == sf::Keyboard::Space ) {
			Game::Bullet::spawnBullet( this,
									   Pattern()
									   .setColor( Colors::WHITE )
									   .setSize( 2.0f, 8.0f )
									   .setVelocity( Math::Vec2( -196.0f, -196.0f ), Math::Vec2( 196.0f, 196.0f ) )
									   .setLifetime( 1000ms, 2000ms )
									   .setSize( []( float alpha, float size ) { return size * ( 1.0f - alpha ); } )
									   .setVelocity( []( float alpha, Math::Vec2 velocity ) { return velocity * ( 1.0f - alpha ); } )
									   .setColor( []( float alpha, Math::Color color ) { color.a = color.a * ( 1.0f - alpha ); return color; } )
									   .setNumber( 1u, 10u )
									   .addPattern( Pattern()
													.setColor( Colors::CYAN )
													.setSize( 1.0f, 4.0f )
													.setVelocity( Math::Vec2( -196.0f, -196.0f ), Math::Vec2( 196.0f, 196.0f ) )
													.setLifetime( 1000ms, 2000ms )
													.setSize( []( float alpha, float size ) { return size * ( 1.0f - alpha ); } )
													.setVelocity( []( float alpha, Math::Vec2 velocity ) { return velocity * ( 1.0f - alpha ); } )
													.setColor( []( float alpha, Math::Color color ) { color.a = color.a * ( 1.0f - alpha ); return color; } )
													.setNumber( 5u, 15u )
													.setTime( 1.0f )
													.addPattern( Pattern()
																 .setColor( Colors::MAGENTA )
																 .setSize( 0.5f, 2.0f )
																 .setVelocity( Math::Vec2( -196.0f, -196.0f ), Math::Vec2( 196.0f, 196.0f ) )
																 .setSize( []( float alpha, float size ) { return size * ( 1.0f - alpha ); } )
																 .setVelocity( []( float alpha, Math::Vec2 velocity ) { return velocity * ( 1.0f - alpha ); } )
																 .setColor( []( float alpha, Math::Color color ) { color.a = color.a * ( 1.0f - alpha ); return color; } )
																 .setNumber( 10u, 20u )
																 .setTime( 1.0f )
																 .setLifetime( 500ms, 1500ms ) ) )
									   .addPattern( Pattern()
													.setColor( Colors::CYAN )
													.setSize( 1.0f, 4.0f )
													.setVelocity( Math::Vec2( -196.0f, -196.0f ), Math::Vec2( 196.0f, 196.0f ) )
													.setSize( []( float alpha, float size ) { return size * ( 1.0f - alpha ); } )
													.setVelocity( []( float alpha, Math::Vec2 velocity ) { return velocity * ( 1.0f - alpha ); } )
													.setColor( []( float alpha, Math::Color color ) { color.a = color.a * ( 1.0f - alpha ); return color; } )
													.setNumber( 5u, 15u )
													.setTime( 1.0f )
													.setLifetime( 1000ms, 2000ms )
													.addPattern( Pattern()
																 .setColor( Colors::YELLOW )
																 .setSize( 0.5f, 2.0f )
																 .setVelocity( Math::Vec2( -196.0f, -196.0f ), Math::Vec2( 196.0f, 196.0f ) )
																 .setSize( []( float alpha, float size ) { return size * ( 1.0f - alpha ); } )
																 .setVelocity( []( float alpha, Math::Vec2 velocity ) { return velocity * ( 1.0f - alpha ); } )
																 .setColor( []( float alpha, Math::Color color ) { color.a = color.a * ( 1.0f - alpha ); return color; } )
																 .setNumber( 10u, 20u )
																 .setTime( 1.0f )
																 .setLifetime( 500ms, 1500ms ) ) ) );
		}
	}
}

//--------------------------------------------------------------------------------

}

//================================================================================