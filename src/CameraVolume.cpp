//================================================================================

#include "CameraVolume.h"

//--------------------------------------------------------------------------------

#include "Player.h"
#include "SFMLWorld.h"

//================================================================================

namespace Game {

//--------------------------------------------------------------------------------

void CameraVolume::onProcessCollisions() {
	Worlds::World* world = getWorld();
	if( world == nullptr )
		return;

	shared_ptr< Player > player = world->getPlayer();

	Collision::CollisionResult result = isColliding( player );
	if( result.success ) {
		Worlds::SFMLWorld* world = dynamic_cast< Worlds::SFMLWorld* >( m_parent );
		if( world == nullptr )
			return;

		world->setCameraTarget( m_position );
		world->setCameraDistanceTarget( m_distance );
	}
}

//--------------------------------------------------------------------------------

}

//================================================================================