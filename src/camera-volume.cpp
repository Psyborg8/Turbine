//================================================================================

#include "camera-volume.h"

//--------------------------------------------------------------------------------

#include "player.h"
#include "world.h"

//================================================================================

namespace Game {

//--------------------------------------------------------------------------------

void CameraVolume::onProcessCollisions() {
	World* world = getWorld();
	if( world == nullptr )
		return;

	shared_ptr< Player > player = world->getPlayer();

	// TODO - Move to Camera
	/*
	Collision::CollisionResult result = isColliding( player );
	if( result.success ) {
		world->setCameraTarget( m_position );
		world->setCameraDistanceTarget( m_distance );
	}
	*/
}

//--------------------------------------------------------------------------------

}

//================================================================================