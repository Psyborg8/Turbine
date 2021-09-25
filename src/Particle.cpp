//================================================================================

#include "Particle.h"

//--------------------------------------------------------------------------------

#include "Debug.h"
#include "World.h"
#include "Timer.h"
#include "System.h"
#include "Random.h"
#include "Emitter.h"
#include "Particle.h"
#include "Shader.h"
#include "Sprite.h"

//================================================================================

namespace Gfx {
namespace Particle {

//================================================================================

vector< shared_ptr< Particle > > particles;
vector< Pattern > patterns;

//================================================================================

void Particle::onUpdate( sf::Time deltaTime ) {
	if( m_duration >= microseconds( std::numeric_limits< int32_t >::min() ) ) {
		m_duration -= microseconds( deltaTime.asMicroseconds() );
		if( m_duration <= 0s )
			return destroy();

		m_alpha = 1.0f - float( duration_cast< milliseconds >( m_duration ).count() ) / float( m_pattern.properties.lifetime.value );
	}

	// Spin
	float rotation = m_sprite->getRotation();
	if( m_pattern.physics.rotation.fade.active )
		rotation = Math::mix( m_pattern.physics.rotation.rotation.value, m_pattern.physics.rotation.fade.target.value, m_alpha );

	float spin = m_pattern.physics.rotation.spin.spin.value;
	if( m_pattern.physics.rotation.spin.fade.active )
		spin *= Math::mix( m_pattern.physics.rotation.spin.fade.start.value, m_pattern.physics.rotation.spin.fade.end.value, m_alpha );

	rotation += spin * deltaTime.asSeconds();
	m_sprite->setRotation( rotation );

	// Color
	Math::Color color = m_pattern.shape.color.color.value;
	if( m_pattern.shape.color.fade.r )
		color.r = Math::mix( m_pattern.shape.color.color.value.r, m_pattern.shape.color.fade.target.value.r, m_alpha );
	if( m_pattern.shape.color.fade.g )
		color.g = Math::mix( m_pattern.shape.color.color.value.g, m_pattern.shape.color.fade.target.value.g, m_alpha );
	if( m_pattern.shape.color.fade.b )
		color.b = Math::mix( m_pattern.shape.color.color.value.b, m_pattern.shape.color.fade.target.value.b, m_alpha );
	if( m_pattern.shape.color.fade.a )
		color.a = Math::mix( m_pattern.shape.color.color.value.a, m_pattern.shape.color.fade.target.value.a, m_alpha );

	m_sprite->setFillColor( color.sf() );

	// Origin
	Math::Vec2 origin = m_origin;
	Math::Vec2 originVelocity = processVelocitySet( m_pattern.shape.origin.velocity, m_pattern.shape.origin.velocity.startValue, m_sprite->getOrigin() );
	origin += originVelocity * deltaTime.asSeconds();

	setOrigin( origin.sf() );

	// Size
	Math::Vec2 size = m_pattern.shape.size.size.value;
	if( m_pattern.shape.size.fade.x )
		size.x *= Math::mix( m_pattern.shape.size.fade.start.value, m_pattern.shape.size.fade.end.value, m_alpha );
	if( m_pattern.shape.size.fade.y )
		size.y *= Math::mix( m_pattern.shape.size.fade.start.value, m_pattern.shape.size.fade.end.value, m_alpha );

	m_sprite->setScale( size.sf() );

	// Velocity
	Math::Vec2 velocity = processVelocitySet( m_pattern.physics.velocity, getVelocity(), getPosition() );
	Math::Vec2 acceleration = processVelocitySet( m_pattern.physics.acceleration, m_pattern.physics.acceleration.startValue, getPosition() );

	velocity += acceleration * deltaTime.asSeconds();
	setVelocity( velocity );
}

//--------------------------------------------------------------------------------

void Particle::onPostUpdate( sf::Time deltaTime ) {
	for( shared_ptr< Emitter::Emitter > emitter : m_emitters )
		if( !emitter->isDead() )
			emitter->setPosition( getPosition() );

	m_sprite->move( getVelocity().sf() * deltaTime.asSeconds() );
}

//--------------------------------------------------------------------------------

void Particle::onRender() {
	sf::RenderStates states;
	states.blendMode = sf::BlendAdd;

	if( m_pattern.shape.type == Pattern::Shape::Type::Texture ) {
		sf::Shader& shader = Gfx::Shader::get( m_pattern.shape.shader );
		shader.setUniform( "texture", sf::Shader::CurrentTexture );
		shader.setUniform( "alpha", m_alpha );
		shader.setUniform( "color", sf::Glsl::Vec4( m_sprite->getFillColor() ) );
		states.shader = &shader;
	}
		
	System::getWindow()->draw( *m_sprite, states );
	if( System::getSystemInfo().drawDebug ) {
		//
	}

	Debug::incDrawCall();
}

//--------------------------------------------------------------------------------

void Particle::onDestroy() {
	m_dead = true;
}

//--------------------------------------------------------------------------------

void Particle::init( const Pattern& pattern, int idx ) {
	setName( "Bullet" );
	setVisibility( true );

	m_pattern = pattern;

	/* Properties */

	// Lifetime
	Math::processSet( m_pattern.properties.lifetime );

	if( m_pattern.properties.lifetime.value == 0 )
		m_duration = microseconds( std::numeric_limits< long long >::max() );
	else
		m_duration = milliseconds( m_pattern.properties.lifetime.value );

	/* Shape */
	Math::processSet( m_pattern.shape.color.color );
	Math::processSet( m_pattern.shape.color.fade.target );

	Math::processSet( m_pattern.shape.size.size );
	Math::processSet( m_pattern.shape.size.fade.start );
	Math::processSet( m_pattern.shape.size.fade.end );
	Math::processSet( m_pattern.shape.outline.color.color );
	Math::processSet( m_pattern.shape.outline.color.fade.target );
	Math::processSet( m_pattern.shape.outline.thickness.thickness );
	Math::processSet( m_pattern.shape.outline.thickness.fade.start );
	Math::processSet( m_pattern.shape.outline.thickness.fade.end );

	Math::processSet( m_pattern.shape.origin.position.position );
	Math::processSet( m_pattern.shape.origin.position.circle.radius );
	Math::processSet( m_pattern.shape.origin.position.rectangle.size );
	Math::processSet( m_pattern.shape.origin.velocity.direction.direction );
	Math::processSet( m_pattern.shape.origin.velocity.direction.power );
	Math::processSet( m_pattern.shape.origin.velocity.direction.fade.target );
	Math::processSet( m_pattern.shape.origin.velocity.direction.fade.start );
	Math::processSet( m_pattern.shape.origin.velocity.direction.fade.end );
	Math::processSet( m_pattern.shape.origin.velocity.value.value );
	Math::processSet( m_pattern.shape.origin.velocity.value.fade.target );
	Math::processSet( m_pattern.shape.origin.velocity.point.point );
	Math::processSet( m_pattern.shape.origin.velocity.point.power );
	Math::processSet( m_pattern.shape.origin.velocity.point.fade.target );
	Math::processSet( m_pattern.shape.origin.velocity.point.fade.start );
	Math::processSet( m_pattern.shape.origin.velocity.point.fade.end );
	Math::processSet( m_pattern.shape.origin.velocity.point.scale.factor );

	if( m_pattern.shape.type == Pattern::Shape::Type::Circle ) {
		shared_ptr< sf::CircleShape > shape = make_unique< sf::CircleShape >();
		shape->setRadius( 16.f );
		shape->setOutlineThickness( m_pattern.shape.outline.thickness.thickness.value );
		shape->setOutlineColor( m_pattern.shape.outline.color.color.value.sf() );
		m_sprite = shape;
	}
	else {
		shared_ptr< sf::RectangleShape > shape = make_unique< sf::RectangleShape >();
		shape->setSize( sf::Vector2f( 32.f, 32.f ) );
		shape->setOutlineThickness( m_pattern.shape.outline.thickness.thickness.value );
		shape->setOutlineColor( m_pattern.shape.outline.color.color.value.sf() );

		if( m_pattern.shape.type == Pattern::Shape::Type::Texture ) {
			char buffer[ 1024 ];
			sprintf_s( buffer, "%s%s.png", Folders::Particles.c_str(), m_pattern.shape.texture.c_str() );
			shape->setTexture( &Gfx::Sprite::get( buffer ) );
		}

		m_sprite = shape;
	}

	// Origin
	Math::Vec2 origin = processPositionSet( m_pattern.shape.origin.position, idx );
	setOrigin( origin );
	initializeVelocitySet( m_pattern.shape.origin.velocity, origin );

	// Color
	m_sprite->setFillColor( m_pattern.shape.color.color.value.sf() );

	// Size
	Math::Vec2 size = m_pattern.shape.size.size.value;
	if( m_pattern.shape.size.fade.x )
		size.x *= Math::mix( m_pattern.shape.size.fade.start.value, m_pattern.shape.size.fade.end.value, 0.f );
	if( m_pattern.shape.size.fade.y )
		size.y *= Math::mix( m_pattern.shape.size.fade.start.value, m_pattern.shape.size.fade.end.value, 0.f );

	m_sprite->setScale( size.sf() );

	/* Physics */
	Math::processSet( m_pattern.physics.position.position );
	Math::processSet( m_pattern.physics.position.circle.radius );
	Math::processSet( m_pattern.physics.position.rectangle.size );

	Math::processSet( m_pattern.physics.velocity.direction.direction );
	Math::processSet( m_pattern.physics.velocity.direction.power );
	Math::processSet( m_pattern.physics.velocity.direction.fade.target );
	Math::processSet( m_pattern.physics.velocity.direction.fade.start );
	Math::processSet( m_pattern.physics.velocity.direction.fade.end );
	Math::processSet( m_pattern.physics.velocity.value.value );
	Math::processSet( m_pattern.physics.velocity.value.fade.target );
	Math::processSet( m_pattern.physics.velocity.point.point );
	Math::processSet( m_pattern.physics.velocity.point.power );
	Math::processSet( m_pattern.physics.velocity.point.fade.target );
	Math::processSet( m_pattern.physics.velocity.point.fade.start );
	Math::processSet( m_pattern.physics.velocity.point.fade.end );
	Math::processSet( m_pattern.physics.velocity.point.scale.factor );

	Math::processSet( m_pattern.physics.acceleration.direction.direction );
	Math::processSet( m_pattern.physics.acceleration.direction.power );
	Math::processSet( m_pattern.physics.acceleration.direction.fade.target );
	Math::processSet( m_pattern.physics.acceleration.direction.fade.start );
	Math::processSet( m_pattern.physics.acceleration.direction.fade.end );
	Math::processSet( m_pattern.physics.acceleration.value.value );
	Math::processSet( m_pattern.physics.acceleration.value.fade.target );
	Math::processSet( m_pattern.physics.acceleration.point.point );
	Math::processSet( m_pattern.physics.acceleration.point.power );
	Math::processSet( m_pattern.physics.acceleration.point.fade.target );
	Math::processSet( m_pattern.physics.acceleration.point.fade.start );
	Math::processSet( m_pattern.physics.acceleration.point.fade.end );
	Math::processSet( m_pattern.physics.acceleration.point.scale.factor );

	Math::processSet( m_pattern.physics.rotation.rotation );
	Math::processSet( m_pattern.physics.rotation.fade.target );
	Math::processSet( m_pattern.physics.rotation.spin.spin );
	Math::processSet( m_pattern.physics.rotation.spin.fade.start );
	Math::processSet( m_pattern.physics.rotation.spin.fade.end );

	// Position
	m_sprite->setPosition( processPositionSet( m_pattern.physics.position, idx ).sf() );

	// Velocity
	initializeVelocitySet( m_pattern.physics.velocity, getPosition() );
	setVelocity( m_pattern.physics.velocity.startValue );

	initializeVelocitySet( m_pattern.physics.acceleration, getPosition() );

	// Rotation
	m_sprite->rotate( m_pattern.physics.rotation.rotation.value );

	/* Emitters */
	for( Emitter::Pattern emitter : m_pattern.emitters )
		m_emitters.push_back( Emitter::spawn( this, emitter, Math::Vec2(), duration_cast< milliseconds >( m_duration ) ) );
}

//--------------------------------------------------------------------------------

Math::Vec2 Particle::processVelocitySet( const VelocitySet& set, Math::Vec2 current, Math::Vec2 position ) {
	Math::Vec2 out = current;
	if( set.type == VelocitySet::Type::Direction ) {
		if( set.direction.fade.direction ) {
			Math::Vec2 direction = Math::mix( set.direction.direction.value, set.direction.fade.target.value, m_alpha );
			out = direction.normalize() * out.length();
		}

		if( set.direction.fade.power ) {
			float power = out.length();
			power *= Math::mix( set.direction.fade.start.value, set.direction.fade.end.value, m_alpha );
			out = out.normalize() * power;
		}

		return out;
	}

	if( set.type == VelocitySet::Type::Value ) {
		if( set.value.fade.x )
			out.x = Math::mix( set.value.value.value.x, set.value.fade.target.value.x, m_alpha );
		if( set.value.fade.y )
			out.y = Math::mix( set.value.value.value.y, set.value.fade.target.value.y, m_alpha );

		return out;
	}

	Math::Vec2 target;
	if( set.type == VelocitySet::Type::Point ) {
		if( set.point.fade.point )
			target = Math::mix( set.point.point.value, set.point.fade.target.value, m_alpha );
		else
			target = set.point.point.value;
	}
	else
		target = getWorld()->getPlayer()->getPosition();

	float power = out.length();
	if( set.point.fade.power )
		power = set.point.power.value * Math::mix( set.point.fade.start.value, set.point.fade.end.value, m_alpha );
	if( set.point.scale.active )
		if( set.point.scale.inverse )
			power /= ( target - position ).length() * set.point.scale.factor.value;
		else
			power *= ( target - position ).length() * set.point.scale.factor.value;

	if( ( set.type == VelocitySet::Type::Player && ( set.point.track ) )
		|| ( set.type == VelocitySet::Point && ( set.point.fade.point ) )
		|| set.point.fade.power
		|| set.point.scale.active ) 
	{
		out = ( target - position ).normalize() * power;
	}

	return out;
}

//--------------------------------------------------------------------------------

void Particle::initializeVelocitySet( VelocitySet& set, Math::Vec2 position ) {
	// Velocity
	if( set.type == VelocitySet::Type::Direction ) {
		float speed = set.direction.power.value;
		if( set.direction.fade.power )
			speed *= set.direction.fade.start.value;

		const Math::Vec2 direction = set.direction.direction.value;

		set.startValue = direction.normalize() * speed;
	}
	else if( set.type == VelocitySet::Type::Value ) {
		set.startValue = set.value.value.value;
	}
	else {
		Math::Vec2 point;

		if( set.type == VelocitySet::Type::Point )
			point = set.point.point.value;
		else
			point = getWorld()->getPlayer()->getPosition();

		const Math::Vec2 direction = point - position;

		float power = set.point.power.value;

		if( set.point.scale.active )
			if( set.point.scale.inverse )
				power /= direction.length() * set.point.scale.factor.value;
			else
				power *= direction.length() * set.point.scale.factor.value;
		if( set.point.fade.power )
			power *= set.point.fade.start.value;

		set.startValue = direction.normalize() * power;
	}
}

//--------------------------------------------------------------------------------

Math::Vec2 Particle::processPositionSet( const PositionSet& set, int idx ) {
	Math::Vec2 out;

	if( set.type == PositionSet::Type::Point )
		return set.position.value.sf();
	else if( set.type == PositionSet::Type::Circle ) {
		float distance;
		if( set.circle.fill )
			distance = Random::getRandomFloatInRange( 0.f, set.circle.radius.value );
		else
			distance = set.circle.radius.value;

		float angle;
		if( set.circle.uniform )
			angle = 360.f / m_pattern.properties.number.value * idx;
		else
			angle = Random::getRandomFloatInRange( 0.f, 360.f );

		const float radians = ( angle * PI ) / 180.f;

		const Math::Vec2 dist{ cos( radians ), sin( radians ) };

		return set.position.value + dist * distance;
	}
	else if( set.type == PositionSet::Type::Rectangle ) {
		const Math::Vec2 position = set.position.value;
		const Math::Vec2 size = set.rectangle.size.value;

		if( set.rectangle.fill ) {
			const Math::Vec2 topLeft = position - size / 2.f;
			const Math::Vec2 bottomRight = position + size / 2.f;

			return Random::getRandomVec2InRange( topLeft, bottomRight ).sf();
		}
		else
		{
			float alpha;
			if( set.rectangle.uniform )
				alpha = 1.f / m_pattern.properties.number.value * idx;
			else
				alpha = Random::getRandomFloatInRange( 0.f, 1.f );

			const float length = 2 * size.x + 2 * size.y;
			const float distance = length * alpha;

			Math::Vec2 pos;
			if( distance <= size.x ) {
				pos.y = position.y;
				pos.x = position.x + distance;
			}
			else if( distance <= size.x + size.y ) {
				pos.x = position.x + size.x;
				pos.y = position.y + ( distance - size.x );
			}
			else if( distance <= ( 2.f * size.x ) + size.y ) {
				pos.y = position.y + size.y;
				pos.x = position.x + ( distance - ( size.x + size.y ) );
			}
			else {
				pos.x = position.x;
				pos.y = position.y + ( distance - ( ( 2.f * size.x ) + size.y ) );
			}

			return pos;
		}
	}

	return Math::Vec2();
}

//================================================================================

void spawn( Object* parent, Pattern pattern, Math::Vec2 position ) {
	Math::processSet( pattern.properties.number );

	for( uint16_t i = 0u; i < pattern.properties.number.value; ++i ) {
		shared_ptr< Particle > particle = Object::makeObject< Particle >( parent );
		particle->init( pattern, i );
		particles.push_back( particle );
	}
}

//--------------------------------------------------------------------------------

void killAll() {
	for( shared_ptr< Particle > particle : particles )
		if( particle != nullptr ) {
			particle->kill();
			particle->destroy();
		}
}

//--------------------------------------------------------------------------------

void cleanup() {
	for( size_t i = 0u; i < particles.size(); ++i ) {
		if( !particles.at( i )->isDead() )
			continue;

		particles.erase( particles.begin() + i );
		--i;
	}
}

//--------------------------------------------------------------------------------

vector< shared_ptr< Particle > > getAll() {
	return particles;
}

//--------------------------------------------------------------------------------

}
}

//================================================================================
