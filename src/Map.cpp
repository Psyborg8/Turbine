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
#include "CameraVolume.h"

#include "Debug.h"

//================================================================================

namespace Gfx {
namespace Map {

//================================================================================

struct Property {
	string name;
	string type;

	bool boolValue{ false };
	float floatValue{ 0.0f };
	int intValue{ -1 };
	string stringValue{ "" };
	int objectValue{ -1 };
};

//--------------------------------------------------------------------------------

struct MapObject {
	vector< Property > properties;

	string name;
	string type;
	int id;
	Math::Vec2 position;
	Math::Vec2 size;
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
	string name;
};

//--------------------------------------------------------------------------------

struct Tile {
	int uid;
	int id;
	string type;
};

//--------------------------------------------------------------------------------

struct ObjectLayer {
	vector< MapObject > objects;
	string name;
};

//--------------------------------------------------------------------------------

struct Map {
	unordered_map< string, TileLayer > tileLayers;
	unordered_map< string, ObjectLayer > objectLayers;

	unordered_map< string, vector< shared_ptr< Object > > > objects;
	map< int, MapObject > objectIds;

	vector< Tileset > tilesets;
	map< int, Tile > tilemap;

	string name;
	Math::Vec2 tileSize;
	Math::Vec2 mapSize;
};

//================================================================================

// Loading
bool loadLayer( const rapidjson::Value& data, TileLayer& layer );
bool loadLayer( const rapidjson::Value& data, ObjectLayer& layer, Map& map );
bool loadTileset( const rapidjson::Value& data, Tileset& tileset, Map& map );

//--------------------------------------------------------------------------------

// Construction
void constructObject( const MapObject& object, Map& map, Object* world );
void constructCollider( const Tile& tile, Map& map, Math::Vec2 position, Object* world );

//================================================================================

vector< Map >::iterator getMap( string name );

bool getString( const rapidjson::Value& data, string name, string& out );
bool getBool( const rapidjson::Value& data, string name, bool& out );
bool getInt( const rapidjson::Value& data, string name, int& out );
bool getFloat( const rapidjson::Value& data, string name, float& out );

//--------------------------------------------------------------------------------

vector< Map > maps;

//================================================================================

// LOADING

//================================================================================

void loadMap( string name ) {
	Debug::addMessage( "Loading Map: " + name, Debug::DebugType::Info );
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
	if( fopen_s( &pFile, path.c_str(), "rb" ) )
		return;
	char buffer[ 65536 ];
	FileReadStream is( pFile, buffer, sizeof( buffer ) );
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>( is );
	fclose( pFile );

	if( document.HasParseError() ) {
		Debug::addMessage( "Map \"" + name + "\" has JSON parse error", Debug::DebugType::Error );
		return;
	}

	// Meta Data
	{
		getFloat( document, "tilewidth", map.tileSize.x );
		getFloat( document, "tileheight", map.tileSize.y );
		getFloat( document, "width", map.mapSize.x );
		getFloat( document, "height", map.mapSize.y );
	}

	// Layers
	if( document.HasMember( "layers" ) ) {
		const auto layers = document[ "layers" ].GetArray();
		for( SizeType i = 0; i < layers.Size(); ++i ) {
			string type;
			if( !getString( layers[ i ], "type", type ) )
				continue;

			// Tile Layer
			if( type == "tilelayer" ) {
				TileLayer layer;
				if( loadLayer( layers[ i ], layer ) )
					map.tileLayers[ layer.name ] = layer;
			}

			// Object Layer
			else if( type == "objectgroup" ) {
				ObjectLayer layer;
				if( loadLayer( layers[ i ], layer, map ) )
					map.objectLayers[ layer.name ] = layer;
			}
		}
	}

	// Tilesets
	if( document.HasMember( "tilesets" ) ) {
		const auto tilesets = document[ "tilesets" ].GetArray();
		for( SizeType i = 0; i < tilesets.Size(); ++i ) {
			Tileset tileset;

			if( loadTileset( tilesets[ i ], tileset, map ) )
				map.tilesets.push_back( tileset );

			std::sort( map.tilesets.begin(), map.tilesets.end(),
					   []( const Tileset& a, const Tileset& b ) {
						   return a.uid > b.uid;
					   } );
		}
	}

	maps.push_back( map );

	Debug::stopTimer( "Map::Load" );

	Debug::addMessage( "Map \"" + name + "\" loaded successfully.", Debug::DebugType::Info );
}

//--------------------------------------------------------------------------------

void unloadMap( string name ) {
	Debug::addMessage( "Map \"" + name + "\" unloaded.", Debug::DebugType::Info );
	Debug::startTimer( "Map::Unload" );

	const auto it = getMap( name );
	if( it != maps.end() )
		maps.erase( it );

	Debug::stopTimer( "Map::Unload" );
}

//--------------------------------------------------------------------------------

bool loadLayer( const rapidjson::Value& data, TileLayer& layer ) {
	using namespace rapidjson;

	if( !getString( data, "name", layer.name ) )
		return false;

	// Chunks
	if( data.HasMember( "chunks" ) ) {
		const auto chunks = data[ "chunks" ].GetArray();

		for( SizeType j = 0; j < chunks.Size(); ++j ) {
			Chunk chunk;

			// Chunk position
			getFloat( chunks[ j ], "x", chunk.position.x );
			getFloat( chunks[ j ], "y", chunk.position.y );

			chunk.position *= 16.0f;

			if( chunks[ j ].HasMember( "data" ) ) {
					const auto chunkData = chunks[ j ][ "data" ].GetArray();
					for( SizeType k = 0; k < chunkData.Size(); ++k )
						chunk.blockData[ k ] = chunkData[ k ].GetInt();

					layer.chunks.push_back( chunk );
			}
		}
	}

	return true;
}

//--------------------------------------------------------------------------------

bool loadLayer( const rapidjson::Value& data, ObjectLayer& layer, Map& map ) {
	using namespace rapidjson;

	if( !getString( data, "name", layer.name ) )
		return false;

	// Load objects
	if( !data.HasMember( "objects" ) )
		return false;

	const auto objects = data[ "objects" ].GetArray();

	for( SizeType i = 0u; i < objects.Size(); ++i ) {

		// Get object data
		MapObject object;

		getString( objects[ i ], "name", object.name );
		getString( objects[ i ], "type", object.type );
		getFloat( objects[ i ], "x", object.position.x );
		getFloat( objects[ i ], "y", object.position.y );
		getFloat( objects[ i ], "width", object.size.x );
		getFloat( objects[ i ], "height", object.size.y );
		getInt( objects[ i ], "id", object.id );

		if( objects[ i ].HasMember( "properties" ) && objects[ i ][ "properties" ].IsArray() ) {
			GenericArray< true, Value > properties = objects[ i ][ "properties" ].GetArray();

			for( const Value& property : properties ) {
				Property p;

				if( !getString( property, "name", p.name ) )
					continue;
				if( !getString( property, "type", p.type ) )
					continue;

				if( p.type == "bool" )
					getBool( property, "value", p.boolValue );
				if( p.type == "float" )
					getFloat( property, "value", p.floatValue );
				if( p.type == "int" )
					getInt( property, "value", p.intValue );
				if( p.type == "object" )
					getInt( property, "value", p.objectValue );
				if( p.type == "string" )
					getString( property, "value", p.stringValue );

				object.properties.push_back( p );
			}
		}

		layer.objects.push_back( object );

		if( object.id == -1 )
			return true;

		map.objectIds[ object.id ] = object;
	}

	return true;
}

//--------------------------------------------------------------------------------

bool loadTileset( const rapidjson::Value& data, Tileset& tileset, Map& map ) {
	using namespace rapidjson;

	// Get meta data
	getInt( data, "firstgid", tileset.uid );
	getString( data, "image", tileset.path );
	tileset.path = Folders::Maps + tileset.path;
	getString( data, "name", tileset.name );
	getFloat( data, "tilewidth", tileset.tileSize.x );
	getFloat( data, "tileheight", tileset.tileSize.y );

	if( !data.HasMember( "tiles" ) )
		return false;

	const auto tiles = data[ "tiles" ].GetArray();
	for( SizeType i = 0u; i < tiles.Size(); ++i ) {
		Tile tile;

		getInt( tiles[ i ], "id", tile.id );
		tile.uid = tile.id + tileset.uid;
		getString( tiles[ i ], "type", tile.type );

		map.tilemap[ tile.uid ] = tile;
	}

	return true;
}

//================================================================================

// CONSTRUCTION

//================================================================================

void constructMap( string name, Object* world ) {
	Debug::addMessage( "Constructing Map \"" + name + "\"", Debug::DebugType::Info );
	Debug::startTimer( "Map::Construct" );

	// Constructing map without parent will leak memory
	if( world == nullptr )
		return Debug::addMessage( "Map \"" + name + "\" attempting construction without parent.", Debug::DebugType::Error );

	const auto it = getMap( name );
	if( it == maps.end() )
		return Debug::addMessage( "Map \"" + name + "\" not found for construction.", Debug::DebugType::Error );

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

			if( layer.name == "Background" )
				texture.setSmooth( true );

			// Construct objects and render tiles
			for( size_t idx = 0u; idx < chunk.blockData.size(); ++idx ) {
				const int id = chunk.blockData.at( idx );

				if( !id )
					continue;

				// Find the tileset
				const auto jt = std::find_if( tilesets.begin(), tilesets.end(),
											  [id]( const Tileset& tileset ) {
												  return id >= tileset.uid;
											  } );

				if( jt == tilesets.end() )
					continue;

				const Tileset& tileset = *jt;

				// Calculate position
				Math::Vec2 position;
				position.x = float( idx % 16u ) * 16.0f;
				position.y = float( idx / 16u ) * 16.0f;
				Math::Vec2 worldPosition = position + layer.position + chunk.position;

				// Render to texture
				Gfx::Tileset::renderTile( tileset.name, id - tileset.uid, &texture, position );

				if( layer.name == "Terrain" ) {
					// Construct the collider
					const auto kt = it->tilemap.find( id );
					if( kt != it->tilemap.end() )
						constructCollider( kt->second, *it, worldPosition, world );
					else
						constructCollider( Tile(), *it, worldPosition, world );
				}

			}

			texture.display();
			chunk.texture = sf::Texture( texture.getTexture() );
		}
	}

	// Process Object Layers
	for( const auto& pair : it->objectLayers ) {
		const ObjectLayer& layer = pair.second;

		for( const MapObject& object : layer.objects )
			constructObject( object, *it, world );
	}

	// Unload Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::unloadTileset( tileset.name );

	Debug::stopTimer( "Map::Construct" );
	Debug::addMessage( "Map \"" + name + "\" constructed successfully.", Debug::DebugType::Info );
}

//--------------------------------------------------------------------------------

void constructObject( const MapObject& object, Map& map, Object* world ) {
	if( object.type == "Checkpoint" ) {
		// Player Start
		if( object.name == "S" ) {
			shared_ptr< Game::Player > player = Object::makeObject< Game::Player >( world );
			Math::Vec2 position;
			position.y = object.position.y + object.size.y - player->getSize().y;
			position.x = object.position.x + object.size.x / 2.0f;
			player->setPosition( position );
 			player->setSpawn( position );
			player->setName( "Player" );
			map.objects[ "Player" ].push_back( player );
			return;
		}

		// Level End
		if( object.name == "E" ) {
			shared_ptr< Game::RigidRect > levelEnd = Object::makeObject< Game::RigidRect >( world );
			levelEnd->setPosition( object.position );
			levelEnd->setSize( object.size );
			levelEnd->setName( "Level End" );
			levelEnd->setCollisionType( CollisionType::Static );
			levelEnd->setUid( object.id );
			map.objects[ object.type ].push_back( levelEnd );
			return;
		}
		
		// Checkpoint
		shared_ptr< Game::RigidRect > checkpoint = Object::makeObject< Game::RigidRect >( world );
		checkpoint->setPosition( object.position );
		checkpoint->setSize( object.size );
		checkpoint->setName( "Checkpoint" );
		checkpoint->setCollisionType( CollisionType::Static );
		checkpoint->setUid( object.id );
		map.objects[ object.type ].push_back( checkpoint );
		return;
	}

	// Events
	if( object.type == "Event" ) {
		shared_ptr< Game::RigidRect > event = Object::makeObject< Game::RigidRect >( world );
		event->setPosition( object.position );
		event->setSize( object.size );
		event->setName( object.name );
		event->setCollisionType( CollisionType::Static );
		event->getRect().setFillColor( sf::Color( 255u, 255, 0u, 100u ) );
		event->setUid( object.id );
		map.objects[ object.type ].push_back( event );
		return;
	}

	// Camera Volumes
	if( object.type == "Camera" ) {
		shared_ptr< Game::CameraVolume > volume = Object::makeObject< Game::CameraVolume >( world );
		volume->setPosition( object.position );
		volume->setSize( object.size );
		volume->setName( object.name );
		volume->setCollisionType( CollisionType::Static );
		volume->getRect().setOutlineColor( Colors::CYAN.sf() );
		volume->setUid( object.id );
		map.objects[ object.type ].push_back( volume );

		const auto it = std::find_if( object.properties.begin(), object.properties.end(),
									  []( const Property& property ) {
										  return property.name == "Position";
									  } );
		if( it == object.properties.end() )
			volume->setCameraPosition( volume->getPosition() + volume->getSize() / 2.0f );
		
		for( Property property : object.properties ) {
			if( property.name == "Position" ) {
				const int id = property.objectValue;
				if( id == -1 || !map.objectIds.count( id ) )
					volume->setCameraPosition( volume->getPosition() + volume->getSize() / 2.0f );
				else
					volume->setCameraPosition( map.objectIds[ id ].position );
				continue;
			}

			if( property.name == "Distance" ) {
				volume->setDistance( property.floatValue );
				continue;
			}
		}
	}

	shared_ptr< Game::RigidRect > collider = Object::makeObject< Game::RigidRect >( world );
	collider->setPosition( object.position );
	collider->setSize( object.size );
	collider->setName( object.name );
	collider->setCollisionType( CollisionType::Static );
	collider->setUid( object.id );
	map.objects[ object.type ].push_back( collider );
}

//--------------------------------------------------------------------------------

void constructCollider( const Tile& tile, Map& map, Math::Vec2 position, Object* world ) {
	if( tile.type == "Trap" ) {
		array< Math::Vec2, 9u > normalMap = {
			{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { -1.0f, 1.0f },
			{ 1.0f, 0.0f }, { 0.0f, 0.0f }, { -1.0f, 0.0f },
			{ 1.0f, -1.0f }, { 0.0f, -1.0f }, { -1.0f, -1.0f } } };

		if( tile.id >= normalMap.size() )
			return;

		Math::Vec2 normal = normalMap.at( tile.id );

		if( normal.x ) {
			shared_ptr< Game::RigidRect > trap = Object::makeObject< Game::RigidRect >( world );
			trap->setSize( Math::Vec2( 3.0f, 13.0f ) );
			trap->setCollisionType( CollisionType::Static );
			trap->setName( "Trap" );

			Math::Vec2 pos;
			pos.x = normal.x > 0.0f ? position.x : position.x + 16.0f - trap->getSize().x;
			pos.y = position.y + ( 16.0f - trap->getSize().y ) / 2.0f;
			trap->setPosition( pos );
			trap->setColor( Colors::CLEAR );
			trap->getRect().setOutlineColor( sf::Color::Red );
			trap->getRect().setOutlineThickness( 0.2f );
			//trap->setVisibility( true );

			map.objects[ tile.type ].push_back( trap );
		}

		if( normal.y ) {
			shared_ptr< Game::RigidRect > trap = Object::makeObject< Game::RigidRect >( world );
			trap->setSize( Math::Vec2( 13.0f, 3.0f ) );
			trap->setCollisionType( CollisionType::Static );
			trap->setName( "Trap" );
			trap->setColor( Colors::CLEAR );
			trap->getRect().setOutlineColor( sf::Color::Red );
			trap->getRect().setOutlineThickness( 0.35f );
			//trap->setVisibility( true );

			Math::Vec2 pos;
			pos.y = normal.y > 0.0f ? position.y : position.y + 16.0f - trap->getSize().y;
			pos.x = position.x + ( 16.0f - trap->getSize().x ) / 2.0f;
			trap->setPosition( pos );

			map.objects[ tile.type ].push_back( trap );
		}

		return;
	}

	shared_ptr< Game::RigidRect > object = Object::makeObject< Game::RigidRect >( world );
	object->setPosition( position );
	object->setSize( Math::Vec2( 16.0f, 16.0f ) );
	object->setCollisionType( CollisionType::Static );
	object->setColor( Colors::CLEAR );
	object->getRect().setOutlineColor( sf::Color( 150u, 150u, 150u, 255u ) );
	object->getRect().setOutlineThickness( 0.25f );
	object->setName( "Wall" );
	//object->setVisibility( true );

	map.objects[ "Wall" ].push_back( object );
}

//================================================================================

// RENDERING

//================================================================================

void renderMap( string name ) {
	Debug::startTimer( "Map::Render" );

	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	for( const Chunk& chunk : it->tileLayers[ "Background" ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ "Background" ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );

		// Darken the background
		sf::RectangleShape rect;
		rect.setPosition( position.sf() );
		rect.setSize( sf::Vector2f( 256.0f, 256.0f ) );
		rect.setFillColor( sf::Color( 0u, 0u, 0u, 64u ) );
		System::getWindow()->draw( rect );
	}

	for( const Chunk& chunk : it->tileLayers[ "Background Details" ].chunks ) {

		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ "Background Details" ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );
	}

	for( const Chunk& chunk : it->tileLayers[ "Terrain" ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		Math::Vec2 position = chunk.position + it->tileLayers[ "Terrain" ].position;
		sprite.setPosition( position.sf() );
		System::getWindow()->draw( sprite );
	}

	Debug::stopTimer( "Map::Render" );
}

//================================================================================

vector< shared_ptr< Object > > getObjects( string mapName, string objectName ) {
	vector< shared_ptr< Object > > out;

	const auto it = getMap( mapName );
	if( it == maps.end() )
		return out;

	const unordered_map < string, vector< shared_ptr< Object > > >& objectMap = it->objects;

	if( objectName != "" ) {
		const auto jt = objectMap.find( objectName );
		if( jt == objectMap.end() )
			return out;

		out.insert( out.end(), jt->second.begin(), jt->second.end() );
		return out;
	}

	// If there's no given name then just return everything
	for( const auto& pair : objectMap )
		out.insert( out.end(), pair.second.begin(), pair.second.end() );

	return out;
}

//================================================================================

vector< Map >::iterator getMap( string name ) {
	const auto it = find_if( maps.begin(), maps.end(),
							 [name]( const Map& map ) {
								 return map.name == name;
							 } );

	return it;
}

//--------------------------------------------------------------------------------

bool getString( const rapidjson::Value& data, string name, string& out ) {
	const auto it = data.FindMember( name.c_str() );
	if( it != data.MemberEnd() ) {
		if( it->value.IsString() ) {
			out = it->value.GetString();
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------

bool getBool( const rapidjson::Value& data, string name, bool& out ) {
	const auto it = data.FindMember( name.c_str() );
	if( it != data.MemberEnd() ) {
		if( it->value.IsBool() ) {
			out = it->value.GetBool();
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------

bool getInt( const rapidjson::Value& data, string name, int& out ) {
	const auto it = data.FindMember( name.c_str() );
	if( it != data.MemberEnd() ) {
		if( it->value.IsInt() ) {
			out = it->value.GetInt();
			return true;
		}
		else if( it->value.IsFloat() ) {
			out = int( it->value.GetFloat() );
			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------

bool getFloat( const rapidjson::Value& data, string name, float& out ) {
	const auto it = data.FindMember( name.c_str() );
	if( it != data.MemberEnd() ) {
		if( it->value.IsFloat() ) {
			out = it->value.GetFloat();
			return true;
		}
		else if( it->value.IsInt() ) {
			out = float( it->value.GetInt() );
			return true;
		}
	}

	return false;
}

//================================================================================

} // Map
} // Gfx

//================================================================================