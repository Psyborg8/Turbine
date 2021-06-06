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
	Colliders,
	Camera,
	Events,
};

//--------------------------------------------------------------------------------

enum class ObjectType {
	None = 0,
	Checkpoint,
	Wall,
	Trap,
	Camera,
	Event,
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

struct Tileset {
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
	{ "Colliders", LayerType::Colliders },
	{ "Camera", LayerType::Camera },
	{ "Events", LayerType::Events },
};

//--------------------------------------------------------------------------------

const map< string, ObjectType > objectMap{
	{ "Checkpoint", ObjectType::Checkpoint },
	{ "Wall", ObjectType::Wall },
	{ "Trap", ObjectType::Trap },
	{ "Camera", ObjectType::Camera },
	{ "Event", ObjectType::Event },
};

//================================================================================

// Loading
bool loadLayer( rapidjson::Value& data, TileLayer& layer );
bool loadLayer( rapidjson::Value& data, ObjectLayer& layer );
bool loadTileset( rapidjson::Value& data, Tileset& tileset );

//--------------------------------------------------------------------------------

// Construction
void constructObject( const Rect& object, Object* parent );

//================================================================================

vector< Map >::iterator getMap( string name );

//--------------------------------------------------------------------------------

vector< Map > maps;

//================================================================================

// LOADING

//================================================================================

void loadMap( string name ) {
	Debug::addMessage( "Loading Map: " + name, DebugType::Info );
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

	if( document.HasParseError() ) {
		Debug::addMessage( "Map \"" + name + "\" has JSON parse error", DebugType::Error );
		return;
	}

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

	std::sort( map.tilesets.begin(), map.tilesets.end(),
			   []( const Tileset& a, const Tileset& b ) {
				   return a.uid > b.uid;
			   } );

	maps.push_back( map );

	Debug::stopTimer( "Map::Load" );

	Debug::addMessage( "Map \"" + name + "\" loaded successfully.", DebugType::Info );
}

//--------------------------------------------------------------------------------

void unloadMap( string name ) {
	Debug::addMessage( "Map \"" + name + "\" unloaded.", DebugType::Info );
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

	const string name = data[ "name" ].GetString();
	layer.type = layerMap.at( name );

	// Load objects
	GenericArray< false, Value > objects = data[ "objects" ].GetArray();
	for( SizeType i = 0u; i < objects.Size(); ++i ) {

		// Get object data
		Rect rect;
		rect.type = objectMap.at( objects[ i ][ "type" ].GetString() );
		rect.position.x = objects[ i ][ "x" ].GetFloat();
		rect.position.y = objects[ i ][ "y" ].GetFloat();
		rect.size.x = objects[ i ][ "width" ].GetFloat();
		rect.size.y = objects[ i ][ "height" ].GetFloat();
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

	return true;
}

//================================================================================

// CONSTRUCTION

//================================================================================

void constructMap( string name, Object* world ) {
	Debug::addMessage( "Constructing Map \"" + name + "\"", DebugType::Info );
	Debug::startTimer( "Map::Construct" );

	// Constructing map without parent will leak memory
	if( world == nullptr )
		return Debug::addMessage( "Map \"" + name + "\" attempting construction without parent.", DebugType::Error );

	const auto it = getMap( name );
	if( it == maps.end() )
		return Debug::addMessage( "Map \"" + name + "\" not found for construction.", DebugType::Error );

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
												  return id >= tileset.uid;
											  } );

				if( it == tilesets.end() )
					continue;

				const Tileset& tileset = *it;

				// Calculate position
				Math::Vec2 position;
				position.x = float( idx % 16u ) * 16.0f;
				position.y = float( idx / 16u ) * 16.0f;
				Math::Vec2 worldPosition = position + layer.position + chunk.position;

				// Render to texture
				Gfx::Tileset::renderTile( tileset.name, id - tileset.uid, &texture, position );
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
	Debug::addMessage( "Map \"" + name + "\" constructed successfully.", DebugType::Info );
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
			shared_ptr< Game::RigidRect > levelEnd = Object::makeObject< Game::RigidRect >( parent );
			levelEnd->setPosition( object.position );
			levelEnd->setSize( object.size );
			levelEnd->setName( "Level End" );
			levelEnd->setCollisionType( CollisionType::Static );
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
			return;
		}

		return;
	}

	if( object.type == ObjectType::Wall ) {
		shared_ptr< Game::RigidRect > wall = Object::makeObject< Game::RigidRect >( parent );
		wall->setName( "Wall" );
		wall->setPosition( object.position );
		wall->setSize( object.size );
		wall->setCollisionType( CollisionType::Static );
		return;
	}

	if( object.type == ObjectType::Trap ) {
		shared_ptr< Game::RigidRect > trap = Object::makeObject< Game::RigidRect >( parent );
		trap->setName( "Trap" );
		trap->setPosition( object.position );
		trap->setSize( object.size );
		trap->setCollisionType( CollisionType::Static );
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
		rect.setFillColor( sf::Color( 0u, 0u, 0u, 64u ) );
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