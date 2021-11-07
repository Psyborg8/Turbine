//================================================================================

#include "particle-affector.h"

#include "debug.h"
#include "mathtypes.h"

#include "json.h"
#include "string-utils.h"
#include "imgui-utils.h"

#include "particle.h"
#include "particle-emitter.h"
#include "particle-system.h"
#include "particle-loader.h"

#include "system.h"
#include "editor.h"

//================================================================================

namespace Gfx::Particle::Affector {

//--------------------------------------------------------------------------------

void Affector::affect( Emitter* target, sf::Time delta ) {
	for( Particle* particle : target->particles )
		affect( particle, delta );
}

//--------------------------------------------------------------------------------

void Affector::affect( System* target, sf::Time delta ) {
	for( Emitter& emitter : target->getEmitters() )
		affect( &emitter, delta );
}

//================================================================================

// PHYSICS

//================================================================================


void RotateToVelocityAffector::affect( Particle* target, sf::Time delta ) {
	const float angle = target->frame.velocity.angle() + m_offset;
	target->frame.transform.setRotation( angle );
}

//--------------------------------------------------------------------------------

bool RotateToVelocityAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Rotate To Velocity" );

	ImGui::Text( "Rotate To Velocity" );
	ImGui::Separator();

	out |= ImGui::render( m_offset, "offset" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > RotateToVelocityAffectorCreator::get() {
	Math::processSet( m_offset );
	return make_unique< RotateToVelocityAffector >( m_offset.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value RotateToVelocityAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void RotateToVelocityAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
}

//================================================================================

// SIN

//================================================================================

void SinPositionAffector::affect( Particle* target, sf::Time delta ) {
	const float alpha = ( float )target->frame.duration.count() / ( float )m_period.count() + m_offset;
	const float sin = ( cosf( ( alpha + 1.f ) * PI ) + 1.f ) / 2.f;
	const float amp = std::powf( sin, m_exponent );
	const sf::Vector2f pos = Math::mix( sf::Vector2f(), m_target.sf(), amp );

	target->frame.transform.move( pos );
}

//--------------------------------------------------------------------------------

bool SinPositionAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Sine Position" );

	ImGui::Text( "Sine Position" );
	ImGui::Separator();

	out |= ImGui::render( m_period, "Period (ms)" );
	out |= m_target.render();
	out |= ImGui::render( m_offset, "Offset" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > SinPositionAffectorCreator::get() {
	Math::processSet( m_period );
	Math::processSet( m_offset );
	Math::processSet( m_exponent );

	return make_unique< SinPositionAffector >( milliseconds( m_period.value ), m_target.process( 1, 1 ), m_offset.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value SinPositionAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "period", json::getValue( m_period ), json::getAllocator() );
	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );
	out.AddMember( "target", m_target.getValue(), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void SinPositionAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "period" ) )
		json::getValue( value[ "period" ], m_period );
	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
	if( value.HasMember( "target" ) )
		m_target.setValue( value[ "target" ] );
}

//================================================================================

void SinVelocityAffector::affect( Particle* target, sf::Time delta ) {
	const float alpha = ( float )target->frame.duration.count() / ( float )m_period.count() + m_offset;
	const float sin = ( cosf( ( alpha + 1.f ) * PI ) + 1.f ) / 2.f;
	const float amp = Math::mix( m_min, m_max, std::powf( sin, m_exponent ) );

	Math::Vec2 vel = target->frame.velocity * amp;
	target->frame.velocity = vel;
}

//--------------------------------------------------------------------------------

bool SinVelocityAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Sine Velocity" );

	ImGui::Text( "Sine Velocity" );
	ImGui::Separator();

	out |= ImGui::render( m_period, "Period (ms)" );
	out |= ImGui::render( m_min, "Min" );
	out |= ImGui::render( m_max, "Max" );
	out |= ImGui::render( m_offset, "Offset" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > SinVelocityAffectorCreator::get() {
	Math::processSet( m_period );
	Math::processSet( m_min );
	Math::processSet( m_max );
	Math::processSet( m_offset );
	Math::processSet( m_exponent );

	return make_unique< SinVelocityAffector >( milliseconds( m_period.value ), m_min.value, m_max.value, m_offset.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value SinVelocityAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "period", json::getValue( m_period ), json::getAllocator() );
	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );
	out.AddMember( "min", json::getValue( m_min ), json::getAllocator() );
	out.AddMember( "max", json::getValue( m_max ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void SinVelocityAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "period" ) )
		json::getValue( value[ "period" ], m_period );
	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
	if( value.HasMember( "min" ) )
		json::getValue( value[ "min" ], m_min );
	if( value.HasMember( "max" ) )
		json::getValue( value[ "max" ], m_max );
}

//================================================================================

void SinAlphaAffector::affect( Particle* target, sf::Time delta ) {
	if( m_period == 0ms )
		return;

	const float alpha = ( float )target->frame.duration.count() / ( float )m_period.count() + m_offset;
	const float sin = ( cosf( ( alpha + 1.f ) * PI ) + 1.f ) / 2.f;
	const float a = Math::mix( target->frame.color.a, m_target, std::powf( sin, m_exponent ) );

	target->frame.color.a = a;
}

//--------------------------------------------------------------------------------

bool SinAlphaAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Sine Alpha" );

	ImGui::Text( "Sine Alpha" );
	ImGui::Separator();

	out |= ImGui::render( m_period, "Period (ms)" );
	out |= ImGui::render( m_target, "Target" );
	out |= ImGui::render( m_offset, "Offset" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > SinAlphaAffectorCreator::get() {
	Math::processSet( m_period );
	Math::processSet( m_target );
	Math::processSet( m_offset );
	Math::processSet( m_exponent );

	return make_unique< SinAlphaAffector >( milliseconds( m_period.value ), m_target.value, m_offset.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value SinAlphaAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "period", json::getValue( m_period ), json::getAllocator() );
	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );
	out.AddMember( "target", json::getValue( m_target ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void SinAlphaAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "period" ) )
		json::getValue( value[ "period" ], m_period );
	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
	if( value.HasMember( "target" ) )
		json::getValue( value[ "target" ], m_target );
}

//================================================================================

void SinColorAffector::affect( Particle* target, sf::Time delta ) {
	const float alpha = ( float )target->frame.duration.count() / ( float )m_period.count() + m_offset;
	const float sin = ( cosf( ( alpha + 1.f ) * PI ) + 1.f ) / 2.f;
	const Math::Color color = Math::mix( target->frame.color, m_target, std::powf( sin, m_exponent ) );

	target->frame.color = color;
}

//--------------------------------------------------------------------------------

bool SinColorAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Sine Color" );

	ImGui::Text( "Sine Color" );
	ImGui::Separator();

	out |= ImGui::render( m_period, "Period (ms)" );
	out |= ImGui::render( m_target, "Target" );
	out |= ImGui::render( m_offset, "Offset" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > SinColorAffectorCreator::get() {
	Math::processSet( m_period );
	Math::processSet( m_target );
	Math::processSet( m_offset );
	Math::processSet( m_exponent );

	return make_unique< SinColorAffector >( milliseconds( m_period.value ), m_target.value, m_offset.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value SinColorAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "period", json::getValue( m_period ), json::getAllocator() );
	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );
	out.AddMember( "target", json::getValue( m_target ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void SinColorAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "period" ) )
		json::getValue( value[ "period" ], m_period );
	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
	if( value.HasMember( "target" ) )
		json::getValue( value[ "target" ], m_target );
}

//================================================================================

void SinSpinAffector::affect( Particle* target, sf::Time delta ) {
	const float alpha = ( float )target->frame.duration.count() / ( float )m_period.count() + m_offset;
	const float sin = ( cosf( ( alpha + 1.f ) * PI ) + 1.f ) / 2.f;
	const float mul = Math::mix( m_min, m_max, std::powf( sin, m_exponent ) );

	target->frame.spin = target->frame.spin * mul;
}

//--------------------------------------------------------------------------------

bool SinSpinAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Sine Spin" );

	ImGui::Text( "Sine Spin" );
	ImGui::Separator();

	out |= ImGui::render( m_period, "Period (ms)" );
	out |= ImGui::render( m_min, "Min" );
	out |= ImGui::render( m_max, "Max" );
	out |= ImGui::render( m_offset, "Offset" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > SinSpinAffectorCreator::get() {
	Math::processSet( m_period );
	Math::processSet( m_min );
	Math::processSet( m_max );
	Math::processSet( m_offset );
	Math::processSet( m_exponent );

	return make_unique< SinSpinAffector >( milliseconds( m_period.value ), m_min.value, m_max.value, m_offset.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value SinSpinAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "period", json::getValue( m_period ), json::getAllocator() );
	out.AddMember( "offset", json::getValue( m_offset ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );
	out.AddMember( "min", json::getValue( m_min ), json::getAllocator() );
	out.AddMember( "max", json::getValue( m_max ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void SinSpinAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "period" ) )
		json::getValue( value[ "period" ], m_period );
	if( value.HasMember( "offset" ) )
		json::getValue( value[ "offset" ], m_offset );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
	if( value.HasMember( "min" ) )
		json::getValue( value[ "min" ], m_min );
	if( value.HasMember( "max" ) )
		json::getValue( value[ "max" ], m_max );
}

//================================================================================

// FADES

//================================================================================

void FadeVelocityAffector::affect( Particle* target, sf::Time delta ) {
	float alpha = 1.0f - ( ( float )target->current.remaining.count() / ( float )target->initial.remaining.count() );
	alpha = std::powf( alpha, m_exponent );
	target->frame.velocity = target->frame.velocity * Math::mix( m_start, m_end, alpha );
}

//--------------------------------------------------------------------------------

bool FadeVelocityAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Fade Velocity" );

	ImGui::Text( "Fade Velocity" );
	ImGui::Separator();

	out |= ImGui::render( m_start, "Start" );
	out |= ImGui::render( m_end, "End" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > FadeVelocityAffectorCreator::get() {
	Math::processSet( m_start );
	Math::processSet( m_end );
	Math::processSet( m_exponent );

	return make_unique< FadeVelocityAffector >( m_start.value, m_end.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value FadeVelocityAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", json::getValue( m_start ), json::getAllocator() );
	out.AddMember( "end", json::getValue( m_end ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void FadeVelocityAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "start" ) )
		json::getValue( value[ "start" ], m_start );
	if( value.HasMember( "end" ) )
		json::getValue( value[ "end" ], m_end );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
}

//================================================================================

void FadeScaleAffector::affect( Particle* target, sf::Time delta ) {
	float alpha = 1.0f - ( ( float )target->current.remaining.count() / ( float )target->initial.remaining.count() );
	alpha = std::powf( alpha, m_exponent );
	target->frame.transform.setScale( target->frame.transform.getScale() * Math::mix( m_start, m_end, alpha ) );
}

//--------------------------------------------------------------------------------

bool FadeScaleAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Fade Scale" );

	ImGui::Text( "Fade Scale" );
	ImGui::Separator();

	out |= ImGui::render( m_start, "Start" );
	out |= ImGui::render( m_end, "End" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > FadeScaleAffectorCreator::get() {
	Math::processSet( m_start );
	Math::processSet( m_end );
	Math::processSet( m_exponent );

	return make_unique< FadeScaleAffector >( m_start.value, m_end.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value FadeScaleAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "start", json::getValue( m_start ), json::getAllocator() );
	out.AddMember( "end", json::getValue( m_end ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void FadeScaleAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "start" ) )
		json::getValue( value[ "start" ], m_start );
	if( value.HasMember( "end" ) )
		json::getValue( value[ "end" ], m_end );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
}

//================================================================================

void FadeAlphaAffector::affect( Particle* target, sf::Time delta ) {
	float alpha = 1.0f - ( ( float )target->current.remaining.count() / ( float )target->initial.remaining.count() );
	alpha = std::powf( alpha, m_exponent );
	target->frame.color.a = Math::mix( target->frame.color.a, m_target, alpha );
}

//--------------------------------------------------------------------------------

bool FadeAlphaAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Fade Alpha" );

	ImGui::Text( "Fade Alpha" );
	ImGui::Separator();

	out |= ImGui::render( m_target, "Target" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > FadeAlphaAffectorCreator::get() {
	Math::processSet( m_target );
	Math::processSet( m_exponent );

	return make_unique< FadeAlphaAffector >( m_target.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value FadeAlphaAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "target", json::getValue( m_target ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void FadeAlphaAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "target" ) )
		json::getValue( value[ "target" ], m_target );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
}

//================================================================================

void FadeColorAffector::affect( Particle* target, sf::Time delta ) {
	float alpha = 1.0f - ( ( float )target->current.remaining.count() / ( float )target->initial.remaining.count() );
	alpha = std::powf( alpha, m_exponent );
	target->frame.color = Math::mix( target->frame.color, m_target, alpha );
	target->frame.color.a = Math::mix( target->frame.color.a, m_target.a, alpha );
}

//--------------------------------------------------------------------------------

bool FadeColorAffectorCreator::render() {
	bool out = false;
	ImGui::PushID( "Fade Color" );

	ImGui::Text( "Fade Color" );
	ImGui::Separator();

	out |= ImGui::render( m_target, "Target" );
	out |= ImGui::render( m_exponent, "Exponent" );

	ImGui::PopID();
	return out;
}

//--------------------------------------------------------------------------------

shared_ptr< Affector > FadeColorAffectorCreator::get() {
	Math::processSet( m_target );
	Math::processSet( m_exponent );

	return make_unique< FadeColorAffector >( m_target.value, m_exponent.value );
}

//--------------------------------------------------------------------------------

rapidjson::Value FadeColorAffectorCreator::getValue() {
	rapidjson::Value out;
	out.SetObject();

	out.AddMember( "target", json::getValue( m_target ), json::getAllocator() );
	out.AddMember( "exponent", json::getValue( m_exponent ), json::getAllocator() );

	return out;
}

//--------------------------------------------------------------------------------

void FadeColorAffectorCreator::setValue( const rapidjson::Value& value ) {
	if( !value.IsObject() )
		return;

	if( value.HasMember( "target" ) )
		json::getValue( value[ "target" ], m_target );
	if( value.HasMember( "exponent" ) )
		json::getValue( value[ "exponent" ], m_exponent );
}

//================================================================================

// Emission

//================================================================================

void OnDeathEmitterAffector::affect( Particle* target, sf::Time delta ) {
	if( target->alive || m_patterns.empty() )
		return;

	if( m_type == EmitterType::Set ) {
		for( ParticlePattern* pattern : m_patterns )
			Manager::spawnParticle( *pattern, target );
	}
	else if( m_type == EmitterType::Random ) {
		Manager::spawnParticle( *Random::getElement( m_patterns ), target );
	}
}

//--------------------------------------------------------------------------------

void EmitterAffector::affect( Particle* target, sf::Time delta ) {
	if( m_finished || m_patterns.empty() )
		return;

	m_elapsed += microseconds( delta.asMicroseconds() );

	if( m_elapsed > m_next ) {
		Math::processSet( m_spawnRate );
		if( m_spawnRate.value != 0.f )
			m_next = m_elapsed + milliseconds( ( int )( 1000.f / m_spawnRate.value ) );
		else
			m_duration = -1ms;

		switch( m_type ) {
		case EmitterType::Set:
			for( ParticlePattern* pattern : m_patterns )
				Manager::spawnParticle( *pattern, target );
			break;
		case EmitterType::Random:
			Manager::spawnParticle( *Random::getElement( m_patterns ), target );
			break;
		case EmitterType::Sequence:
			Manager::spawnParticle( *m_patterns.at( m_index++ ) );
			if( m_index == m_patterns.size() )
				m_index = 0u;
		}
	}

	if( m_duration != 0ms && m_elapsed > m_duration )
		m_finished = true;
}

//================================================================================

// Inheritance

//================================================================================

void InheritLifetimeAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy:
	case InheritanceType::Offset:
	case InheritanceType::Multiply:
		target->alive = target->parent->alive;
	}
}

//--------------------------------------------------------------------------------

void InheritPositionAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.transform.setPosition( target->parent->frame.transform.getPosition() ); break;
	case InheritanceType::Offset: target->frame.transform.move( target->parent->frame.transform.getPosition() ); break;
	case InheritanceType::Multiply:
		Math::Vec2 pos = target->frame.transform.getPosition();
		pos *= target->parent->frame.transform.getPosition();
		target->frame.transform.move( pos.sf() );
		break;
	}
}

//--------------------------------------------------------------------------------

void InheritRotationAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.transform.setRotation( target->parent->frame.transform.getRotation() ); break;
	case InheritanceType::Offset: target->frame.transform.rotate( target->parent->frame.transform.getRotation() ); break;
	case InheritanceType::Multiply:
		float rot = target->frame.transform.getRotation();
		rot *= target->parent->frame.transform.getRotation();
		target->frame.transform.rotate( rot );
		break;
	}
}

//--------------------------------------------------------------------------------

void InheritSpinAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.spin = target->parent->frame.spin; break;
	case InheritanceType::Offset: target->frame.spin += target->parent->frame.spin; break;
	case InheritanceType::Multiply: target->frame.spin *= target->parent->frame.spin; break;
	}
}

//--------------------------------------------------------------------------------

void InheritScaleAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.transform.setScale( target->parent->frame.transform.getScale() ); break;
	case InheritanceType::Offset: target->frame.transform.scale( target->parent->frame.transform.getScale() ); break;
	case InheritanceType::Multiply:
		Math::Vec2 s = target->frame.transform.getScale();
		s *= target->parent->frame.transform.getScale();
		target->frame.transform.scale( s.sf() );
		break;
	}
}

//--------------------------------------------------------------------------------

void InheritColorAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy:
		target->frame.color.r = target->parent->frame.color.r;
		target->frame.color.g = target->parent->frame.color.g;
		target->frame.color.b = target->parent->frame.color.b;
		break;
	case InheritanceType::Offset:
		target->frame.color.r += target->parent->frame.color.r;
		target->frame.color.g += target->parent->frame.color.g;
		target->frame.color.b += target->parent->frame.color.b;
		break;
	case InheritanceType::Multiply:
		target->frame.color.r *= target->parent->frame.color.r;
		target->frame.color.g *= target->parent->frame.color.g;
		target->frame.color.b *= target->parent->frame.color.b;
		break;
	}
}

//--------------------------------------------------------------------------------

void InheritAlphaAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy:
		target->frame.color.a = target->parent->frame.color.a;
		break;
	case InheritanceType::Offset:
		target->frame.color.a += target->parent->frame.color.a;
		break;
	case InheritanceType::Multiply:
		target->frame.color.a *= target->parent->frame.color.a;
		break;
	}
}

//--------------------------------------------------------------------------------

void InheritVelocityAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.velocity = target->parent->frame.velocity; break;
	case InheritanceType::Offset: target->frame.velocity += target->parent->frame.velocity; break;
	case InheritanceType::Multiply: target->frame.velocity *= target->parent->frame.velocity; break;
	}

	if( m_scale ) {
		sf::Transform t;
		t.scale( target->parent->frame.transform.getScale() );
		target->frame.velocity = t.transformPoint( target->frame.velocity.sf() );
	}
	if( m_rotate ) {
		sf::Transform t;
		t.rotate( target->parent->frame.transform.getRotation() );
		target->frame.velocity = t.transformPoint( target->frame.velocity.sf() );
	}
}

//--------------------------------------------------------------------------------

void InheritAccelerationAffector::affect( Particle* target, sf::Time delta ) {
	if( target->parent == nullptr )
		return;

	switch( m_type ) {
	case InheritanceType::None: break;
	case InheritanceType::Copy: target->frame.acceleration = target->parent->frame.acceleration; break;
	case InheritanceType::Offset: target->frame.acceleration += target->parent->frame.acceleration; break;
	case InheritanceType::Multiply: target->frame.acceleration *= target->parent->frame.acceleration; break;
	}
}

//--------------------------------------------------------------------------------

}

//================================================================================