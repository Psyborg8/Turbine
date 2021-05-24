//================================================================================

#include "CollisionWorld.h"

//--------------------------------------------------------------------------------

#include "System.h"

//================================================================================

void CollisionWorld::onSpawnChildren() {
	World::onSpawnChildren();

	m_camera->setDistance( 3.0 );

	{
		Gfx::Rect wall;
		wall.position = Vec2( -1.0, -1.0 );
		wall.size = Vec2( 0.5, 0.5 );
		m_walls.push_back( wall );
	}
	{
		Gfx::Rect wall;
		wall.position = Vec2( -1.5, 1.0 );
		wall.size = Vec2( 2.5, 0.5 );
		m_walls.push_back( wall );
	}
	{
		Gfx::Rect wall;
		wall.position = Vec2( 1.0, -1.0 );
		wall.size = Vec2( 1.0, 1.0 );
		m_walls.push_back( wall );
	}
	{
		Gfx::Rect player;
		player.position = Vec2();
		player.size = Vec2( 0.5, 1.0 );
		m_player = player;
	}
}

//--------------------------------------------------------------------------------

void CollisionWorld::onUpdate( double deltaTime ) {
	Vec2 mousePosition = System::getMousePosition();
	mousePosition = m_camera->screenToWorld( mousePosition );

	Vec2 diff = mousePosition - m_player.midpoint();
	diff = diff.normalize() * 0.05 * deltaTime;

	m_player.velocity += diff;

	m_player.position += m_player.velocity;

	m_lines.clear();
	for( Gfx::Rect& wall : m_walls ) {
		Collision::CollisionResult result = Collision::collision( &m_player, &wall );
		if( result.success ) {
			wall.fillColor = Colors::YELLOW;

			Gfx::Line distance;
			distance.start = m_player.midpoint();
			distance.end = result.point;
			distance.color = Colors::GREEN;
			m_lines.push_back( distance );

			Gfx::Line normal;
			normal.start = result.point;
			normal.end = result.point + ( result.normal * 0.3 );
			normal.color = Colors::CYAN;
			m_lines.push_back( normal );

			Collision::resolveCollision( result );
		}
		else
			wall.fillColor = Colors::WHITE;
	}
}

//--------------------------------------------------------------------------------

void CollisionWorld::onRender() {
	for( Gfx::Rect wall : m_walls )
		wall.render();

	m_player.render();

	for( Gfx::Line line : m_lines )
		line.render();
}

//--------------------------------------------------------------------------------