//================================================================================

#include "Player.h"

//--------------------------------------------------------------------------------

#include "MathTypes.h"
#include "System.h"
#include "World.h"
#include "Timer.h"
#include "Line.h"

//================================================================================

namespace Game {

//================================================================================

Player::Player() : Player( Math::Vec2() ) {
	//
}

//--------------------------------------------------------------------------------

Player::Player( Math::Vec2 pos ) : RigidRect( pos, Math::Vec2( 0.15, 0.2 ), Colors::BLUE ) {
	setName( "Player" );
}

//--------------------------------------------------------------------------------

void Player::onUpdate( double deltaTime ) {
	// Ignore physics while dashing
	if( !dashData.isDashing ) {
		// Horizontal
		{
			double move = 0.0;
			// Movement
			if( abs( velocity.x ) <= movementData.maxSpeed ) {
				// Input
				if( System::getKeyState( KeyCode::d ) )
					move += movementData.acceleration * deltaTime;
				if( System::getKeyState( KeyCode::a ) )
					move -= movementData.acceleration * deltaTime;

				// Air control
				if( !jumpData.canJump )
					move *= movementData.airMultiplier;

				// Apply movement
				if( abs( velocity.x + move ) > movementData.maxSpeed &&
					velocity.x / abs( velocity.x ) == move / abs( move ) )
					velocity.x = movementData.maxSpeed * ( move / abs( move ) );
				else
					velocity.x += move;
			}

			// Friction
			if( !move && velocity.x ) {
				double friction = frictionData.power * abs( velocity.x ) * deltaTime;

				// Air resistance
				if( !jumpData.canJump )
					friction *= frictionData.airMultiplier;

				friction = std::clamp( friction, frictionData.min * deltaTime, frictionData.max * deltaTime );
				if( abs( velocity.x ) < friction )
					velocity.x = 0.0;
				else
					velocity.x -= friction * ( velocity.x / abs( velocity.x ) );
			}
		}

		// Vertical
		{
			// Gravity
			velocity.y -= gravityData.power * deltaTime;
			velocity.y = velocity.y < -gravityData.max ? -gravityData.max : velocity.y;

			// Jump Release
			if( !System::getKeyState( KeyCode::Space ) )
				velocity.y = std::min( velocity.y, jumpData.release );

			// Wall Cling
			if( wallClingData.isClinging ) {
				double friction = wallClingData.multiplier * abs( velocity.y ) * deltaTime;
				friction = std::clamp( friction, wallClingData.min * deltaTime, wallClingData.max * deltaTime );
				if( abs( velocity.y ) < friction )
					velocity.y = 0.0;
				else
					velocity.y -= friction * ( velocity.y / abs( velocity.y ) );
			}
		}
	}

	// Reset flags
	wallClingData.isClinging = false;
	jumpData.canJump = false;
}

//--------------------------------------------------------------------------------

void Player::onProcessCollisions()
{
	vector< shared_ptr< Object > > targets;
	const vector< shared_ptr< Object > > walls = getObjects( System::getWorld(), "Wall" );
	const vector< shared_ptr< Object > > traps = getObjects( System::getWorld(), "Trap" );
	const vector< shared_ptr< Object > > platforms = getObjects( System::getWorld(), "Platform" );

	targets.insert( targets.end(), walls.begin(), walls.end() );
	targets.insert( targets.end(), traps.begin(), traps.end() );

	// Check if we're supposed to collide with platforms

	bool collision = false;
	for( shared_ptr< Object > platform : platforms ) {
		Collision::CollisionResult result = isColliding( platform );
		if( result.success ) {
			collision = true;
			break;
		}
	}

	jumpData.isJumpingDown &= collision;
	if( !jumpData.isJumpingDown )
		if( velocity.y < 0.0 )
			targets.insert( targets.end(), platforms.begin(), platforms.end() );
		else if( collision )
			jumpData.isJumpingDown = true;

	resolveCollisions( targets, true );
}

//--------------------------------------------------------------------------------

void Player::onRender() {
	RigidRect::render();
}

//--------------------------------------------------------------------------------

void Player::onDestroy() {
	Observers::removeObserver( m_keyPressObserver );
	Observers::removeObserver( m_keyReleaseObserver );
}

//--------------------------------------------------------------------------------

void Player::onCreateObservers() {
	using namespace std::placeholders;

	m_keyPressObserver = Observers::addObserver( Observers::ObserverType::KeyPress, KeyCallback( std::bind( &Player::onKeyboardPress, this, _1 ) ) );
	m_keyReleaseObserver = Observers::addObserver( Observers::ObserverType::KeyRelease, KeyCallback( std::bind( &Player::onKeyboardRelease, this, _1 ) ) );
}

//--------------------------------------------------------------------------------

void Player::onKeyboardPress( int key ) {
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::Space )
		jump();
	else if( keyCode == KeyCode::e )
		dash();
}

//--------------------------------------------------------------------------------

void Player::onKeyboardRelease( int key ) {
	const KeyCode keyCode = static_cast< KeyCode >( key );
	if( keyCode == KeyCode::Space )
		jumpData.wait = false;
	else if( keyCode == KeyCode::e )
		dashData.wait = false;
}

//--------------------------------------------------------------------------------

void Player::onCollision( Collision::CollisionResult result, shared_ptr< Object > target ) {
	// Check for Trap or Wall
	if( target->getName() == "Trap" )
		return kill();
	if( target->getName() == "Platform" )
		jumpData.canJumpDown = true;

	if( result.normal.y == 1.0 ) {
		jumpData.canJump = true;
		doubleJumpData.canDoubleJump = true;
	}

	if( result.normal.x ) {
		wallClingData.isClinging = true;
		wallJumpData.normal = result.normal.x;
	}

	Timers::triggerTimer( "Dash Timer" );
}

//--------------------------------------------------------------------------------

void Player::jump()
{
	if( jumpData.wait )
		return;

	if( jumpData.canJump ) {
		if( jumpData.canJumpDown && System::getKeyState( KeyCode::s ) )
			jumpData.isJumpingDown = true;
		else {
			velocity.y = jumpData.power;
			jumpData.canJump = false;
		}
	}
	else if( doubleJumpData.canDoubleJump ) {
		if( wallClingData.isClinging ) {
			velocity = wallJumpData.direction * wallJumpData.power;
			velocity.x *= wallJumpData.normal;
		}
		else
			velocity.y = doubleJumpData.power;

		doubleJumpData.canDoubleJump = false;
	}

	jumpData.wait = true;
}

//--------------------------------------------------------------------------------

void Player::dash() {
	if( dashData.wait )
		return;

	dashData.wait = true;

	if( !dashData.canDash )
		return;

	Math::Vec2 direction;
	if( System::getKeyState( KeyCode::d ) )
		direction.x += 1.0;
	if( System::getKeyState( KeyCode::a ) )
		direction.x -= 1.0;
	if( System::getKeyState( KeyCode::w ) )
		direction.y += 1.0;
	if( System::getKeyState( KeyCode::s ) )
		direction.y -= 1.0;

	if( direction == Math::Vec2() )
		return;

	direction = direction.normalize();
	direction *= dashData.power;

	velocity = direction;

	dashData.canDash = false;
	dashData.isDashing = true;
	dashData.wait = true;
	doubleJumpData.canDoubleJump = true;

	Timers::addTimer( "Dash Cooldown", dashData.cooldown, nullptr, [this] { dashData.canDash = true; }, false );
	Timers::addTimer( "Dash Timer",
					 dashData.duration, nullptr,
					 [this] {
						 dashData.isDashing = false;
						 velocity.y = velocity.y > jumpData.release ? jumpData.release : velocity.y;
					 }, false );
}

//--------------------------------------------------------------------------------

void Player::kill() {
	System::getWorld()->reset();
}

//================================================================================

} // Game

//================================================================================
