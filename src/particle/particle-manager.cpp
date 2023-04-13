//================================================================================

#include "particle-manager.h"

//--------------------------------------------------------------------------------

#include "debug.h"
#include "system.h"

#include "particle-emitter.h"
#include "particle-system.h"
#include "particle-affector-manager.h"

#include "string-utils.h"

//================================================================================

namespace Gfx::Particle::Manager {

//--------------------------------------------------------------------------------

struct RenderGroup {
	bool active{ false };
	RenderProperties properties;
	sf::Color color{ sf::Color::White };
	list< Particle* > particles;

	bool operator==( const RenderProperties& rh ) {
		return active 
			&& properties.priority == rh.priority
			&& properties.texture == rh.texture;
	}
	bool operator<( const RenderGroup& rh ) {
		return properties.priority < rh.properties.priority;
	}
	operator bool() const {
		return active;
	}
};

//--------------------------------------------------------------------------------

constexpr size_t globalParticleLimit = 100000u;
constexpr size_t globalGroupLimit = 128u;
static const array< sf::Color, 8u > debugColors{ {
		sf::Color::White,
		sf::Color::Green,
		sf::Color::Red,
		sf::Color::Cyan,
		sf::Color::Magenta,
		sf::Color::Blue,
		sf::Color::Yellow,
} };

array< RenderGroup, 128u > groups;
stack< size_t > groupIDs;

array< Particle, globalParticleLimit > particles;
stack< size_t > particleIDs;
stack< size_t > fullParticleIDs;
size_t particleCount{ 0u };

//================================================================================

void init() {
	for( size_t i = globalParticleLimit; i != 0; --i )
		fullParticleIDs.push( i - 1u );
	particleIDs = fullParticleIDs;
	for( size_t i = globalGroupLimit; i != 0; --i )
		groupIDs.push( i - 1u );

	particles.fill( Particle() );

	Debug::addPerformancePage( "Particles",
							   [] {
								   string out;

								   out = Utils::format(
									   "Particles: %i\n"
									   "Groups: %i\n"
									   "\n"
									   "Next Particle ID: %i\n"
									   "Next Group ID: %i\n"
									   "\n"
									   "Global Particle Limit: %i\n"
									   "Global Group Limit: %i\n",
									   particleCount,
									   std::count( groups.begin(), groups.end(), true ),
									   particleIDs.top(),
									   groupIDs.top(),
									   globalParticleLimit,
									   globalGroupLimit
								   );

								   return out;
							   } );
}

//--------------------------------------------------------------------------------

void update( sf::Time delta ) {
	if( particleCount == 0u )
		return;

	Debug::startTimer( "Particle - Update" );

	size_t processedParticles = 0u;

	for( size_t i = 0; i < particles.size(); ++i ) {
		if( !particles.at( i ).alive )
			continue;

		Particle& particle = particles.at( i );

		// Handle lifetime
		particle.current.duration += microseconds( delta.asMicroseconds() );

		if( particle.initial.remaining > 0ms ) {
			particle.current.remaining -= microseconds( delta.asMicroseconds() );

			// Particle is dead
			if( particle.current.remaining <= 0ms )
				particle.alive = false;
		}
		if( particle.dead )
			particle.alive = false;

		particle.frame = particle.current;

		if( !particle.affectors.empty() )
			Affector::apply( &particle, delta );

		if( !particle.alive ) {
			particleIDs.push( i );
			particleCount--;
			processedParticles--;
		}

		processedParticles++;
		if( processedParticles == particleCount )
			break;
	}

	Debug::stopTimer( "Particle - Update" );

	Debug::startTimer( "Particle - Cleanup Groups" );
	for( size_t i = 0; i < groups.size(); ++i ) {
		if( !( groups.at( i ).active ) )
			continue;

		auto it = groups.at( i ).particles.begin();
		while( it != groups.at( i ).particles.end() ) {
			auto temp = it++;
			if( !( *temp )->alive )
				groups.at( i ).particles.erase( temp );
		}

		if( groups.at( i ).particles.empty() ) {
			groups.at( i ).active = false;
			groupIDs.push( i );
		}
	}
	Debug::stopTimer( "Particle - Cleanup Groups" );
}

//--------------------------------------------------------------------------------

void postUpdate( sf::Time delta ) {
	Debug::startTimer( "Particle - Post Update" );
	const float dt = delta.asSeconds();

	size_t processedParticles = 0u;
	for( Particle& particle : particles ) {
		if( particle.alive ) {
			particle.current.velocity += particle.frame.acceleration * dt;
			particle.frame.velocity += particle.frame.acceleration * dt;

			particle.current.transform.move( particle.frame.velocity.sf() * dt );
			particle.frame.transform.move( particle.frame.velocity.sf() * dt );

			particle.current.transform.rotate( particle.frame.spin * dt );
			particle.frame.transform.rotate( particle.frame.spin * dt );

			processedParticles++;
			if( processedParticles == particleCount )
				break;
		}
	}

	Debug::stopTimer( "Particle - Post Update" );
}

//--------------------------------------------------------------------------------

void render( sf::RenderTarget* target ) {
	 Debug::startTimer( "Particle - Render" );
	 for( size_t group = 0u; group < groups.size(); ++group ) {
		 if( !groups.at( group ).active )
			 continue;

		sf::RenderStates states;
		states.texture = &Sprite::get( groups.at( group ).properties.texture );
		states.blendMode = sf::BlendAdd;

		sf::Vector2u textureSize = states.texture->getSize();

		sf::VertexArray vertexArray;
		sf::VertexArray debugVertexArray;
		bool debug = ::System::getSystemInfo().drawDebug;

		for( Particle* particle : groups.at( group ).particles ) {
			if( !particle->alive )
				continue;

			sf::Vertex vertex;

			sf::Transform transform = particle->frame.transform.getTransform();

			std::array< sf::Vector2f, 4u > vertices = { {
				{ sf::Vector2f( 0.f, 0.f ) },
				{ sf::Vector2f( 1.f, 0.f ) },
				{ sf::Vector2f( 1.f, 1.f ) },
				{ sf::Vector2f( 0.f, 1.f ) }
			} };

			for( sf::Vector2f v : vertices ) {
				vertex.position = transform.transformPoint( v );
				vertex.color = particle->frame.color.sf();
				vertex.texCoords = sf::Vector2f( v.x * textureSize.x, v.y * textureSize.y );
				vertexArray.append( vertex );
			}

			if( debug ) {
				function< void( sf::Vector2f ) > makeVertex = [&debugVertexArray, &transform, &vertex, &group]( sf::Vector2f v ) {
					vertex.position = transform.transformPoint( v );
					vertex.color = groups.at( group ).color;
					debugVertexArray.append( vertex );
				};

				makeVertex( vertices[ 0 ] );
				makeVertex( vertices[ 1 ] );
				makeVertex( vertices[ 1 ] );
				makeVertex( vertices[ 2 ] );
				makeVertex( vertices[ 2 ] );
				makeVertex( vertices[ 3 ] );
				makeVertex( vertices[ 3 ] );
				makeVertex( vertices[ 0 ] );
			}
		}

		vertexArray.setPrimitiveType( sf::PrimitiveType::Quads );
		target->draw( vertexArray, states );
		Debug::incDrawCall();

		if( debug ) {
			debugVertexArray.setPrimitiveType( sf::Lines );
			target->draw( debugVertexArray );
			Debug::incDrawCall();
		}
	}
	Debug::stopTimer( "Particle - Render" );
}

//--------------------------------------------------------------------------------

void clearAll() {
	 if( particleCount == 0u )
		 return;

	 size_t processedParticles = 0u;
	 for( Particle& particle : particles ) {
		 if( !particle.alive )
			 continue;
		 particle.alive = false;
		 processedParticles++;

		 if( processedParticles == particleCount )
			 break;
	 }
	 for( RenderGroup& group : groups )
		 group.particles.clear();

	 particleCount = 0u;

	 particleIDs = fullParticleIDs;
	 groupIDs = stack< size_t >();
	 for( size_t i = globalGroupLimit; i != 0; --i )
		 groupIDs.push( i - 1u );
}

//--------------------------------------------------------------------------------

size_t getGroupID( RenderProperties properties ) {
	size_t out;

	const auto it = std::find( groups.begin(), groups.end(), properties );

	if( it == groups.end() ) {
		RenderGroup group;
		group.properties = properties;
		group.active = true;

		out = groupIDs.top();
		groupIDs.pop();

		if( out < debugColors.size() )
			group.color = debugColors.at( out );
		else
			group.color = Random::getColor( Math::Color( 0.f, 0.f, 0.f, 1.f ), Math::Color( 1.f, 1.f, 1.f, 1.f ), RandomColorType::MixRGB ).sf();

		groups.at( out ) = group;
	}
	else
		out = std::distance( groups.begin(), it );

	return out;
}

//--------------------------------------------------------------------------------

list< Particle* > spawnParticle( ParticlePattern pattern, sf::Transformable parentTransform ) {
	list< Particle* > out;

	Math::processSet( pattern.number );
	if( pattern.number.value == 0 )
		return out;

	RenderProperties properties;
	properties.priority = pattern.priority;
	properties.texture = Sprite::get( pattern.texture );

	size_t gId = getGroupID( properties );

	for( int i = 0; i < pattern.number.value; ++i ) {
		Particle particle = pattern.process( i, pattern.number.value );

		particle.group = gId;
		particle.alive = true;

		const size_t pId = particleIDs.top();
		particleIDs.pop();
		
		particles.at( pId ) = particle;
		out.push_back( &particles.at( pId ) );
		particleCount++;

		Affector::apply( &particle, sf::Time::Zero );
	}

	groups.at( gId ).particles.insert( groups.at( gId ).particles.end(), out.begin(), out.end() );

	return out;
}

//--------------------------------------------------------------------------------

list< Particle* > spawnParticle( ParticlePattern pattern, Particle* parent ) {
	list< Particle* > out;

	Math::processSet( pattern.number );

	RenderProperties properties;
	properties.priority = pattern.priority;
	properties.texture = Sprite::get( pattern.texture );

	size_t gId = getGroupID( properties );

	for( int i = 0; i < pattern.number.value; ++i ) {
		if( particleIDs.empty() )
			break;

		Particle particle = pattern.process( parent, i, pattern.number.value );

		particle.current = particle.initial;
		particle.group = gId;
		particle.alive = true;

		const size_t pId = particleIDs.top();
		particleIDs.pop();

		particles.at( pId ) = particle;
		out.push_back( &particles.at( pId ) );
		particleCount++;

		Affector::apply( &particle, sf::Time::Zero );
	}

	if( parent->parent != nullptr )
		parent->emitter->particles.insert( parent->emitter->particles.end(), out.begin(), out.end() );

	groups.at( gId ).particles.insert( groups.at( gId ).particles.end(), out.begin(), out.end() );

	return out;
}

//--------------------------------------------------------------------------------

list< Particle* > spawnParticle( ParticlePattern pattern, Emitter* parent ) {
	list< Particle* > out;

	Math::processSet( pattern.number );

	RenderProperties properties;
	properties.priority = pattern.priority;
	properties.texture = Sprite::get( pattern.texture );

	size_t gId = getGroupID( properties );

	for( int i = 0; i < pattern.number.value; ++i ) {
		Particle particle = pattern.process( i, pattern.number.value );

		if( parent != nullptr )
			particle.emitter = parent;
		particle.group = gId;
		particle.alive = true;

		const size_t pId = particleIDs.top();
		particleIDs.pop();

		particles.at( pId ) = particle;
		out.push_back( &particles.at( pId ) );
		particleCount++;

		Affector::apply( &particle, sf::Time::Zero );
	}

	if( parent != nullptr )
		parent->particles.insert( parent->particles.begin(), out.begin(), out.end() );

	groups.at( gId ).particles.insert( groups.at( gId ).particles.end(), out.begin(), out.end() );

	return out;
}

//--------------------------------------------------------------------------------

size_t getParticleCount() {
	return particleCount;
}

//--------------------------------------------------------------------------------

size_t getRenderGroupCount() {
	return groups.size();
}

//--------------------------------------------------------------------------------

}

//================================================================================