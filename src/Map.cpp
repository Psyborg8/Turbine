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

enum class DevSymbol {
	Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
	A, B, C, D, E, S, Cross, Tick, Warning, Dash
};

//--------------------------------------------------------------------------------

struct Tile {
	int id;
	TileType type;
};

//--------------------------------------------------------------------------------

struct Tileset {
	vector< Tile > tiles;
	string name;
	string path;
	int uid;
	sf::Vector2u tileSize;
};

//--------------------------------------------------------------------------------

struct Chunk {
	array< int, 16u * 16u > blockData;
	sf::Vector2i position; 
	sf::Texture texture;
};

//--------------------------------------------------------------------------------

struct Layer {
	vector< Chunk > chunks;
	sf::Vector2i position;
};

//--------------------------------------------------------------------------------

struct Map {
	map< LayerType, Layer > layers;
	vector< Tileset > tilesets;
	string name;
	sf::Vector2u tileSize;
	sf::Vector2u mapSize;
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

//================================================================================

vector< Map >::iterator getMap( string name );

//--------------------------------------------------------------------------------

vector< Map > maps;

//================================================================================

void renderMap( string name ) {
	Debug::startTimer( "Map::Render" );

	const auto it = getMap( name );
	if( it == maps.end() )
		return;


	for( const Chunk& chunk : it->layers[ LayerType::Background ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		sf::Vector2i position = chunk.position + it->layers[ LayerType::Background ].position;
		sprite.setPosition( sf::Vector2f( float( position.x ), float( position.y ) ) );
		System::getWindow()->draw( sprite );
	}

	for( const Chunk& chunk : it->layers[ LayerType::BackgroundDetails ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		sf::Vector2i position = chunk.position + it->layers[ LayerType::BackgroundDetails ].position;
		sprite.setPosition( sf::Vector2f( float( position.x ), float( position.y ) ) );
		System::getWindow()->draw( sprite );
	}

	for( const Chunk& chunk : it->layers[ LayerType::Terrain ].chunks ) {
		sf::Sprite sprite( chunk.texture );
		sf::Vector2i position = chunk.position + it->layers[ LayerType::Terrain ].position;
		sprite.setPosition( sf::Vector2f( float( position.x ), float( position.y ) ) );
		System::getWindow()->draw( sprite );
	}

	Debug::stopTimer( "Map::Render" );
}

//--------------------------------------------------------------------------------

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

	if( document.HasParseError() )
		return;

	// Map Data
	const int tileWidth = document[ "tilewidth" ].GetInt();
	const int tileHeight = document[ "tileheight" ].GetInt();
	map.tileSize.x = tileWidth;
	map.tileSize.y = tileHeight;

	const int mapwidth = document[ "width" ].GetInt();
	const int mapheight = document[ "height" ].GetInt();
	map.mapSize.x = mapwidth;
	map.mapSize.y = mapheight;

	// Layers
	GenericArray< false, Value > layers = document[ "layers" ].GetArray();
	for( SizeType i = 0; i < layers.Size(); ++i ) {
		const string name = layers[ i ][ "name" ].GetString();
		const int startx = layers[ i ][ "startx" ].GetInt();
		const int starty = layers[ i ][ "starty" ].GetInt();

		LayerType layer = layerMap.at( name );
		map.layers[ layer ] = Layer();

		map.layers.at( layer ).position.x = startx;
		map.layers.at( layer ).position.y = starty;

		// Chunks
		GenericArray< false, Value > chunks = layers[ i ][ "chunks" ].GetArray();
		for( SizeType j = 0; j < chunks.Size(); ++j ) {
			Chunk chunk;

			Math::Vec2 position;
			const int x = chunks[ j ][ "x" ].GetInt();
			const int y = chunks[ j ][ "y" ].GetInt();

			chunk.position.x = x * map.tileSize.x;
			chunk.position.y = y * map.tileSize.y;

			// Data
			GenericArray< false, Value > data = chunks[ j ][ "data" ].GetArray();
			for( SizeType k = 0; k < data.Size(); ++k ) {
				chunk.blockData[ k ] = data[ k ].GetInt();
			}

			map.layers.at( layer ).chunks.push_back( chunk );
		}
	}

	// Tilesets
	GenericArray< false, Value > tilesets = document[ "tilesets" ].GetArray();
	for( SizeType i = 0; i < tilesets.Size(); ++i ) {
		Tileset tileset;

		tileset.uid = tilesets[ i ][ "firstgid" ].GetInt();
		tileset.path = Folders::Maps + tilesets[ i ][ "image" ].GetString();
		tileset.name = tilesets[ i ][ "name" ].GetString();
		const int tileheight = tilesets[ i ][ "tileheight" ].GetInt();
		const int tilewidth = tilesets[ i ][ "tilewidth" ].GetInt();
		tileset.tileSize.y = tileheight;
		tileset.tileSize.x = tilewidth;

		const GenericArray< false, Value > tiles = tilesets[ i ][ "tiles" ].GetArray();
		for( SizeType j = 0; j < tiles.Size(); ++j ) {
			Tile tile;
			tile.id = tiles[ j ][ "id" ].GetInt();
			tile.type = tileMap.at( tiles[ j ][ "type" ].GetString() );
			tileset.tiles.push_back( tile );
		}

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

void constructMap( string name, Object* world ) {
	Debug::startTimer( "Map::Construct" );

	// Constructing map without parent will leak memory
	if( world == nullptr )
		return;
	
	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	vector< Tileset >& tilesets = it->tilesets;
	const sf::Vector2u mapTileSize = it->tileSize;

	// Load Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::loadTileset( tileset.name, tileset.tileSize, tileset.path );

	// Create objects
	for( auto& pair : it->layers ) {
		LayerType type = pair.first;
		Layer& layer = pair.second;

		for( Chunk& chunk : layer.chunks ) {
			// Render chunk to a texture to save on render calls
			sf::RenderTexture texture;
			texture.create( it->tileSize.x * 16u, it->tileSize.y * 16u );
			texture.clear( sf::Color( 0u, 0u, 0u, 0u ) );

			if( type == LayerType::Background )
				texture.setSmooth( true );

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
				position.x = float( idx % 16u * mapTileSize.x );
				position.y = float( idx / 16u * mapTileSize.y );
				Math::Vec2 worldPosition = position + Math::Vec2( float( layer.position.x + chunk.position.x ), 
										float( layer.position.y + chunk.position.y ) );

				// Process tile attributes
				const Tile& tile = tileset.tiles[ id - tileset.uid ];

				// Render to texture
				Gfx::Tileset::renderTile( tileset.name, tile.id, &texture, position );

				// Construct collider
				if( type == LayerType::Terrain ) {

					shared_ptr< Game::RigidRect > rect =
						Object::makeObject< Game::RigidRect >( world );
					rect->setSize( Math::Vec2( float( mapTileSize.x ), float( mapTileSize.y ) ) );
					rect->setPosition( worldPosition );

					if( tile.type == TileType::Wall )
						rect->setName( "Wall" );
					if( tile.type == TileType::Trap )
						rect->setName( "Trap" );

					rect->setCollisionType( CollisionType::Static );
				}

				// Create the checkpoints and player
				if( type == LayerType::Checkpoints && tile.type == TileType::DevSymbol ) {
					const DevSymbol symbol = static_cast< DevSymbol >( tile.id );

					// Player start
					if( symbol == DevSymbol::S ) {
						shared_ptr< Game::Player > player = Object::makeObject< Game::Player >( world );
						player->setPosition( worldPosition );
						player->setSpawn( worldPosition );
					}
				}
			}
			texture.display();
			chunk.texture = sf::Texture( texture.getTexture() );
		}
	}

	// Load Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::unloadTileset( tileset.name );

	Debug::stopTimer( "Map::Construct" );
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