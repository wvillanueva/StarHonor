
#include <SPI.h>
#include <EEPROM.h>
#include "ArduboyCustom.h"
#include "Globals.h"
#include "GUI.h"
#include "StarField.h"
#include "Ship.h"
#include "Map.h"
#include "Text.h"

#define Debug 0

// avr-nm --size-sort -C -r ./VoyageHome.ino.elf > ~/output.txt

StarField* _StarField;

void setup() {
  // put your setup code here, to run once:
  #if Debug
    Serial.begin(9600);
  #endif
  MilliPerFrame = 1000.0f / 60.0f;

  arduboy.start();
  arduboy.display();
  arduboy.initRandomSeed();

#if !Debug
  _StarField = new StarField();
#endif

  MapUpperBounds = new Vector2d(-32, -128);
  MapLowerBounds = new Vector2d(512, 128);
//  MapUpperBounds = new Vector2d(-128, -128);
//  MapLowerBounds = new Vector2d(128, 128);

  TextManager = new Text();
#if !Debug
  CreateStatusSelectionArrow();

  CreateCombatSelectionArrow();
#endif
//  CurrentStatusScreen = Overview;
//  ChangeGameState( WinGame );
  ChangeGameState( TitleLoop );
//  ChangeGameState( Map );

  MenuWaitTime = 15;

  // Order is important to prevent Heap Fragmentation
  PlayerShip = new Ship();
  Ship::SetupShip( PlayerShip );
  TimeUntilNextRepair = RepairTime;
  InitializePlanetsArray();
  NewMap();
}

void loop()
{
//  Serial.println(MilliPerFrame);
  if ( millis() < (LastUpdateTime + MilliPerFrame) ) return;
  DeltaTime = ( millis() - LastUpdateTime ) / 1000.0f;
  LastUpdateTime = millis();
  
  arduboy.clearDisplay();
//  AbPrinter text(arduboy);
//  text.print("Hello");
  GetInput();
  
  switch ( GameState )
  {
    case TitleLoop:
      _StarField->Draw();
      Text::DisplayTextClear( TitleScreen2, 6, 56, true, false );
      if ( TextManager->DisplayTextOverTime( TitleScreen, 42, 24 ) && BButton )
        ChangeGameState( Prologue );
    break;
    case Prologue:
      PrologueLoop();
    break;
    case Map:
      PlayerShip->Update();
      PlayerShip->DrawOnMap();
      
      #if !Debug
      _StarField->Move(PlayerShip->GetVelocity() * -1.0f);
      _StarField->Draw();
      #endif
      
      MapLoop();

      if ( StatusUpdateAvailable )
      {
        Text::DisplayTextClear( Status_Update, 0, 6, StatusUpdateFromProgMem, true );
        StatusUpdateTime -= 1;
        StatusUpdateAvailable = StatusUpdateTime > 0;
      }
      ClockUpdate( true );

      if ( CanHail )
      {
        Text::DisplayTextClear( Hail, 12, 56, true, false );
        if ( BButton )
        {
          ChangeGameState( Encounter );
          return;
        }
      }
      else if ( PlayerShip->Fuel >= 3 )
      {
        Text::DisplayText( StatusHelp3, 0, 58, true );
        if ( DPad == Up && BButton )
        {
          ChangeGameState( Warping );
        }
      }
    break;
    case Status:
      ShipStatusLoop();
    break;
    case Encounter:
      _StarField->Draw();
      PlayerShip->DrawOnMap();
      DrawMap();
      EncouterUpdate();
    break;
    case GameOver:
      GameOverLoop( 1 );
    break;
    case TimeUp:
      GameOverLoop( 2 );
    break;
    case Reset:
      StatusUpdateAvailable = false;
      StatusUpdateTime = 0;
      Status_Update = NULL;
      CanHail = false;
//      DeleteMap();
      LatestPlanetEncountered = NULL;
      ResetPlayer();
      NewMap();
      ChangeGameState( TitleLoop );
      minutes = 9;
      seconds = 59.99;
    break;
    case Warping:
//      Vector2d warpVelocity;
//      warpVelocity.x = -10;
//      warpVelocity.y = 0;
      _StarField->Move(Vector2d(-60,0));
      _StarField->Draw();
      PlayerShip->DrawOnMap();
      WaitTime -= 1;
      if ( WaitTime <= 0 ) 
      {
        NextSector();
        if ( GameState == WinGame) return;
        
        ChangeGameState( Map );
        SetupSectorReachedText();
      }
    break;
    case WinGame:
      WinGameLoop();
    break;
  }

  arduboy.display();
}

void GetInput()
{
  int DPadDirection = 8;

  //typedef enum { Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft, None } Direction;
  if ( arduboy.pressed( UP_BUTTON ) )
      DPadDirection = 0;
  if ( arduboy.pressed( DOWN_BUTTON ) )
      DPadDirection = 4;
  if ( arduboy.pressed( LEFT_BUTTON ) )
  { 
    if ( DPadDirection == 0 )
      DPadDirection = 7;
    else if ( DPadDirection == 4 )
      DPadDirection = 5;
    else
      DPadDirection = 6;
  }
  if ( arduboy.pressed( RIGHT_BUTTON ) )
  {
    if ( DPadDirection == 0 )
      DPadDirection = 1;
    else if ( DPadDirection == 4 )
      DPadDirection = 3;
    else
      DPadDirection = 2;
  }
  DPad = static_cast<Direction>( DPadDirection );

  //
  //  Button Input
  //
  AButton = false;
  BButton = false;

  if ( newButtonInputAllowed == false && ( !arduboy.pressed( B_BUTTON ) && !arduboy.pressed( A_BUTTON ) ) )
  {
    newButtonInputAllowed = true;
  }

  if ( arduboy.pressed( A_BUTTON ) && newButtonInputAllowed )
  {
    newButtonInputAllowed = false;
    AButton = true;
  }
  if ( arduboy.pressed( B_BUTTON ) && newButtonInputAllowed )
  {
    newButtonInputAllowed = false;
    BButton = true;
  }
}

void CreateStatusSelectionArrow()
{
  OverviewLocations = new Vector2d*[6];
  OverviewLocations[0] = new Vector2d( 0, 6 );  // Ship Systems
  OverviewLocations[1] = new Vector2d( 0, 13 );  // Ship Hull
  OverviewLocations[2] = new Vector2d( 0, 20 );  // Ship Engines
  OverviewLocations[3] = new Vector2d( 0, 27 );  // Crew
  OverviewLocations[4] = new Vector2d( 0, 34 );  // Fuel
  OverviewLocations[5] = new Vector2d( 0, 41 );  // Food

  RepairSelectionArrow = new SelectionArrow( OverviewLocations, 6 );
  CurrentSelectionArrow = RepairSelectionArrow;
}

void CreateCombatSelectionArrow()
{
  CombatLocations = new Vector2d*[3];
  CombatLocations[0] = new Vector2d( 2, 43 );
  CombatLocations[1] = new Vector2d( 2, 49 );
  CombatLocations[2] = new Vector2d( 2, 56 );
  CombatSelectionArrow = new SelectionArrow( CombatLocations, 3 );
}

void ChangeGameState( State newState )
{
  PreviousGameState = GameState;
  switch ( newState )
  {
    case TitleLoop:
      CurrentSector = 1;
      TextManager->NewDisplayTextOverTime();
    break;
    case Prologue:
      SequenceStage = 1;
      TextManager->NewDisplayTextOverTime();
//      GenerateRandomEncounter();
    break;
    case Map:
//      SequenceStage = 1;
    break;
    case Status:
      Text::ConvertIntToChar( PlayerShip->Crew, PlayerShip->crewCharArray );
      Text::ConvertIntToChar( PlayerShip->Max_Crew, PlayerShip->maxCrewCharArray );
      Text::ConvertIntToChar( PlayerShip->Fuel, PlayerShip->fuelCharArray );
//      Text::ConvertIntToChar( PlayerShip->Inv_Goods, PlayerShip->goodsCharArray );
    break;
    case Encounter:
      SetupEncounter();
    break;
    case GameOver:
      PlayerShip->IsAlive = false;
      TextManager->NewDisplayTextOverTime();
      SequenceStage = 1;
    break;
    case TimeUp:
      PlayerShip->IsAlive = false;
      TextManager->NewDisplayTextOverTime();
      SequenceStage = 1;
    case Reset:
    break;
    case Warping:
      PlayerShip->ShipRotation = 90;
      PlayerShip->Velocity->x = 2;
      PlayerShip->Velocity->y = 0;
      PlayerShip->PlayerUpdate();
      PlayerShip->Fuel -= 3;
      WaitTime = 240; // 240 frames.  Changed from float to get under 28,672
    break;
    case WinGame:
      SequenceStage = 1;
      TextManager->NewDisplayTextOverTime();
    break;
  }

  GameState = newState;
}

void PrologueLoop()
{ 
  switch ( SequenceStage )
  {
    case 1:
      if ( TextManager->DisplayTextOverTimeClear( PrologueText1, 0, 0 ) || BButton )
        { SequenceStage++; TextManager->NewDisplayTextOverTime(); }
    break;
    case 2:
      Text::DisplayText( PrologueText1, 2, 2, true );
      if ( TextManager->DisplayTextOverTimeClear( PrologueText2, 0, 31 ) || BButton )
        SequenceStage++;
    break;
    case 3:
      Text::DisplayText( PrologueText1, 2, 2, true );
      Text::DisplayText( PrologueText2, 2, 33, true );
      if ( BButton )
       ChangeGameState(Map);
     break;
  }
}

void ShipStatusLoop()
{
  if ( DPad == None && AcceptMenuInput == false )
    AcceptMenuInput = true;

  if ( AcceptMenuInput )
  {
    if ( DPad == Up )
    {
      CurrentSelectionArrow->SelectionMoveUp();
      AcceptMenuInput = false;
    }
    else if ( DPad == Down )
    {
      CurrentSelectionArrow->SelectionMoveDown();
      AcceptMenuInput = false;
    }
//    else if ( DPad == Right || DPad == Left )
//    {
//      AcceptMenuInput = false;
//    }
    // Status Menu
    if ( AButton )
    {
      ChangeGameState( PreviousGameState );
    }
    // Lock Selection
    if ( BButton )
    {
    }
  }
  
//  if ( DPad == Up && AcceptMenuInput )
//  {
//    CurrentSelectionArrow->SelectionMoveUp();
//    AcceptMenuInput = false;
//  }
//  else if ( DPad == Down && AcceptMenuInput )
//  {
//    CurrentSelectionArrow->SelectionMoveDown();
//    AcceptMenuInput = false;
//  }
//  else if ( ( DPad == Right || DPad == Left ) && AcceptMenuInput )
//  {
//    AcceptMenuInput = false;
//  }

  DrawShipStatusScreen();
  CurrentSelectionArrow->Draw();
}

void DrawShipStatusScreen()
{
  int StatusBarX = 66;
  int TextXPos = 10;
//  switch ( CurrentStatusScreen )
  {
//    case Overview:
      Text::DisplayText(ShipStatus, 42, 0, true);
      Text::DisplayText(TCrew, TextXPos, 8, true);
      Text::DisplayText(THull, TextXPos, 15, true);
      Text::DisplayText(TWeapons, TextXPos, 22, true);
      Text::DisplayText(TShields, TextXPos, 29, true);
      Text::DisplayText(TEngines, TextXPos, 36, true);
      Text::DisplayText(TFuel, TextXPos, 43, true);
//      Text::DisplayText(TGoods, 72, 8, true);

      if ( RepairTarget != NoTarget && StatusBlinkTime > 30)
      {
        arduboy.fillRect( TextXPos, 8 + 7 * RepairTarget, 40, 6, 0 );
      }
      else if (StatusBlinkTime < 0)
        StatusBlinkTime = 60;
      StatusBlinkTime -= 1;
      
  
//    DrawStatusBar( int x, int y, int length, int height, float fill )
      // 27,600 -> 27,322 ...wow  Just removing the array reference with the arithmetic offset  "OverviewLocation[0].y + 2"
      Text::DisplayText( PlayerShip->crewCharArray, 66, 8, false);
      Text::DisplayText( "/", 80, 8, false );
      Text::DisplayText( PlayerShip->maxCrewCharArray, 86, 8, false );
//      DrawStatusBar( StatusBarX, 15, PlayerShip->Max_Hull, 4, (float) PlayerShip->HP_Hull / PlayerShip->Max_Hull );
      DrawStatusBar( StatusBarX, 15, PlayerShip->Max_Hull, 4, 100 * PlayerShip->HP_Hull / PlayerShip->Max_Hull );
      DrawStatusBar( StatusBarX, 22, PlayerShip->Max_Weapons, 4, 100 * PlayerShip->HP_Weapons / PlayerShip->Max_Weapons );
      DrawStatusBar( StatusBarX, 29, PlayerShip->Max_Shields, 4, 100 * PlayerShip->HP_Shields / PlayerShip->Max_Shields );
      DrawStatusBar( StatusBarX, 36, PlayerShip->Max_Engine, 4, 100 * PlayerShip->HP_Engine / PlayerShip->Max_Engine );
//      DrawStatusBar( StatusBarX, 43, PlayerShip->Max_Fuel, 4, (float) PlayerShip->Fuel / PlayerShip->Max_Fuel );
      Text::DisplayText( PlayerShip->fuelCharArray, StatusBarX, 43, false );
//      Text::DisplayText( PlayerShip->goodsCharArray, 114, 8, false);

      if ( PreviousGameState == Map )
      {
        if ( CurrentSelectionArrow->Position == 0 )
        {
          Text::DisplayText( StatusHelp2, 0, 50, true );
          if (BButton)  // Triage
          {
             RepairTarget = static_cast<SystemTarget>( CurrentSelectionArrow->Position );
          }
        }
        if ( CurrentSelectionArrow->Position > 0 && CurrentSelectionArrow->Position <= 4 )
        {
          Text::DisplayText( StatusHelp, 0, 50, true );
          if (BButton)  // Repair
          {
            RepairTarget = static_cast<SystemTarget>( CurrentSelectionArrow->Position );
          }
        }
        else if ( CurrentSelectionArrow->Position == 5 )
        {
          Text::DisplayText( StatusHelp4, 0, 50, true );
        }
      }
      else if ( PreviousGameState == Encounter )
      {
        if ( CurrentSelectionArrow->Position > 0 && CurrentSelectionArrow->Position <= 4 )
        {
          Text::DisplayText( SpendEmergencyRepairs, 0, 50, true );
          if (BButton && BattleRepairs > 0)  // Repair
          {
            RepairTarget = static_cast<SystemTarget>( CurrentSelectionArrow->Position );
            int randomRepairs = random(2, 6);
            while ( randomRepairs > 0 )
            {
              PlayerShip->RepairSystem();
              randomRepairs--;
            }
            BattleRepairs--;
          }
        }

        // Bubbles
        if ( BattleRepairsMax > 2 )
          arduboy.drawBitmap(106, 58, Bubble_Empty_8_8, 8, 8, 1);
        if ( BattleRepairsMax > 1 )
          arduboy.drawBitmap(94, 58, Bubble_Empty_8_8, 8, 8, 1);
        if ( BattleRepairsMax > 0 )
          arduboy.drawBitmap(82, 58, Bubble_Empty_8_8, 8, 8, 1);
        
        Text::DisplayText( EmergencyRepairs, 0, 58, true );
        
        if ( BattleRepairs > 2 )
          arduboy.drawBitmap(106, 58, Bubble_8_8, 8, 8, 1);
        if ( BattleRepairs > 1 )
          arduboy.drawBitmap(94, 58, Bubble_8_8, 8, 8, 1);
        if ( BattleRepairs > 0 )
          arduboy.drawBitmap(82, 58, Bubble_8_8, 8, 8, 1);
      }
//    break;
  }
}

void SetupEncounter()
{
  // For if we visit the status screen from within combat, we don't want to reset up the encounter
  if ( PreviousGameState == Status ) return;
  
  SequenceStage = 1;
  RunningAway = false;
  
  if ( LatestPlanetEncountered->Alignment < 0 )
  {
    Comm_A_1 = (char *) Negative_Response[ random(0, 3) ];
  }
  else if ( LatestPlanetEncountered->Alignment == 0 )
  {
    Comm_A_1 = (char *) Neutral_Response[ random(0, 5) ];
  }
  else if ( LatestPlanetEncountered->Alignment > 0 )
  {
    Comm_A_1 = (char *) Positive_Response[ random(0, 3) ];
  }

  Cmd_Atk = (char *) Engage_Combat[random(0, 3)];
  Cmd_Repair = (char *) Repair_Combat[random(0, 3)];
  Cmd_Flee = (char *) Flee_Combat[random(0, 3)];

  PlayerShip->CalculateBattleRepairs();
}

void EncouterUpdate()
{
  int randomInt;
  int index = 0;
  int nextSequence = SequenceStage;
  
  switch ( SequenceStage )
  {
    case 1: // Intro
      if ( TextManager->DisplayTextOverTime( Comm_A_1, 0, 0 ) && BButton )
      {
          TextManager->NewDisplayTextOverTime();
        
          if ( LatestPlanetEncountered->Alignment < 0 )
            nextSequence = 3;
          else if ( LatestPlanetEncountered->Alignment == 0 )
            nextSequence = 7;
          else
            nextSequence = 8;
      }
      break;
    
//    case 2: // Intro
//      TextManager->DisplayTextClear( Comm_A_1, 0, 0, true, true );
//      if ( TextManager->DisplayTextOverTime( Comm_B_1, 0, 55 ) || ( AButton || BButton ) )
//      {
//        SequenceStage++;
//      }
//    break;
    
    case 3: // Combat Choice
      DrawCombatScreen( true );
      
      if ( BButton )
      {
        nextSequence = 4 + CombatSelectionArrow->Position;
      }
    break;
        
    case 4: // Combat Calc
    {
      DrawCombatScreen( false );
      index = 0;

      CombatPlanetDamage = PlayerShip->HP_Weapons + (int) ( random( - (PlayerShip->HP_Weapons * 0.2f ), ( PlayerShip->HP_Weapons * 0.2f ) ) );
      CombatPlanetDamage = max( CombatPlanetDamage, 1 );
      CombatShipShieldDamage = LatestPlanetEncountered->Attack;// + (int) ( random( - (LatestPlanetEncountered->Attack * 0.1f), ( LatestPlanetEncountered->Attack * 0.1f ) ) );
      CombatShipShieldDamage = max( CombatShipShieldDamage, 1 );
      
      CombatPlanetDef = LatestPlanetEncountered->Defense - CombatPlanetDamage;
      
      if ( CombatPlanetDef <= 0 && RunningAway == false )
      {
        Combat_Result = (char *) Victory[ random( 0, 2 ) ];
        TextManager->NewDisplayTextOverTime();
        nextSequence = 15;
        break;
      }
      else
      {
        if (!RunningAway)
        {
          index += Text::CopyIntoBuffer(CombatTakeDamage4, index, 12);
          Text::ConvertIntToChar(CombatPlanetDamage, typeBuffer, index);
          index++;
          int CombatPlanetDamageCopy = CombatPlanetDamage;
          for(; CombatPlanetDamageCopy /= 10; index++);
  
          index += Text::CopyIntoBuffer(CombatTakeDamage2, index, 9);
        }
      }
      
      if ( CombatShipShieldDamage >= PlayerShip->HP_Shields )
      {
        CombatShipDamage = CombatShipShieldDamage - PlayerShip->HP_Shields;

        // Shields Down
        index += Text::CopyIntoBuffer(ShieldsDown, index, 18);

        // Ship Damage
        index += Text::CopyIntoBuffer(CombatTakeDamage1, index, 12);
        Text::ConvertIntToChar(CombatShipDamage, typeBuffer, index);

        index++;
        int CombatShipDamageCopy = CombatShipDamage;
        for(; CombatShipDamageCopy /= 10; index++);

        index += Text::CopyIntoBuffer(CombatTakeDamage2, index, 9);
        SystemDamaged = PlayerShip->TakeDamage( 0 );
        switch ( SystemDamaged )
        {
          case Crew:
            index += Text::CopyIntoBuffer(DamageReportCrew, index, 27);
          break;
          case Hull:
            index += Text::CopyIntoBuffer(DamageReportHull, index, 21);
          break;
          case Weapons:
            index += Text::CopyIntoBuffer(DamageReportWeapons, index, 29);
          break;
          case Shields:
            index += Text::CopyIntoBuffer(DamageReportShields, index, 22);
          break;
          case Engines:
            index += Text::CopyIntoBuffer(DamageReportEngines, index, 24);
          break;
        }
        typeBuffer[index] = '\0';
      }
      else
      {
        index += Text::CopyIntoBuffer(CombatTakeDamage3, index, 17);;
        
        Text::ConvertIntToChar(CombatShipShieldDamage, typeBuffer, index);

        // Optimization 27,288 -> 27,264
        index++;
        int CombatShipDamageCopy = CombatShipDamage;
        for(; CombatShipDamageCopy /= 10; index++);

        index += Text::CopyIntoBuffer(CombatTakeDamage2, index, 9);
        index += Text::CopyIntoBuffer(ShieldsHolding, index, 18);
        typeBuffer[index] = '\0';    
      }
      MenuWaitTime = 15;
      nextSequence = 9;
    }
    break;
    
    case 5: // Status Screen
      nextSequence = 3;
      ChangeGameState(Status);
    break;

    case 6: // Run Away
      nextSequence = 4;
      RunningAway = true;
    break;

    case 7: // Neutral
      LatestPlanetEncountered->Contacted = true;
      CanHail = false;
      ChangeGameState( Map );
    break;

    case 8: // Positive
      LatestPlanetEncountered->Contacted = true;
      GenerateReward( LatestPlanetEncountered->Prize );
      TextManager->NewDisplayTextOverTime();
      nextSequence = 17;
    break;
    
    case 9: // Print Damage
      DrawCombatScreen( false );
      Text::DisplayTextClear( typeBuffer, 0, 36, false, true );
      MenuWaitTime -= 1;
      if ( MenuWaitTime <= 0 && BButton )
      {
        LatestPlanetEncountered->Defense = CombatPlanetDef;
        PlayerShip->HP_Shields = max( PlayerShip->HP_Shields - CombatShipShieldDamage, 0 );
        if ( PlayerShip->HP_Shields <= 0 )
          PlayerShip->TakeDamage(CombatShipDamage, SystemDamaged);
        
        if ( PlayerShip->HP_Hull <= 0 )
        {
          nextSequence = 18;
          break;
        }
        else if ( !RunningAway )
          nextSequence = 3; // Back to combat
        else
          nextSequence = 16;
      }
    break;
    
    case 15: // Success
      
      if ( TextManager->DisplayTextOverTime( Combat_Result, 0, 30 ) )
      {
        if ( BButton )
        {
          GenerateReward( LatestPlanetEncountered->Prize );
          LatestPlanetEncountered->Contacted = true;
          TextManager->NewDisplayTextOverTime();
          nextSequence = 17;
        }
      }
    break;
    case 16: // Ran Away
      ChangeGameState( Map );
    break;
    case 17: // Result
      Text::DisplayTextClear( typeBuffer, 0, 30, false, true );
      if ( BButton )
      {
        CanHail = false;
        ChangeGameState( Map );
      }
    break;
    case 18: // Game Over
      ChangeGameState( GameOver );
      return;
    break;
  }
  SequenceStage = nextSequence;
}

void DrawCombatScreen( bool DrawCommands )
{
  // Left Side
  arduboy.fillRect( 0, 16, 35, 24, 0 );
  arduboy.drawRect(0, 18, 35, 21, 1);
  // static int DisplayText( char* text, int x, int y, bool fromProgMem );
  //static int DisplayTextClear( char* text, int x, int y, bool fromProgMem, bool withBorder );
  
  Text::DisplayText(CombatMenu_Atk, 2, 20, true);
  Text::ConvertIntToChar( PlayerShip->HP_Weapons );
  Text::DisplayText( buffer, 22, 20, false );
  
  Text::DisplayText(CombatMenu_Shld, 2, 26, true);
  Text::ConvertIntToChar( PlayerShip->HP_Shields );
  Text::DisplayText( buffer, 22, 26, false );
  
  Text::DisplayText(CombatMenu_Hull, 2, 32, true);
  Text::ConvertIntToChar( PlayerShip->HP_Hull );
  Text::DisplayText( buffer, 22, 32, false);

  Text::DisplayTextClear( CombatMenu_Player, 2, 8, true, false );

  // Right Side
  Text::DisplayText( CombatMenu_Enemy, 100, 10, true );
  arduboy.fillRect( 96, 18, 32, 22, 0 );
  arduboy.drawRect( 96, 18, 32, 21, 1);
  
  Text::DisplayText(CombatMenu_Atk, 98, 20, true);
  Text::ConvertIntToChar( LatestPlanetEncountered->Attack );
  Text::DisplayText( buffer, 114, 20, false );
  
  Text::DisplayText(CombatMenu_Def, 98, 26, true);
  Text::ConvertIntToChar( LatestPlanetEncountered->Defense );
  Text::DisplayText( buffer, 114, 26, false );

  // Orders
  if ( DrawCommands )
  {
    // Selection Arrow
    if ( DPad == None && AcceptMenuInput == false )
      AcceptMenuInput = true;
  
    if ( DPad == Up && AcceptMenuInput )
    {
      CombatSelectionArrow->SelectionMoveUp();
      AcceptMenuInput = false;
    }
    else if ( DPad == Down && AcceptMenuInput )
    {
      CombatSelectionArrow->SelectionMoveDown();
      AcceptMenuInput = false;
    }
  
    Text::DisplayTextClear( Cmd_Atk, 8, 42, true, false );
    Text::DisplayTextClear( Cmd_Repair, 8, 49, true, false );
    Text::DisplayTextClear( Cmd_Flee, 8, 56, true, false );
  
    CombatSelectionArrow->Draw();
    arduboy.drawRect( 0, 42, 124, 28, 1 );
  }
}

void GenerateReward( Loot reward )
{
  int index = 0;
  int upgradeAmount = 0;

  // Display initial text
  if (reward == 0)
  {
//    index = 25;
    index = Text::CopyIntoBuffer( DiscoveredNothing, 0, 37);
  }
  else if (reward > 4)
  {
//    index = 22;
    index = Text::CopyIntoBuffer( DiscoveredGood, 0, 22 );
  }
  else
  {
//    index = 35;
    index = Text::CopyIntoBuffer( DiscoveredUpgrade, 0, 35 );
  }

  upgradeAmount = PlayerShip->Upgrade(reward);
  
  switch (reward)
  {
    case NoLoot:
    
    break;
    case LootHull:
      index += Text::CopyIntoBuffer( THull, index, 4 );
    break;
    case LootWeapons:
      index += Text::CopyIntoBuffer( TWeapons, index, 7 );
    break;
    case LootShields:
      index += Text::CopyIntoBuffer( TShields, index, 7 );
    break;
    case LootEngines:
      index += Text::CopyIntoBuffer( TEngines, index, 7 );
    break;
    case LootCrew:
      index +=Text::CopyIntoBuffer( CapturedCrew, index, 13 );
    break;
    case LootFuel:
      index += Text::CopyIntoBuffer( TFuel, index, 7 );
    break;
//    case LootGoods:
//      
//      index += Text::CopyIntoBuffer( TGoods, index, 4 );
//    break;
  }
  typeBuffer[index] = '\0';
}


//static void DrawStatusBar( int x, int y, int length, int height, float fill )  // To save about 100 bytes
static void DrawStatusBar( int x, int y, int length, int height, int fill )
{
  length *= 2;
//  int fillTo = x + ceil( fill * length );
    int fillTo = x + floor( fill * length / 100.0f );
//  int fillTo = x + floor( length / fill );
  for ( int i(0); i < height; i++ )
  {
    int xOffset = abs( height - ( ceil( height / 2.0f ) + i ) );
    int startX = x + xOffset;
    int startY = y + i;
    int endX = startX + length - ( 2 * xOffset );
    if ( i == 0 || i == (height - 1) )
      arduboy.drawLine( startX, startY, endX, startY, 1 );
    else
    {
      arduboy.drawPixel( startX, startY, 1 );
      arduboy.drawPixel( startX + length - ( 2 * xOffset ), startY, 1 );
    }

    // Draw Fill
    if ( i != 0 && i != (height - 1) )
    {
      arduboy.drawLine( startX, startY, fillTo , startY, 1 );
    }
  }
}

void ResetPlayer()
{
//  delete( PlayerShip );
//  PlayerShip = new Ship();
  Ship::SetupShip( PlayerShip );
  TimeUntilNextRepair = RepairTime;
}

void NextSector()
{
  CurrentSector++;
  
  if ( CurrentSector >= 7 )
  {
    ChangeGameState( WinGame );
    return;
  }
  LatestPlanetEncountered = NULL;
  CanHail = false;
  
//  DeleteMap();
  NewMap();
  PlayerShip->MapPosition->x = 0;
  PlayerShip->MapPosition->y = 0;
  PlayerShip->ShipRotation = 90;
//  GenerateRandomEncounter();
}

void WinGameLoop()
{
  bool done = false;
  
  if ( SequenceStage < 4 )
  {
//    _StarField->Draw();
    arduboy.drawBitmap( 112, 0, PlanetHome_16_64, 16, 64, 1 );
    arduboy.drawBitmap( 56, 25, PlayerBitMaps[2], 16, 16, 1 );

    ClockUpdate( false );
  }
  
  if ( SequenceStage == 1 )
  {
    done = TextManager->DisplayTextOverTime( GameWin1, 0, 2 ) && BButton;
  }
  else if ( SequenceStage == 2 )
  {
    done = TextManager->DisplayTextOverTime( GameWin2, 0, 2 );
  }
  else if ( SequenceStage == 3 )
  {
    done = TextManager->DisplayTextOverTime( GameWin3, 0, 2 );
  }
  else if ( SequenceStage == 4 )
  {
    done = TextManager->DisplayTextOverTime( GameWin4, 0, 2 );
  }
  if ( done && BButton )
  {
    TextManager->NewDisplayTextOverTime();
    SequenceStage++;
    if ( SequenceStage == 5 )
      ChangeGameState( Reset );
  }
}

void GameOverLoop( int ending )
{
  bool done = false;
  
  if ( ending == 1 )
  {
    if ( SequenceStage == 1 )
    {
      done = TextManager->DisplayTextOverTime( Defeat_A, 0, 32 );
    }
    else if ( SequenceStage == 2 )
    {
      done = TextManager->DisplayTextOverTime( Defeat_B, 0, 32 );
    }
    else if ( SequenceStage == 3 )
    {
      done = TextManager->DisplayTextOverTime( Defeat_C, 0, 32 );
    }
    else if ( SequenceStage == 4 )
      ChangeGameState( Reset );
    
  }
  else if ( ending == 2 )
  {
    if ( SequenceStage == 1 )
    {
      done = TextManager->DisplayTextOverTime( Defeat_D, 0, 20 );
    }
    else if ( SequenceStage == 2 )
    {
      done = TextManager->DisplayTextOverTime( Defeat_E, 0, 20 );
    }
    else if ( SequenceStage == 3 )
      ChangeGameState( Reset );
  }
  if ( done && BButton )
  {
    TextManager->NewDisplayTextOverTime();
    SequenceStage++;
  }
}

//void GenerateRandomEncounter()
//{
//  TimeToNextRandomEncounter = random( 10, 20 );
//  ShipEncountered->Attack = 1;
//  ShipEncountered->Defense = 1;
//  ShipEncountered->Prize = static_cast<Loot>( rand() % 7 );
//}

void SetupSectorReachedText()
{
  StatusUpdateTime = 240;
//  int index = 0;
  StatusUpdateAvailable = true;
  StatusUpdateFromProgMem = false;
  Text::CopyIntoBuffer( SectorReachedA, 0, 16 );
//  index = 15;
  Text::ConvertIntToChar( 60 + random( 1, 10 )  - ( CurrentSector * 10 ), typeBuffer, 16 );
//  index = 17;
  typeBuffer[18] = '\0';
  Status_Update = typeBuffer;
}

void ClockUpdate( bool runnning )
{
  // Slow it down because it was too hard?
  if ( runnning ) seconds -= ( DeltaTime * 1.0f );
  if ( seconds < 0 ) 
  {
    if ( minutes == 0 )
    {
      // Game Over
      ChangeGameState( TimeUp );
    }
    
    minutes -= 1;
    seconds = 59.99;
  }

  Text::ConvertIntToChar( minutes, clockBuffer, 0 );
  clockBuffer[1] =  ':';
  if ( seconds < 10 )
  {
    clockBuffer[2] = '0';
    Text::ConvertIntToChar( seconds, clockBuffer, 3 );
  }
  else
    Text::ConvertIntToChar( seconds, clockBuffer, 2 );
  clockBuffer[4] = '\0';
  Text::DisplayText( clockBuffer, 100, 52, false );
}

/*
 * To Do
 * 
 *  
 * 
 *  
/*
 * Notes:
 * 
 * Random encounters
 * Random space debris
 * Not all Planets are encounters
 * Crew for emergency repairs
 * 
 *  Note: It would be cool to render wreckage so you can see where you've been
 */
