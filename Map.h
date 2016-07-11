#ifndef MAP_H
#define MAP_H

#include "Globals.h"
#include "Ship.h"
#include "Text.h"

class Planetoid
{
  public:
  const unsigned char* BitMap;
  Vector2d* MapPosition;
  int Alignment;
  byte Attack;
  byte Defense;
  Loot Prize;
  bool Contacted;
};

extern Planetoid** Planets;
extern Planetoid* LatestPlanetEncountered;

void InitializePlanetsArray();
void NewMap();
void DeleteMap();
//void CreateEnemyShips( int count );
Vector2d RandomMapPosition();
void MapLoop();
void DrawMap();
void DrawMarkers();

#endif
