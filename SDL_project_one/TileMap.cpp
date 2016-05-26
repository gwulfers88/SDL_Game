#include "TileMap.h"

vec2	WorldPosToTilePos(vec2 worldPos, TileMap *tileMap)
{
	vec2 tilePos = Vec2(worldPos.x / tileMap->tileWidth, worldPos.y / tileMap->tileHeight);
	return tilePos;
}

uint32	GetTileID(vec2 tilePos, TileMap *tileMap)
{
	uint32 tileID = (int)tilePos.y * (int)tileMap->width + (int)tilePos.x;
	return tileID;
}

vec2	TileIDToTilePos(uint32 TileID, TileMap *tileMap)
{
	vec2 pos = Vec2(TileID % tileMap->width, TileID / tileMap->width);
	return pos;
}
