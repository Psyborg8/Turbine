//================================================================================

#pragma once

//================================================================================

#include "Object.h"
#include "Camera.h"
#include "World.h"

//--------------------------------------------------------------------------------

namespace Gfx {
namespace Map {

//--------------------------------------------------------------------------------

void renderMap( string name );
void renderMap( string name, MapLayer layer, Camera* camera = nullptr );

void loadMap( string name );

void unloadMap( string name );

void constructMap( string name, Object* parent );

vector< shared_ptr< Object > > getObjects( string mapName, string objectName = "" );

//--------------------------------------------------------------------------------

class RenderLayer : public Object {
public:
	RenderLayer() = default;

	// Events
public:
	void onRender() override { 
		Worlds::World* world = getWorld();

		if( world == nullptr )
			renderMap( m_mapName, m_layer );
		else
			renderMap( m_mapName, m_layer, &world->getCamera() );
	}

public:
	void set( string mapName, MapLayer layer ) { m_mapName = mapName; m_layer = layer; }

private:
	MapLayer m_layer;
	string m_mapName;
};

//--------------------------------------------------------------------------------

} // Map
} // Gfx

//================================================================================