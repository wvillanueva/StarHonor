#include "Map.h"

Planetoid** Planets;
Planetoid* LatestPlanetEncountered;
const int PROGMEM PlanetsPerMap = 10;

const unsigned char* PlanetArt[] { Planet_1_16_16, Planet_2_16_16, Planet_3_16_16, Planet_4_16_16, Planet_5_16_16, Planet_6_16_16, Planet_7_16_16, Planet_8_16_16, Planet_9_16_16 , Planet_10_16_16 };

void InitializePlanetsArray()
{
  Planets = new Planetoid*[PlanetsPerMap];
  for ( int i(0); i < PlanetsPerMap; i++ )
  {
    Planets[i] = new Planetoid();
    Planets[i]->MapPosition = new Vector2d( 0, 0 );
  }
}

void NewMap()
{
  int fuel = 3;
  int goodPlanets = (int) PlanetsPerMap / 3.0f;
  
  for ( int i(0); i < PlanetsPerMap; i++ )
  {
    Vector2d randPos = RandomMapPosition();
    Planets[i]->MapPosition->x = randPos.x;
    Planets[i]->MapPosition->y = randPos.y;
    
    Planets[i]->BitMap = PlanetArt[random(0, 10)];
    if ( goodPlanets > 0 )
    {
      Planets[i]->Alignment = goodPlanets;
      goodPlanets--;
    }
    else
      Planets[i]->Alignment = round(random( -3, 2 ));
    
    Planets[i]->Attack = CurrentSector + random( 1, 3 ) + 3;
    Planets[i]->Defense = CurrentSector + random( 3, 5 ) + 3;
    if ( fuel > 0 )
    {
      Planets[i]->Prize = LootFuel;
      fuel--;
    }
    else
      Planets[i]->Prize = static_cast<Loot>( rand() % 7 );
    Planets[i]->Contacted = false;
  }
}

void DeleteMap()
{
  LatestPlanetEncountered = NULL;
}

Vector2d RandomMapPosition()
{
  Vector2d pos;
  pos.x = random( MapUpperBounds->x + 16, MapLowerBounds->x - 16);
  pos.y = random ( MapUpperBounds->y + 16, MapLowerBounds->y - 16 );
  return pos;
}

void MapLoop()
{
  if ( AButton )
  {
    ChangeGameState( Status );
  }

  DrawMap();
}

void DrawMap()
{
  if ( Planets == NULL ) return;
  
  // Draw Planets
  for ( int i(0); i < PlanetsPerMap; i++ )
  {
    Vector2d distance = *Planets[i]->MapPosition - *PlayerShip->MapPosition;
    
    if ( abs( distance.x ) < 80 || abs( distance.y ) < 48 )
    {
      arduboy.drawBitmap( distance.x + 56, distance.y + 24, Planets[i]->BitMap, 16, 16, 1 );

      if ( distance.MagnitudeSquared() < 256 && LatestPlanetEncountered == NULL && !(Planets[i]->Contacted) )
      {
        CanHail = true;
        LatestPlanetEncountered = Planets[i];
      }
    }

    // Wait until we've moved away before triggering the planet again
    if ( LatestPlanetEncountered != NULL && Planets[i] == LatestPlanetEncountered )
    {
      if ( LatestPlanetEncountered->Contacted || distance.MagnitudeSquared() > 256 )
      {
        CanHail = false;
        LatestPlanetEncountered = NULL;
      }
    }
  }
}

void DrawMarkers()
{
  // 50 Bytes Savings
//  int xMarker = ( ( PlayerShip->MapPosition->x + abs(MapUpperBounds->x ) ) / ( MapLowerBounds->x - MapUpperBounds->x ) ) * 125;
//  int yMarker = ( PlayerShip->MapPosition->y + (( MapLowerBounds->y - MapUpperBounds->y ) / 2.0f ))  / ( MapLowerBounds->y - MapUpperBounds->y ) * 61;
  arduboy.drawRect( ( ( PlayerShip->MapPosition->x + abs(MapUpperBounds->x ) ) / ( MapLowerBounds->x - MapUpperBounds->x ) ) * 125, 0, 3, 3, 1 );
  arduboy.drawRect( 0, ( PlayerShip->MapPosition->y + (( MapLowerBounds->y - MapUpperBounds->y ) / 2.0f ))  / ( MapLowerBounds->y - MapUpperBounds->y ) * 61, 3, 3, 1 );
}

