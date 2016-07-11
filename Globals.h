#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once

#include "ArduboyCustom.h"
#include "bitmaps.h"
#include "Vector2d.h"

#define PI 3.14159265

extern ArduboyCustom arduboy;

//extern int FPS;
extern float LastUpdateTime;
extern float MilliPerFrame;
extern float DeltaTime;
extern int WaitTime;
extern int StatusBlinkTime;

typedef enum { Title, TitleLoop, Prologue, Map, Status, Encounter, GameOver, TimeUp, Reset, WinGame, Warping } State;
extern State GameState;
extern State PreviousGameState;
extern int MenuWaitTime;
extern int SequenceStage;
extern int CurrentSector;
extern bool StatusUpdateAvailable;
extern int StatusUpdateTime;  // Just squeaking on by trying to save bytes
extern bool StatusUpdateFromProgMem;

extern bool CanHail;
extern bool RunningAway;

typedef enum { Overview, Repair, Auxilary } StatusScreen;
//extern StatusScreen CurrentStatusScreen;

typedef enum { Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft, None } Direction;
extern Direction DPad;
extern bool AButton;
extern bool BButton;

typedef enum SystemTarget { Crew, Hull, Weapons, Shields, Engines, NoTarget };
extern SystemTarget RepairTarget;
extern SystemTarget SystemDamaged;

// For menus!
extern bool newInputAllowed;
extern bool newButtonInputAllowed;

extern Vector2d* MapUpperBounds;
extern Vector2d* MapLowerBounds;

//typedef enum WeaponType { Torpedo, Phaser };

typedef enum Loot { NoLoot, LootHull, LootWeapons, LootShields, LootEngines, LootCrew, LootFuel }; //, LootGoods };

// Weird! It sorta makes sense?  Declare function in Globals.h that is defined in VoyageHome.ino.  So the linker finds it there?  Smart compiler?
void ChangeGameState( State newState );

extern int CombatPlanetDef, CombatPlanetDamage, CombatShipShieldDamage, CombatShipDamage;

extern float TimeUntilNextRepair;
extern float RepairTime;
extern int BattleRepairs;
extern int BattleRepairsMax;

extern float seconds;
extern int minutes;

//extern float TimeToNextRandomEncounter;
//struct RandomShipEncounter
//{
//  int Attack;
//  int Defense;
//  Loot Prize;
//};
//
//extern RandomShipEncounter* ShipEncountered;

#endif
