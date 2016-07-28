#include "Ship.h"

Ship* PlayerShip;

float DiagonalVelocity = 1.41421f;

// Externed this variable because I couldn't get the compiler to let me delcare it as static
const unsigned char* PlayerBitMaps[] { ArduShip_TD_Up_16_16, ArduShip_TD_UpRight_16_16, ArduShip_TD_Right_16_16, ArduShip_TD_DownRight_16_16, ArduShip_TD_Down_16_16, ArduShip_TD_DownLeft_16_16,
                                       ArduShip_TD_Left_16_16, ArduShip_TD_UpLeft_16_16 };

Ship::Ship()
{
  IsAlive = true;
  Velocity = new Vector2d( 0, 0 );
  MapPosition = new Vector2d( 0, 0 );
}

Ship::~Ship()
{
  delete( MapPosition );
  delete( Velocity );
  delete[] crewCharArray;
  delete[] fuelCharArray;
  delete[] maxCrewCharArray;
}

void Ship::SetupShip( Ship* s )
{
  s->IsAlive = true;
  s->MaxVelocity = 8.0f;
  s->ShipRotation = 90;
  
  s->HP_Hull = 10;
  s->HP_Weapons = 5;
  s->HP_Shields = 5;
  s->HP_Engine = 5;
  s->MaxVelocity = s->HP_Engine + 5;
  s->Fuel = 0;
  s->Crew = 33;
  s->BitMap = ArduShip_TD_Right_16_16;
    
  s->Max_Hull = s->HP_Hull;
  s->Max_Weapons = s->HP_Weapons;
  s->Max_Engine = s->HP_Engine;
  s->Max_Shields = s->HP_Shields;
  s->Max_Crew = s->Crew;
  s->Max_Fuel = 25;

  s->Velocity->x = s->Velocity->y = 0;
  s->MapPosition->x = s->MapPosition->y = 0;
  RepairTarget = NoTarget;
}

// Applies damage to a ship, returns the system damaged
SystemTarget Ship::TakeDamage( int Damage )
{
  SystemTarget randomTarget = static_cast<SystemTarget>( rand() % 5 );
  return TakeDamage( Damage, randomTarget );
}

// Applies damage to a ship, returns the system damaged
SystemTarget Ship::TakeDamage( int Damage, SystemTarget target )
{
  switch ( target )
  {
    case SystemTarget::Crew:
      this->Crew -= Damage;
      this->Crew = max( this->Crew, 0 );
    break;
    case SystemTarget::Hull:
      this->HP_Hull -= Damage;
    break;
    case SystemTarget::Weapons:
      this->HP_Weapons -= Damage;
      this->HP_Weapons = max( this->HP_Weapons, 0 );
    break;
    case SystemTarget::Engines:
      this->HP_Engine -= Damage;
      this->HP_Engine = max( this->HP_Engine, 0 );
    break;
    case SystemTarget::Shields:
      this->HP_Shields -= Damage;
      this->HP_Shields = max( this->HP_Shields, 0 );
    break;
  }
  
  if ( HP_Hull <= 0 || Crew <= 0)
  {
    IsAlive = false;
  }

  return target;
}

void Ship::DrawOnMap()
{
  if ( IsAlive )
  {
    arduboy.drawBitmap( 56, 25, BitMap, 16, 16, 1 );  
  }
}

void Ship::Update()
{
  PlayerUpdate();
}

Vector2d Ship::GetVelocity()
{
  return *Velocity;
}

//typedef enum { Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft, None } Direction;
void Ship::PlayerUpdate()
{  
  // Be clever and arrange the enums so you can do a bit mask?
  if ( DPad == UpLeft || DPad == Left || DPad == DownLeft )
  {
    ShipRotation -= RotationRate * DeltaTime;
    ( ShipRotation > 360 ) ? ShipRotation -= 360 : ( ( ShipRotation < 0 ) ? ShipRotation += 360 : ShipRotation );
  }
  else if (DPad == UpRight || DPad == Right || DPad == DownRight )
  {
    ShipRotation += RotationRate * DeltaTime;
    ( ShipRotation > 360 ) ? ShipRotation -= 360 : ( ( ShipRotation < 0 ) ? ShipRotation += 360 : ShipRotation );
  }

  int rotation = ShipRotation / 45;
  ShipFacing = static_cast<Direction>( rotation );
  
  // 28,200 -> 28,138
  PlayerShip->BitMap = PlayerBitMaps[ShipFacing];
  
  // Friction
  if (! ( DPad == Up || DPad == UpRight || DPad == UpLeft ) )
  {
    Vector2d friction = *Velocity;
    friction = friction * -1.0f * ShipFriction * DeltaTime;
    *Velocity += friction;
  }

  Vector2d thrust = Vector2d( 0, 0 );
  if ( DPad == Up || DPad == UpRight || DPad == UpLeft )
    thrust = CalcThrust( ShipFacing );
  else if ( DPad == Down || DPad == DownRight || DPad == DownLeft )
    thrust = CalcThrust( ShipFacing ) * -0.4f;
  
  UpdateMovement( thrust );

  {
    TimeUntilNextRepair -= DeltaTime;
    if ( TimeUntilNextRepair <= 0 )
    {
      TimeUntilNextRepair = RepairTime;// + ( RepairTime * 10 - RepairTime * 10 * Crew / Max_Crew );
      RepairSystem();
    }
  }
}

void Ship::UpdateMovement( Vector2d thrust )
{
  *Velocity += ((thrust * ShipAcceleration) * DeltaTime);
  if (Velocity->MagnitudeSquared() > (MaxVelocity * MaxVelocity))
  {
    Velocity->Normalize();
    (*Velocity) = (*Velocity) * MaxVelocity;
  }

  *(this->MapPosition) = *(this->MapPosition) + ( *Velocity * DeltaTime ) * 4;

  if ( this->MapPosition->x < MapUpperBounds->x )
  {
    this->MapPosition->x = MapUpperBounds->x;
    Velocity->x = 0;
  }
  if ( this->MapPosition->y < MapUpperBounds->y )
  {
    this->MapPosition->y = MapUpperBounds->y;
    Velocity->y = 0;
  }
  if ( this->MapPosition->x > MapLowerBounds->x )
  {
    this->MapPosition->x = MapLowerBounds->x;
    Velocity->x = 0;
  }
  if ( this->MapPosition->y > MapLowerBounds->y )
  {
    this->MapPosition->y = MapLowerBounds->y;
    Velocity->y = 0;
  }
}

Vector2d Ship::CalcThrust( Direction dir )
{
  Vector2d thrust;
  thrust.x = 0;
  thrust.y = -0.2f - HP_Engine / 3.0f;
  int rotation = static_cast<int>( dir ) * 45;
  thrust.Rotate( rotation );
  return thrust;
}

int Ship::Upgrade( Loot Upgrade )
{
  int Max = 50;
  int UpgradeAmount = 1;
  switch ( Upgrade )
  {
    case NoLoot:
    break;
    case LootHull:
      HP_Hull += UpgradeAmount;
      Max_Hull += UpgradeAmount;
      HP_Hull = min( HP_Hull, Max );
      Max_Hull = min( Max_Hull, Max );
      return UpgradeAmount;
    break;
    case LootWeapons:
      HP_Weapons += UpgradeAmount;
      Max_Weapons += UpgradeAmount;
      HP_Weapons = min( HP_Weapons, Max );
      Max_Weapons = min( Max_Weapons, Max );
      return UpgradeAmount;
    break;
    case LootShields:
      HP_Shields += UpgradeAmount;
      Max_Shields += UpgradeAmount;
      HP_Shields = min( HP_Shields, Max );
      Max_Shields = min( Max_Shields, Max );
      return UpgradeAmount;
    break;
    case LootEngines:
      HP_Engine += UpgradeAmount;
      Max_Engine += UpgradeAmount;
      HP_Engine = min( HP_Engine, Max );
      Max_Engine = min( Max_Engine, Max );
      MaxVelocity += UpgradeAmount;
      return UpgradeAmount;
    break;
    case LootCrew:
      UpgradeAmount *= 8;
      Crew += UpgradeAmount;
      Max_Crew += UpgradeAmount;
      return UpgradeAmount;
    break;
    case LootFuel:
      UpgradeAmount = 1;
      Fuel += UpgradeAmount;
      return UpgradeAmount;
    break;
  }
  return 0;
}

void Ship::RepairSystem()
{
  if ( RepairTarget == NoTarget ) return;
  
  bool existingUpdate = StatusUpdateAvailable;
  SystemTarget systemTargeted = RepairTarget;
  
  switch (RepairTarget)
  {
    
    case SystemTarget::Crew:
      if ( (Crew + 1) > Max_Crew )
      {
        Crew = Max_Crew;
        StatusUpdateAvailable = true;
        RepairTarget = NoTarget;
      }
      else
        Crew += 1;
    break;
    case SystemTarget::Hull:
      if ( (HP_Hull + 1) > Max_Hull )
      {
        HP_Hull = Max_Hull;
        StatusUpdateAvailable = true;
        RepairTarget = NoTarget;
      }
      else
        HP_Hull += 1;
    break;
    case SystemTarget::Weapons:
      
      if ( (HP_Weapons + 1) > Max_Weapons )
      {
        HP_Weapons = Max_Weapons;
        StatusUpdateAvailable = true;
        RepairTarget = NoTarget;
      }
      else
        HP_Weapons += 1;
    break;
    case SystemTarget::Engines:
      if ( (HP_Engine + 1) > Max_Engine )
      {
        HP_Engine = Max_Engine;
        StatusUpdateAvailable = true;
        RepairTarget = NoTarget;
      }
      else
        HP_Engine += 1;
    break;
    case SystemTarget::Shields:
      if ( (HP_Shields + 1) > Max_Shields )
      {
        HP_Shields = Max_Shields;
        StatusUpdateAvailable = true;
        RepairTarget = NoTarget;
      }
      else
        HP_Shields += 1;
    break;
  }
  if ( !existingUpdate && StatusUpdateAvailable )
  {
    StatusUpdateTime = 240;
    Status_Update = (char *) RepairedText[systemTargeted];
    StatusUpdateFromProgMem = true;
  }
}

void Ship::CalculateBattleRepairs()
{
  BattleRepairsMax = min( PlayerShip->Crew / 10 - 2, 5 );
  BattleRepairs = BattleRepairsMax;
}

//void Ship::AIUpdate()
//{
//  switch ( State )
//  {
//    case Idle:
//      break;
//
//    case Search:
//      AINavLoop();
//      break;
//
//    case Attack:
//      if ( DistanceToPlayerShip() < 2500 )
//      {
//        timeUntilNextFire -= DeltaTime;
//        if ( timeUntilNextFire < 0 )
//        {
//        }
//      }
//      AINavLoop();
//      break;
//
//    case Dead:
//      break;
//  }
//}

//void Ship::ChooseNavLocation()
//{
//  switch ( State )
//  {
//    case Search:
//      NavFocus->x = PlayerShip->MapPosition->x + ( random( -1, 2 ) * random( 10, 50 ) );
//      NavFocus->y = PlayerShip->MapPosition->y + ( random( -1, 2 ) * random( 10, 50 ) );
//    case Attack:
//      int randomX = random( 0, 50 ) * random ( -1, 2 );
//      int randomY = random( 0, 40 ) * random ( -1, 2 );
//
//      *NavFocus = *(PlayerShip->MapPosition) + Vector2d( randomX, randomY );
//    break;
//  }
//}

// NOTE Optimize
// 798 Bytes
//void Ship::AINavLoop()
//{
//  //arduboy.drawCircle( NavFocus->x - PlayerShip->MapPosition->x + 64, NavFocus->y - PlayerShip->MapPosition->y + 32, 4, 1 );
//  Direction dir = Up;
//  Vector2d thrust;
//  Vector2d vectorToTarget;
//  float angle;
//  int rotation;
//
//  if ( ArrivedAtDestination() )
//    ChooseNavLocation();
//
////  switch ( State )
////  {
////    case Search:
////      vectorToTarget = *NavFocus - *MapPosition;
////    break;
////    
////    case Attack:
////      vectorToTarget = *NavFocus - *MapPosition;
////    break;
////  }
//  
//  angle = atan2( vectorToTarget.y, vectorToTarget.x ) * 180 / PI;
//  angle = ((int) angle + 270) % 360;
//  rotation = (int)(round(angle / 45 + 4)) % 8;
//  dir = static_cast<Direction>( rotation );
//
//  thrust = CalcThrust( dir );
//  UpdateMovement( thrust );
//}

//bool Ship::ArrivedAtDestination()
//{
//  Vector2d distance = *NavFocus - *MapPosition;
//  Vector2d navToShipDistance = *NavFocus - *(PlayerShip->MapPosition);
//  return ( distance.MagnitudeSquared() < 100 || navToShipDistance.MagnitudeSquared() > 2500 );
//}

//float Ship::DistanceToPlayerShip()
//{
//  Vector2d dV = *MapPosition - *(PlayerShip->MapPosition);
//  return dV.MagnitudeSquared();
//}

