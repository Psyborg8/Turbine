//================================================================================

#include "Map.h"

//--------------------------------------------------------------------------------

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "MathTypes.h"
#include "Tileset.h"
#include "RigidRect.h"
#include "System.h"

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
	sf::Texture texture;
	string name;
	sf::Vector2u tileSize;
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
};

//================================================================================

vector< Map >::iterator getMap( string name );

//--------------------------------------------------------------------------------

vector< Map > maps;

//================================================================================

void renderMap( string name ) {
	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	sf::Sprite sprite( it->texture );
	System::getWindow()->draw( sprite );
}

//--------------------------------------------------------------------------------

void loadMap( string name ) {
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

			chunk.position.x = x;
			chunk.position.y = y;

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
}

//--------------------------------------------------------------------------------

void unloadMap( string name ) {
	const auto it = getMap( name );
	if( it != maps.end() )
		maps.erase( it );
}

//--------------------------------------------------------------------------------

void unloadResources( string mapName ) {
	const auto it = getMap( mapName );
	if( it == maps.end() )
		return;

	const vector< Tileset >& tilesets = it->tilesets;
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::unloadTileset( tileset.name );
}

//--------------------------------------------------------------------------------

void constructMap( string name, Object* world ) {
	// Constructing map without parent will leak memory
	if( world == nullptr )
		return;
	
	const auto it = getMap( name );
	if( it == maps.end() )
		return;

	const vector< Tileset >& tilesets = it->tilesets;
	const map< LayerType, Layer >& layers = it->layers;
	const sf::Vector2u mapTileSize = it->tileSize;

	// Load Tilesets
	for( const Tileset& tileset : tilesets )
		Gfx::Tileset::loadTileset( tileset.name, tileset.tileSize, tileset.path );

	// Construct tileset id map
	map< int, Tile > tilemap;
	map< int, Tileset > tilesetmap;
	for( const Tileset& tileset : tilesets ) {
		for( const Tile& tile : tileset.tiles ) {
			const int id = tile.id + tileset.uid;
			tilemap[ id ] = tile;
			tilesetmap[ id ] = tileset;
		}
	}

	sf::RenderTexture texture;
	texture.create( 256, 256 );

	// Create objects
	for( const pair< LayerType, Layer >& pair : layers ) {
		const LayerType& type = pair.first;
		const Layer& layer = pair.second;

		for( const Chunk& chunk : layer.chunks ) {
			for( size_t idx = 0u; idx < chunk.blockData.size(); ++idx ) {
				const int id = chunk.blockData.at( idx );

				if( !id )
					continue;

				const Tile& tile = tilemap[ id ];
				const Tileset& tileset = tilesetmap[ id ];

				// Make collision object
				shared_ptr< Game::RigidRect > rect =
					Object::makeObject< Game::RigidRect >( world );
				rect->setSize( Math::Vec2( mapTileSize.x, mapTileSize.y ) );

				Math::Vec2 position;
				position.x = idx % 16u * mapTileSize.x;
				position.y = idx / 16u * mapTileSize.y;
				position += Math::Vec2( layer.position.x + chunk.position.x, 
										layer.position.y + chunk.position.y );
				rect->setPosition( position );

				// Process tile attributes
				if( tile.type == TileType::Wall )
					rect->setName( "Wall" );
				if( tile.type == TileType::Trap )
					rect->setName( "Trap" );

				// Destroy it if it doesn't have collision
				if( rect->getName() == "" || type != LayerType::Terrain )
					rect->destroy();

				shared_ptr< sf::Sprite > ptr = Gfx::Tileset::getTileSprite( tileset.name, tile.id );
				if( ptr == nullptr )
					continue;

				ptr->setPosition( position.sf() );
				texture.draw( *ptr );
			}
		}
	}

	// Finalize render
	texture.display();
	it->texture = texture.getTexture();
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