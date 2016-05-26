#ifndef TILEMAP_H
#define TILEMAP_H

#include "common.h"
#include "vec2.h"

struct TileMap
{
	uint32 width;
	uint32 height;
	uint32 tileWidth;
	uint32 tileHeight;
	uint32 numLayers;
};

vec2	WorldPosToTilePos(vec2 worldPos, TileMap *tileMap);
vec2	TileIDToTilePos(uint32 TileID, TileMap *tileMap);
uint32	GetTileID(vec2 tilePos, TileMap *tileMap);

#endif