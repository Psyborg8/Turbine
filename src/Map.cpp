//================================================================================

#include "Map.h"

//--------------------------------------------------------------------------------

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "MathTypes.h"
#include "Tileset.h"
#include "RigidRect.h"
#include "Player.h"
#include "System.h"

#include "Debug.h"

//================================================================================

namespace Gfx {
namespace Map {

//================================================================================

enum class LayerType {
	Background = 0,
	BackgroundDetails,
	Terrain,
	Checkpoints,
};

//--------------------------------------------------------------------------------

enum class TileType {
	None = 0,
	Wall,
	Trap,
	DevSymbol,
};

//--------------------------------------------------------------------------------

enum class ObjectType {
	None = 0,
	Checkpoint,
};

//--------------------------------------------------------------------------------

enum class DevSymbol {
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
	A, B, C, D, E, S, Cross, Tick, Warning, Dash
};

//================================================================================

struct Rect {
	string name;
	Math::Vec2 position;
	Math::Vec2 size;
	ObjectType type;
};

//--------------------------------------------------------------------------------

struct Point {
	string name;
	Math::Vec2 position;
	ObjectType type;
};

//--------------------------------------------------------------------------------

struct Tile {
	int id;
	TileType type;
	Math::Vec2 offset;
	Math::Vec2 size;
	Math::Vec2 direction;
};

//--------------------------------------------------------------------------------

struct Tileset {
	vector< Tile > tiles;
	string name;
	string path;
	int uid;
	Math::Vec2 tileSize;
};

//--------------------------------------------------------------------------------

struct Chunk {
	array< int, 16u * 16u > blockData;
	Math::Vec2 position; 
	sf::Texture texture;
};

//--------------------------------------------------------------------------------

struct TileLayer {
	vector< Chunk > chunks;
	Math::Vec2 position;
	LayerType type;
};

//--------------------------------------------------------------------------------

struct ObjectLayer {
	vector< Rect > objects;
	LayerType type;
};

//--------------------------------------------------------------------------------

struct Map {
	map< LayerType, TileLayer > tileLayers;
	map< LayerType, ObjectLayer > objectLayers;
	vector< Tileset > tilesets;
	string name;
	Math::Vec2 tileSize;
	Math::Vec2 mapSize;
};

//================================================================================

const map< string, LayerType > layerMap{
	{ "Background", LayerType::Background },
	{ "Background Details", LayerType::BackgroundDetails },
	{ "Terrain", LayerType::Terrain },
	{ "Checkpoints", LayerType::Checkpoints },
};

//--------------------------------------------------------------------------------

const map< string, TileType > tileMap{
	{ "Wall", TileType::Wall },
	{ "Trap", TileType::Trap },
	{ "DevSymbol", TileType::DevSymbol },
};

//--------------------------------------------------------------------------------

const map< string, ObjectType > objectMap{
	{ "Checkpoint", ObjectType::Checkpoint },
};

//================================================================================

// Loading
bool loadLayer( rapidjson::Value& data, TileLayer& layer );
bool loadLayer( rapidjson::Value& data, ObjectLayer& layer );
bool loadTileset( rapidjson::Value& data, Tileset& tileset );
bool loadTile( rapidjson::Value& data, Tile& tile );

//--------------------------------------------------------------------------------

// Construction
void constructCollider( const Tile& tile, Math::Vec2 position, Math::Vec2 size, Object* parent );
void constructObject( const Rect& object, Object* parent );

//================================================================================

vector< Map >::iterator getMap( string name );

//--------------------------------------------------------------------------------

vector< Map > maps;

//================================================================================

// LOADING

//================================================================================

void loadMap( string name ) {
	Debug::startTimer( "Map::Load" );

	using namespace rapidjson;

	const auto it = getMap( name );
	if( it != maps.end() )
		return;

	Map map;
	map.name = name;

	// Parse JSON
	string path = Folders::Maps + name + ".json";
	FILE* pFile;
	fopen_s( &pFile, path.c_str(), "rb" );
	char buffer[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>( is );
	fclose( pFile );

	if( document.HasParseError() )
		return;

	// Meta Data
	{
		map.tileSize.x = float( document[ "tilewidth" ].GetInt() );
		map.tileSize.y = float( document[ "tileheight" ].GetInt() );
		map.mapSize.x = float( document[ "width" ].GetInt() );
		map.mapSize.y = float( document[ "height" ].GetInt() );
	}

	// Layers
	GenericArray< false, Value > layers = document[ "layers" ].GetArray();
	for( SizeType i = 0; i < layers.Size(); ++i ) {
		// Tile Layer
		if( layers[ i ][ "type" ] == "tilelayer" ) {
			TileLayer layer;
			if( loadLayer( layers[ i ], layer ) )
				map.tileLayers[ layer.type ] = layer;
		}

		// Object Layer
		else if( layers[ i ][ "type" ] == "objectgroup" ) {
			ObjectLayer layer;
			if( loadLayer( layers[ i ], layer ) )
				map.objectLayers[ layer.type ] = layer;
		}
	}

	// Tilesets
	GenericArray< false, Value > tilesets = document[ "tilesets" ].GetArray();
	for( SizeType i = 0; i < tilesets.Size(); ++i ) {
		Tileset tileset;

		if( loadTileset( tilesets[ i ], tileset ) )
			map.tilesets.push_back( tileset );
	}

	maps.push_back( map );

	Debug::stopTimer( "Map::Load" );
}

//--------------------------------------------------------------------------------

void unloadMap( string name ) {
	Debug::startTimer( "Map::Unload" );

	const auto it = getMap( name );
	if( it != maps.end() )
		maps.erase( it );

	Debug::stopTimer( "Map::Unload" );
}

//--------------------------------------------------------------------------------

bool loadLayer( rapidjson::Value& data, TileLayer& layer ) {
	using namespace rapidjson;

	if( !data[ "name" ].IsString() )
		return false;

	const string name = data[ "name" ].GetString();
	layer.type = layerMap.at( name );

	// Chunks
	GenericArray< false, Value > chunks = data[ "chunks" ].GetArray();
	for( SizeType j = 0; j < chunks.Size(); ++j ) {
		Chunk chunk;

		// Chunk position
		Math::Vec2 position;
		if( !chunks[ j ][ "x" ].IsInt() | !chunks[ j ][ "y" ].IsInt() )
			continue;

		chunk.position.x = float( chunks[ j ][ "x" ].GetInt() ) * 16.0f;
		chunk.position.y = float( chunks[ j ][ "y" ].GetInt() ) * 16.0f;

		// Block data
		if( !chunks[ j ][ "data" ].IsArray() )
			continue;

		GenericArray< false, Value > data = chunks[ j ][ "data" ].GetArray();
		for( SizeType k = 0; k < data.Size(); ++k )
			chunk.blockData[ k ] = data[ k ].GetInt();

		layer.chunks.push_back( chunk );
	}

	return true;
}

//--------------------------------------------------------------------------------

bool loadLayer( rapidjson::Value& data, ObjectLayer& layer ) {
	using namespace rapidjson;

	if( !data[ "name" ].IsString()
		|| !data[ "objects" ].IsArray() )
		return false;

	const string name = data[ "name" ].GetString();
	layer.type = layerMap.at( name );

	// Load objects
	GenericArray< false, Value > objects = data[ "objects" ].GetArray();
	for( SizeType i = 0u; i < objects.Size(); ++i ) {

		// Get object data
		Rect rect;
		rect.type = objectMap.at( objects[ i ][ "type" ].GetString() );
		rect.position.x = float( objects[ i ][ "x" ].GetInt() );
		rect.position.y = float( objects[ i ][ "y" ].GetInt() );
		rect.size.x = float( objects[ i ][ "width" ].GetInt() );
		rect.size.y = float( objects[ i ][ "height" ].GetInt() );
		rect.name = objects[ i ][ "name" ].GetString();

		layer.objects.push_back( rect );
	}

	return true;
}

//--------------------------------------------------------------------------------

bool loadTileset( rapidjson::Value& data, Tileset& tileset ) {
	using namespace rapidjson;

	// Get meta data
	tileset.uid = data[ "firstgid" ].GetInt();
	tileset.path = Folders::Maps + data[ "image" ].GetString();
	tileset.name = data[ "name" ].GetString();
	tileset.tileSize.y = float( data[ "tileheight" ].GetInt() );
	tileset.tileSize.x = float( data[ "tilewidth" ].GetInt() );

	// Load tiles
	const GenericArray< false, Value > tiles = data[ "tiles" ].GetArray();
	for( SizeType j = 0; j < tiles.Size(); ++j ) {
		Tile tile;
		if( loadTile( tiles[ j ], tile ) )
			tileset.tiles.push_back( tile );
	}

	return true;
}

//--------------------------------------------------------------------------------

bool loadTile( rapidjson::Value& data, Tile& tile ) {
	using namespace rapidjson;

	tile.id = data[ "id" ].GetInt();
	tile.type = tileMap.at( data[ "type" ].GetString() );

	// Traps
	if( tile.type == TileType::Trap ) {
		const GenericArray< false, Value > properties = data[ "properties" ].GetArray();
		for( const Value& value : properties ) {

			string name = value[ "name" ].GetString();
			if( name == "Normal x" ) {
				const int directionX = value[ "value" ].GetInt();
				tile.direction.x = float( directionX );
			}
			if( name == "Normal y" ) {
				const int directionY = value[ "value" ].GetInt();
				tile.direction.y = float( directionY );
			}
		}
	}

	return true;
}

//================================================================================

// CONSTRUCTION

//================================================================================

void constructMap( string name, Object* world ) {
	Debug::startTimer( "Map::Construct" );

	// Constructing map without parent will leak memory
	if( world == nullptr )
		return;

	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	const vector< Tileset >& tilesets = it->tilesets;
	const Math::Vec2 mapTileSize = it->tileSize;

	// Load Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::loadTileset( tileset.name, tileset.tileSize, tileset.path );

	// Process Tile Layers
	for( auto& pair : it->tileLayers ) {
		TileLayer& layer = pair.second;

		// Process chunks
		for( Chunk& chunk : layer.chunks ) {
			// Pre-render chunk to a texture to save on render calls
			sf::RenderTexture texture;
			texture.create( 256u, 256u );
			texture.clear( sf::Color( 0u, 0u, 0u, 0u ) );

			if( layer.type == LayerType::Background )
				texture.setSmooth( true );

			// Construct objects and render tiles
			for( size_t idx = 0u; idx < chunk.blockData.size(); ++idx ) {
				const int id = chunk.blockData.at( idx );

				if( !id )
					continue;

				// Find the tileset
				const auto it = std::find_if( tilesets.begin(), tilesets.end(),
											  [id]( const Tileset& tileset ) {
												  return id >= tileset.uid && id < tileset.uid + tileset.tiles.size();
											  } );

				if( it == tilesets.end() )
					continue;

				const Tileset& tileset = *it;

				// Calculate position
				Math::Vec2 position;
				position.x = float( idx % 16u ) * 16.0f;
				position.y = float( idx / 16u ) * 16.0f;
				Math::Vec2 worldPosition = position + layer.position + chunk.position;

				// Process tile attributes
				const Tile& tile = tileset.tiles[ id - tileset.uid ];

				// Render to texture
				Gfx::Tileset::renderTile( tileset.name, tile.id, &texture, position );

				// Construct collider
				if( layer.type == LayerType::Terrain )
					constructCollider( tile, worldPosition, Math::Vec2( it->tileSize ), world );
			}

			texture.display();
			chunk.texture = sf::Texture( texture.getTexture() );
		}
	}

	// Process Object Layers
	for( const auto& pair : it->objectLayers ) {
		const ObjectLayer& layer = pair.second;

		for( const Rect& object : layer.objects )
			constructObject( object, world );
	}

	// Unload Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::unloadTileset( tileset.name );

	Debug::stopTimer( "Map::Construct" );
}

//--------------------------------------------------------------------------------

void constructCollider( const Tile& tile, Math::Vec2 position, Math::Vec2 size, Object* parent ) {
	
	// Wall
	if( tile.type == TileType::Wall ) {
		shared_ptr< Game::RigidRect > rect =
			Object::makeObject< Game::RigidRect >( parent );
		rect->setSize( size );
		rect->setPosition( position );
		rect->setName( "Wall" );
		rect->setCollisionType( CollisionType::Static );
		return;
	}

	// Trap
	if( tile.type == TileType::Trap ) {
		// Collider depends on the direction the trap is facing
		Math::Vec2 normal = tile.direction;

		// Construct X and Y colliders separately so we can have corners
		Math::Vec2 pos;
		Math::Vec2 size;
		if( normal.x ) {
			shared_ptr< Game::RigidRect > rect =
				Object::makeObject< Game::RigidRect >( parent );

			rect->setName( "Trap" );
			rect->setCollisionType( CollisionType::Static );
			rect->setSize( sf::Vector2f( 4.0f, 16.0f ) );

			Math::Vec2 pos = position;
			pos.x += normal.x > 0 ? 0.0f : 12.0f;
			rect->setPosition( pos );
		}
		if( normal.y ) {
			shared_ptr< Game::RigidRect > rect =
				Object::makeObject< Game::RigidRect >( parent );

			rect->setName( "Trap" );
			rect->setCollisionType( CollisionType::Static );
			rect->setSize( sf::Vector2f( 16.0f, 4.0f ) );

			Math::Vec2 pos = position;
			pos.y += normal.y > 0 ? 0.0f : 12.0f;
			rect->setPosition( pos );

		}
	}
}

//--------------------------------------------------------------------------------

void constructObject( const Rect& object, Object* parent ) {
	if( object.type == ObjectType::Checkpoint ) {
		// Player Start
		if( object.name == "S" ) {
			Math::Vec2 position = object.position;
			position.x += object.size.x / 2.0f;

			shared_ptr< Game::Player > player = Object::makeObject< Game::Player >( parent );
			player->setPosition( position );
			player->setSpawn( position );

			return;
		}

		// Level End
		if( object.name == "E" ) {

			return;
		}
		
		// Checkpoint
		const int id = std::stoi( object.name );
		if( id > 0 ) {
			shared_ptr< Game::RigidRect > checkpoint = Object::makeObject< Game::RigidRect >( parent );
			checkpoint->setPosition( object.position );
			checkpoint->setSize( object.size );
			checkpoint->setName( "Checkpoint" );
			checkpoint->setCollisionType( CollisionType::Static );
		}
	}
}

//================================================================================

// RENDERING

//================================================================================

void renderMap( string name ) {
	Debug::startTimer( "Map::Render" );

	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	for( const Chunk& chunk : it->tileLayers[ LayerType::Background ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ LayerType::Background ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );

		// Darken the background
		sf::RectangleShape rect;
		rect.setPosition( position.sf() );
		rect.setSize( sf::Vector2f( 256.0f, 256.0f ) );
		rect.setFillColor( sf::Color( 0u, 0u, 0u, 48u ) );
		System::getWindow()->draw( rect );
	}

	for( const Chunk& chunk : it->tileLayers[ LayerType::BackgroundDetails ].chunks ) {

		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ LayerType::BackgroundDetails ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );
	}

	for( const Chunk& chunk : it->tileLayers[ LayerType::Terrain ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ LayerType::Terrain ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );
	}

	Debug::stopTimer( "Map::Render" );
}

//================================================================================

vector< Map >::iterator getMap( string name ) {
	const auto it = find_if( maps.begin(), maps.end(),
							 [name]( const Map& map ) {
								 return map.name == name;
							 } );

	return it;
}

//================================================================================

} // Map
} // Gfx

//================================================================================