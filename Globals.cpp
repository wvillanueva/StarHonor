#include "Globals.h"

ArduboyCustom arduboy;

float LastUpdateTime = 0;
float MilliPerFrame = 0;
float DeltaTime = 0;
int WaitTime = 0;
int StatusBlinkTime = 60;

State GameState;
State PreviousGameState;

int MenuWaitTime;
int SequenceStage = 0;
int CurrentSector = 1;
bool StatusUpdateAvailable = false;
int StatusUpdateTime = 240;
bool StatusUpdateFromProgMem = true;;

bool CanHail = false;
bool RunningAway = false;

Direction DPad;
bool AButton = false;
bool BButton = false;

bool newButtonInputAllowed = true;

Vector2d* MapUpperBounds;
Vector2d* MapLowerBounds;

int CombatPlanetDef, CombatPlanetDamage, CombatShipShieldDamage, CombatShipDamage;
SystemTarget SystemDamaged;
SystemTarget RepairTarget;

float TimeUntilNextRepair;
float RepairTime = 2.0f;
int BattleRepairs = 3;
int BattleRepairsMax = 3;

float seconds = 60;
int minutes = 9;

float TimeToNextRandomEncounter;
