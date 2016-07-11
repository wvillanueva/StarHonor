#ifndef SHIP_H
#define SHIP_H
#pragma once
#include "Globals.h"
#include "Vector2d.h"
#include "Text.h"

static const float RotationRate = 120.0f;
extern const unsigned char* PlayerBitMaps[];

//typedef enum AIState      { Idle, Search, Attack, Dead };

class Ship
{
  public:
  
  Ship();
  ~Ship();

  static void SetupShip( Ship* s );

  bool IsAlive;

  float MaxVelocity = 8.0f;
  float ShipAcceleration = 10.0;
  float ShipFriction = 1.0f;
  
  int HP_Hull;
  int HP_Weapons;
  int HP_Engine;
  int HP_Shields;
  int Crew;
  int Fuel;
//  int Food;
//  int Inv_Goods;

  uint8_t Max_Hull;
  uint8_t Max_Weapons;
  uint8_t Max_Engine;
  uint8_t Max_Shields;
  uint8_t Max_Crew;
  uint8_t Max_Fuel;
//  uint8_t Max_Food;

  char crewCharArray[4];
  char maxCrewCharArray[4];
  char fuelCharArray[4];
//  char goodsCharArray[4];
  
  Direction ShipFacing = Up;
  const unsigned char* BitMap;
//  AIState State;

//  Vector2d* NavFocus;
  Vector2d* MapPosition;

  SystemTarget TakeDamage( int Damage );
  SystemTarget TakeDamage( int Damage, SystemTarget target );
  void DrawOnMap();
  void Update();
  Vector2d GetVelocity();

  void PlayerUpdate();
  void UpdateMovement( Vector2d thrust );
//  void AIUpdate();
//  void ChooseNavLocation();
  Vector2d CalcThrust( Direction dir );
//  void AINavLoop();
//  bool ArrivedAtDestination();
//  static float NextRandomFireTime();
//  float DistanceToPlayerShip();
  int Upgrade( Loot Upgrade );
  void RepairSystem();
  void CalculateBattleRepairs();

  float ShipRotation;
  Vector2d* Velocity;
//  float timeUntilNextFire;
};

//void FireWeapon( Ship* owner, Ship* Target, WeaponType wt );  // Defined in Combat.cpp

extern Ship* PlayerShip;
//extern Ship** EnemyShips;

#endif
