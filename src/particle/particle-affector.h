//================================================================================

#pragma once

//================================================================================

#include "debug.h"
#include "global.h"
#include "mathtypes.h"

#include <rapidjson/document.h>

//================================================================================

namespace Gfx::Particle {

struct Particle;
struct Emitter;
class System;
struct ParticlePattern;

//--------------------------------------------------------------------------------

namespace Affector {

//--------------------------------------------------------------------------------

class Affector {
public:
	Affector()			= default;
	virtual ~Affector() = default;

	virtual void affect( Particle* target, sf::Time delta ) {}
	virtual void affect( Emitter* target, sf::Time delta );
	virtual void affect( System* target, sf::Time delta );
};

//--------------------------------------------------------------------------------

class AffectorCreator {
public:
	AffectorCreator()		   = default;
	virtual ~AffectorCreator() = default;

	virtual bool render()				 = 0;
	virtual shared_ptr< Affector > get() = 0;

	virtual rapidjson::Value getValue()					   = 0;
	virtual void setValue( const rapidjson::Value& value ) = 0;

	virtual string getName() const = 0;
};

//--------------------------------------------------------------------------------

class AffectorFactory {
public:
	AffectorFactory() = default;

	virtual shared_ptr< AffectorCreator > createCreator() = 0;

	virtual string getName() const = 0;
};

//================================================================================

// PHYSICS

//================================================================================

class RotateToVelocityAffector : public Affector {
public:
	RotateToVelocityAffector( float offset ) : m_offset( offset ) {}

	void affect( Particle* target, sf::Time delta );

private:
	float m_offset{ 0.f };
};

class RotateToVelocityAffectorCreator : public AffectorCreator {
public:
	RotateToVelocityAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Rotate To Velocity"; }

private:
	Math::ValueSet< float > m_offset{ 0.f };
};

//--------------------------------------------------------------------------------

class RotateToVelocityAffectorFactory : public AffectorFactory {
public:
	RotateToVelocityAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< RotateToVelocityAffectorCreator >();
	}

	string getName() const override { return "Rotate To Velocity"; }
};

//================================================================================

// SIN

//================================================================================

class SinPositionAffector : public Affector {
public:
	SinPositionAffector( milliseconds period, Math::Vec2 target, float offset, float exponent ) :
		m_period( period ), m_target( target ), m_offset( offset ),
		m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	microseconds m_period;
	Math::Vec2 m_target;
	float m_offset;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class SinPositionAffectorCreator : public AffectorCreator {
public:
	SinPositionAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Sine Position"; }

private:
	Math::ValueSet< int > m_period{ 1000 };
	Math::Position m_target{};
	Math::ValueSet< float > m_offset{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class SinPositionAffectorFactory : public AffectorFactory {
public:
	SinPositionAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< SinPositionAffectorCreator >();
	}

	string getName() const override { return "Sine Position"; }
};

//================================================================================

class SinVelocityAffector : public Affector {
public:
	SinVelocityAffector( milliseconds period, float min, float max, float offset, float exponent ) :
		m_period( period ), m_min( min ), m_max( max ), m_offset( offset ),
		m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	microseconds m_period;
	float m_min;
	float m_max;
	float m_offset;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class SinVelocityAffectorCreator : public AffectorCreator {
public:
	SinVelocityAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Sine Velocity"; }

private:
	Math::ValueSet< int > m_period{ 1000 };
	Math::ValueSet< float > m_min{ 0.f };
	Math::ValueSet< float > m_max{ 1.f };
	Math::ValueSet< float > m_offset{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class SinVelocityAffectorFactory : public AffectorFactory {
public:
	SinVelocityAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< SinVelocityAffectorCreator >();
	}

	string getName() const override { return "Sine Velocity"; }
};

//================================================================================

class SinAlphaAffector : public Affector {
public:
	SinAlphaAffector( milliseconds period, float target, float offset, float exponent ) :
		m_period( period ), m_target( target ), m_offset( offset ),
		m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	microseconds m_period;
	float m_target;
	float m_offset;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class SinAlphaAffectorCreator : public AffectorCreator {
public:
	SinAlphaAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Sine Alpha"; }

private:
	Math::ValueSet< int > m_period{ 1000 };
	Math::ValueSet< float > m_target{ 0.f };
	Math::ValueSet< float > m_offset{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class SinAlphaAffectorFactory : public AffectorFactory {
public:
	SinAlphaAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< SinAlphaAffectorCreator >();
	}

	string getName() const override { return "Sine Alpha"; }
};

//================================================================================

class SinColorAffector : public Affector {
public:
	SinColorAffector( milliseconds period, Math::Color target, float offset, float exponent ) :
		m_period( period ), m_target( target ), m_offset( offset ),
		m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	microseconds m_period;
	Math::Color m_target;
	float m_offset;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class SinColorAffectorCreator : public AffectorCreator {
public:
	SinColorAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Sine Color"; }

private:
	Math::ValueSet< int > m_period{ 1000 };
	Math::ValueSet< Math::Color > m_target{ Math::Color() };
	Math::ValueSet< float > m_offset{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class SinColorAffectorFactory : public AffectorFactory {
public:
	SinColorAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< SinColorAffectorCreator >();
	}

	string getName() const override { return "Sine Color"; }
};

//================================================================================

class SinSpinAffector : public Affector {
public:
	SinSpinAffector( milliseconds period, float min, float max, float offset, float exponent ) :
		m_period( period ), m_min( min ), m_max( max ), m_offset( offset ),
		m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	microseconds m_period;
	float m_min;
	float m_max;
	float m_offset;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class SinSpinAffectorCreator : public AffectorCreator {
public:
	SinSpinAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Sine Spin"; }

private:
	Math::ValueSet< int > m_period{ 100 };
	Math::ValueSet< float > m_min{ 1.f };
	Math::ValueSet< float > m_max{ 0.f };
	Math::ValueSet< float > m_offset{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class SinSpinAffectorFactory : public AffectorFactory {
public:
	SinSpinAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< SinSpinAffectorCreator >();
	}

	string getName() const override { return "Sine Spin"; }
};

//================================================================================

// FADES

//================================================================================

class FadeVelocityAffector : public Affector {
public:
	FadeVelocityAffector( float start, float end, float exponent ) :
		m_start( start ), m_end( end ), m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	float m_start;
	float m_end;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class FadeVelocityAffectorCreator : public AffectorCreator {
public:
	FadeVelocityAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Fade Velocity"; }

private:
	Math::ValueSet< float > m_start{ 1.f };
	Math::ValueSet< float > m_end{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class FadeVelocityAffectorFactory : public AffectorFactory {
public:
	FadeVelocityAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< FadeVelocityAffectorCreator >();
	}

	string getName() const override { return "Fade Velocity"; }
};

//================================================================================

class FadeScaleAffector : public Affector {
public:
	FadeScaleAffector( float start, float end, float exponent ) :
		m_start( start ), m_end( end ), m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	float m_start;
	float m_end;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class FadeScaleAffectorCreator : public AffectorCreator {
public:
	FadeScaleAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Fade Scale"; }

private:
	Math::ValueSet< float > m_start{ 1.f };
	Math::ValueSet< float > m_end{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class FadeScaleAffectorFactory : public AffectorFactory {
public:
	FadeScaleAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< FadeScaleAffectorCreator >();
	}

	string getName() const override { return "Fade Scale"; }
};

//================================================================================

class FadeAlphaAffector : public Affector {
public:
	FadeAlphaAffector( float target, float exponent ) :
		m_target( target ), m_exponent( exponent ) {}
	void affect( Particle* target, sf::Time delta ) override;

private:
	float m_target;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class FadeAlphaAffectorCreator : public AffectorCreator {
public:
	FadeAlphaAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Fade Alpha"; }

private:
	Math::ValueSet< float > m_target{ 0.f };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class FadeAlphaAffectorFactory : public AffectorFactory {
public:
	FadeAlphaAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< FadeAlphaAffectorCreator >();
	}

	string getName() const override { return "Fade Alpha"; }
};

//================================================================================

class FadeColorAffector : public Affector {
public:
	FadeColorAffector( Math::Color target, float exponent ) :
		m_target( target ), m_exponent( exponent ) {}

	void affect( Particle* target, sf::Time delta ) override;

private:
	Math::Color m_target;
	float m_exponent;
};

//--------------------------------------------------------------------------------

class FadeColorAffectorCreator : public AffectorCreator {
public:
	FadeColorAffectorCreator() = default;

	bool render() override;
	shared_ptr< Affector > get() override;
	rapidjson::Value getValue() override;
	void setValue( const rapidjson::Value& value ) override;

	string getName() const override { return "Fade Color"; }

private:
	Math::ValueSet< Math::Color > m_target{ Math::Color() };
	Math::ValueSet< float > m_exponent{ 1.f };
};

//--------------------------------------------------------------------------------

class FadeColorAffectorFactory : public AffectorFactory {
public:
	FadeColorAffectorFactory() = default;

	shared_ptr< AffectorCreator > createCreator() override {
		return make_unique< FadeColorAffectorCreator >();
	}

	string getName() const override { return "Fade Color"; }
};

//================================================================================

// Emission

//================================================================================

class OnDeathEmitterAffector : public Affector {
public:
	OnDeathEmitterAffector( EmitterType type, vector< ParticlePattern* > patterns ) :
		Affector(), m_patterns( patterns ), m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	vector< ParticlePattern* > m_patterns;
	EmitterType m_type;
};

//================================================================================

class EmitterAffector : public Affector {
public:
	EmitterAffector( EmitterType type,
					 milliseconds delay,
					 milliseconds duration,
					 Math::ValueSet< float > spawnRate,
					 vector< ParticlePattern* > patterns ) :
		Affector(),
		m_type( type ), m_duration( duration ), m_elapsed( 0ms ),
		m_next( delay ), m_spawnRate( spawnRate ), m_patterns( patterns ) {}

	void affect( Particle* target, sf::Time delta );

private:
	EmitterType m_type;
	microseconds m_duration;
	microseconds m_elapsed;
	microseconds m_next;
	Math::ValueSet< float > m_spawnRate;
	vector< ParticlePattern* > m_patterns;

	bool m_finished{ false };
	size_t m_index{ 0 };
};

//================================================================================

// Inheritance

//================================================================================

class InheritLifetimeAffector : public Affector {
public:
	InheritLifetimeAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritPositionAffector : public Affector {
public:
	InheritPositionAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritRotationAffector : public Affector {
public:
	InheritRotationAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritSpinAffector : public Affector {
public:
	InheritSpinAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritScaleAffector : public Affector {
public:
	InheritScaleAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritColorAffector : public Affector {
public:
	InheritColorAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritAlphaAffector : public Affector {
public:
	InheritAlphaAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

class InheritVelocityAffector : public Affector {
public:
	InheritVelocityAffector( InheritanceType type, bool scale, bool rotate ) :
		m_type( type ), m_scale( scale ), m_rotate( rotate ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
	bool m_scale;
	bool m_rotate;
};

//--------------------------------------------------------------------------------

class InheritAccelerationAffector : public Affector {
public:
	InheritAccelerationAffector( InheritanceType type ) : m_type( type ) {}

	void affect( Particle* target, sf::Time delta );

private:
	InheritanceType m_type;
};

//--------------------------------------------------------------------------------

}	 // namespace Affector
}	 // namespace Gfx::Particle

//================================================================================
