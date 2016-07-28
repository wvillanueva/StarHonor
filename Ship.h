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

  uint8_t Max_Hull;
  uint8_t Max_Weapons;
  uint8_t Max_Engine;
  uint8_t Max_Shields;
  uint8_t Max_Crew;
  uint8_t Max_Fuel;

  char crewCharArray[4];
  char maxCrewCharArray[4];
  char fuelCharArray[4];
  
  Direction ShipFacing = Up;
  const unsigned char* BitMap;
//  AIState State;

  Vector2d* MapPosition;

  SystemTarget TakeDamage( int Damage );
  SystemTarget TakeDamage( int Damage, SystemTarget target );
  void DrawOnMap();
  void Update();
  Vector2d GetVelocity();

  void PlayerUpdate();
  void UpdateMovement( Vector2d thrust );

  Vector2d CalcThrust( Direction dir );
  
  int Upgrade( Loot Upgrade );
  void RepairSystem();
  void CalculateBattleRepairs();

  float ShipRotation;
  Vector2d* Velocity;
};

extern Ship* PlayerShip;
#endif
