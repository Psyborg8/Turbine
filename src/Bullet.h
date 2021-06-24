//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

namespace Game {
namespace Bullet {

//--------------------------------------------------------------------------------

class Pattern {
public:
	Pattern() = default;

public:
	Pattern& setLifetime( milliseconds lifetime ) { m_lifetime = make_pair( lifetime, lifetime ); return *this; }
	Pattern& setLifetime( milliseconds min, milliseconds max ) { m_lifetime = make_pair( min, max ); return *this; }
	Pattern& setLifetime( function< milliseconds( float alpha, milliseconds lifetime ) > func ) { m_lifetimeFunc = func; return *this; }
	pair< milliseconds, milliseconds > getLifetime() const { return m_lifetime; }

	Pattern& setPosition( Math::Vec2 position ) { m_position = make_pair( position, position ); return *this; }
	Pattern& setPosition( Math::Vec2 min, Math::Vec2 max ) { m_position = make_pair( min, max ); return *this; }
	Pattern& setPosition( function< Math::Vec2( float alpha, Math::Vec2 position ) > func ) { m_positionFunc = func; return *this; }
	pair< Math::Vec2, Math::Vec2 > getPosition() const { return m_position; }
	function< Math::Vec2( float alpha, Math::Vec2 ) > getPositionFunction() const { return m_positionFunc; }

	Pattern& setVelocity( Math::Vec2 velocity ) { m_velocity = make_pair( velocity, velocity ); return *this; }
	Pattern& setVelocity( Math::Vec2 min, Math::Vec2 max ) { m_velocity = make_pair( min, max ); return *this; }
	Pattern& setVelocity( function< Math::Vec2( float alpha, Math::Vec2 velocity ) > func ) { m_velocityFunc = func; return *this; }
	pair< Math::Vec2, Math::Vec2 > getVelocity() const { return m_velocity; }
	function< Math::Vec2( float alpha, Math::Vec2 ) > getVelocityFunction() const { return m_velocityFunc; }

	Pattern& setSize( float size ) { m_size = make_pair( size, size ); return *this; }
	Pattern& setSize( float min, float max ) { m_size = make_pair( min, max ); return *this; }
	Pattern& setSize( function< float( float alpha, float size ) > func ) { m_sizeFunc = func; return *this; }
	pair< float, float > getSize() const { return m_size; }
	function< float( float, float ) > getSizeFunction() const { return m_sizeFunc; }

	Pattern& setNumber( uint16_t number ) { m_number = make_pair( number, number ); return *this; }
	Pattern& setNumber( uint16_t min, uint16_t max ) { m_number = make_pair( min, max ); return *this; }
	pair< uint16_t, uint16_t > getNumber() const { return m_number; }

	Pattern& setTime( float time ) { m_time = make_pair( time, time ); return *this; }
	Pattern& setTime( float min, float max ) { m_time = make_pair( min, max ); return *this; }
	pair< float, float > getTime() const { return m_time; }

	Pattern& setColor( Math::Color color ) { m_color = color; return *this; }
	Pattern& setColor( function< Math::Color( float alpha, Math::Color ) > func ) { m_colorFunc = func; return *this; }
	Math::Color getColor() const { return m_color; }
	function< Math::Color( float, Math::Color ) > getColorFunction() const { return m_colorFunc; }

	Pattern& setLoop( bool loop ) { m_loop = loop; return *this; }
	bool getLoop() const { return m_loop; }

	Pattern& addPattern( Pattern pattern ) {
		m_sequence.push_back( pattern );
		return *this;
	}

	const vector< Pattern >& getSequence() { return m_sequence; }

protected:
	vector< Pattern > m_sequence;

	pair< milliseconds, milliseconds > m_lifetime{ 0ms, 0ms };
	pair< Math::Vec2, Math::Vec2 > m_position;
	pair< Math::Vec2, Math::Vec2 > m_velocity;
	pair< float, float > m_size{ 16.0f, 16.0f };
	pair< uint16_t, uint16_t > m_number{ 1u, 1u };
	pair< float, float > m_time{ 0.0f, 0.0f };
	Math::Color m_color;
	bool m_loop{ false };

	function< milliseconds( float, milliseconds ) > m_lifetimeFunc;
	function< Math::Vec2( float, Math::Vec2 ) > m_positionFunc;
	function< Math::Vec2( float, Math::Vec2 ) > m_velocityFunc;
	function< float( float, float ) > m_sizeFunc;
	function< Math::Color( float, Math::Color ) > m_colorFunc;
};

//--------------------------------------------------------------------------------

void spawnBullet( Object* world, Pattern pattern );

//--------------------------------------------------------------------------------

}
}

//================================================================================