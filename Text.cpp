#include "Text.h"

/*
 * ! = 33
 * 0 = 48
 * A = 65
 * a = 97
*/
 
int tWidth = 3;
int tHeight = 5;
int tPadding = 1;
int FramesPerChar = 2;
int textOffset = 0;

Text* TextManager;

int Text::DisplayText( char* text, int x, int y, bool fromProgMem )
{
  const uint8_t *bitmap = font;
  char* current = text;
  int currentSpacing = 0;
  int currentX = x + textOffset;
  int currentY = y + textOffset;
  int linesDisplayed = 1;
  
  while ( (fromProgMem ? pgm_read_byte( &current[0] ) : current[0]) != '\0' )
  {
    if ( (fromProgMem ? pgm_read_byte( &current[0] ) : current[0]) == '\n' )
    {
      currentX = x + textOffset;
      currentY = currentY + tHeight + tPadding;
      currentSpacing = 0;
      current++;
      linesDisplayed++;
      
      continue;
    }
    
    int character = (int) ( fromProgMem ? pgm_read_byte( &current[0] ) : current[0] );
    int row = 1;
    if ( character >= 97 )
    {
      row = 3;
    }
    else if ( character >= 65 )
    {
      row = 2;
    }
    else if ( character >= 33 )
    {
      row = 1;
    }

    int rowBitSize = 32 * 4;
    rowBitSize = row * rowBitSize;
    int bitMapIndex = rowBitSize + ( ( character % 32 ) * 4 );
    const uint8_t *drawAt = bitmap;
    drawAt += bitMapIndex;
    arduboy.drawBitmap( currentX + currentSpacing, currentY, drawAt, tWidth, tHeight, 1 );
    currentSpacing += (tWidth + tPadding);
    
    current++;
  }

  return linesDisplayed;
}

int Text::DisplayText( char const* text, int x, int y, bool fromProgMem )
{
  return Text::DisplayText( (char *) text, x, y, fromProgMem );
}

int Text::DisplayTextClear( char* text, int x, int y, bool fromProgMem, bool withBorder )
{
  textOffset = 2;
  
  arduboy.fillRect( 0, y, 128, 9, 0 );
  if ( withBorder ) arduboy.drawRect(0, y, 128, 9, 1);
  int linesDisplayed = Text::DisplayText( text, x, y, fromProgMem );

  if ( linesDisplayed > 1 )
  {
    int clearHeight = ( tHeight + tPadding ) * linesDisplayed + 3;
    arduboy.fillRect( 0, y, 128, clearHeight, 0 );
    if ( withBorder ) arduboy.drawRect(0, y, 128, clearHeight, 1);
    Text::DisplayText( text, x, y, fromProgMem );
  }

  textOffset = 0;
  return linesDisplayed;
}

int Text::DisplayTextClear( char const* text, int x, int y, bool fromProgMem, bool withBorder )
{
  return Text::DisplayTextClear( (char *) text, x, y, fromProgMem, withBorder );
}

Text::Text()
{
  TextOverTimeRunning = false;
  FramesToNextChar = 3;
  CharIndex = 0;
}

Text::~Text()
{
  
}

void Text::NewDisplayTextOverTime()
{
  TextOverTimeRunning = true;
  FramesToNextChar = FramesPerChar;
  CharIndex = 0;
}

bool Text::DisplayTextOverTime( char* text, int x, int y )
{
  if ( TextOverTimeRunning )
  {
    ReadTextIntoTypeBuffer( text );
    
    Text::DisplayTextClear( typeBuffer, x, y, false, true );
    
    DeltaFramesToNextChar();
    
    return false;
  }
  else
  {
    Text::DisplayTextClear( text, x, y, true, true );
    return true;
  }
}

bool Text::DisplayTextOverTime( char const* text, int x, int y )
{
  return Text::DisplayTextOverTime( (char *) text, x, y );
}

bool Text::DisplayTextOverTimeClear( char* text, int x, int y )
{
  if ( TextOverTimeRunning )
  {
    ReadTextIntoTypeBuffer( text );

    Text::DisplayTextClear( typeBuffer, x, y, false, false);
    
    DeltaFramesToNextChar();
    
    return false;
  }
  else
  {
    Text::DisplayTextClear( text, x, y, true, false );
    return true;
  }
}

bool Text::DisplayTextOverTimeClear( char const* text, int x, int y )
{
  return Text::DisplayTextOverTimeClear( (char *) text, x, y );
}

void Text::DeltaFramesToNextChar()
{
  FramesToNextChar -= 1;
  if ( FramesToNextChar <= 0 )
  {
    CharIndex++;
    FramesToNextChar = FramesPerChar;
  }
}

// Converts integer into char and puts it into the buffer
void Text::ConvertIntToChar( int integer )
{
  itoa ( integer, buffer, 10 );
}

void Text::ConvertIntToChar( int integer, char* bufferToWrite )
{
  itoa( integer, bufferToWrite, 10 );
}

void Text::ConvertIntToChar( int integer, char* bufferToWrite, int index )
{
  char* bufferIndex = &bufferToWrite[index];
  itoa ( integer, bufferIndex, 10 );
}

int Text::CopyIntoBuffer( const char* Characters, int bufferStartIndex, int charsToCopy )
{
  for ( int i(0); i < charsToCopy; i++ )
  {
    typeBuffer[ i + bufferStartIndex ] = pgm_read_byte( &Characters[i] );
  }
  return charsToCopy;
}

void Text::ReadTextIntoTypeBuffer( char* text )
{
  for ( int i(0); i < CharIndex; i++ )
  {
    if ( pgm_read_byte( &text[i] ) == '\0' )
    {
      TextOverTimeRunning = false;
      return;
    }
    typeBuffer[i] = pgm_read_byte( &text[i] );
  }
  typeBuffer[CharIndex] = '\0';
}

char buffer[64] = { };
char typeBuffer[128] = { };
char clockBuffer[5] = { };
char* Comm_A_1;
char* Comm_B_1;
char* Cmd_Atk;
char* Cmd_Repair;
char* Cmd_Flee;
char* Comm_Result;
char* Combat_Result;
char* Status_Update;

PROGMEM const char TitleScreen[] = "Star Honor";
PROGMEM const char TitleScreen2[] = "By Wenceslao Villanueva 2016";

PROGMEM const char THull[] = "Hull";
PROGMEM const char TWeapons[] = "Weapons";
PROGMEM const char TShields[] = "Shields";
PROGMEM const char TEngines[] = "Engines";
PROGMEM const char TCrew[] = "Red Shirts";
PROGMEM const char TFuel[] = "Crystal";
PROGMEM const char TFood[] = "Food";
PROGMEM const char TGoods[] = "Material";

PROGMEM const char Repaired[] = " repaired";
PROGMEM const char RedShirtsRepaired[] = "All hands back on duty";
PROGMEM const char HullRepaired[] = "Hull Repaired";
PROGMEM const char WeaponsRepaired[] = "Weapon Systems 100%";
PROGMEM const char ShieldsRepaired[] = "Shields restored";
PROGMEM const char EnginesRepaired[] = "Engines at full";

//typedef enum SystemTarget { Crew, Hull, Weapons, Engines, Shields };
// Engines and Shields had to be swapped.  Why, I don't know but I kinda don't care any more?
const char* RepairedText[] { RedShirtsRepaired, HullRepaired, WeaponsRepaired, ShieldsRepaired, EnginesRepaired };

PROGMEM const char ShipStatus[] = "Ship Status";
PROGMEM const char StatusHelp[] = "B Button - Assign Crews";
PROGMEM const char StatusHelp2[] = "B Button - Treat Crew";
PROGMEM const char StatusHelp3[] = "Up + B Button - Warp next sector";
PROGMEM const char StatusHelp4[] = "  3 Crystals needed to Warp";
PROGMEM const char SpendEmergencyRepairs[] = "B - Spend Emergency Repair";
PROGMEM const char EmergencyRepairs[] = "Emergency Repairs:";

PROGMEM const char PrologueText1[] = "USS Arduino Log Update:\n- Most the crew is dead.\n- Sensors are unsalvageable.\n- Engines and Comms functional.\n- Shields...intact\n";
PROGMEM const char PrologueText2[] = "  The Captain is dead. At least\nwe have the cure for the phage.\nWe must get it to homeworld.\nYou're Captain now.\nThe Bridge is yours. Orders?";

PROGMEM const char GameWin1[] = "Captain, we made it.\nDispensing the cure in the\natmosphere";
PROGMEM const char GameWin2[] = "Scans are coming back. The\ncure is working.";
PROGMEM const char GameWin3[] = "Captain! Communication from\nCommand...demanding we power\ndown and prepare to be boarded?";
PROGMEM const char GameWin4[] = "To Be Continued...";

PROGMEM const char Engage_Stronger_A[] = "I'm sure we can talk this out";
PROGMEM const char Engage_Stronger_B[] = "Captain,\nlet us be rational";
PROGMEM const char Engage_Stronger_C[] = "I have lots of powerful friends";
PROGMEM const char Engage_Stronger_D[] = "Prepare the escape pods!";

PROGMEM const char Engage_Equal_A[] = "We'll take your surrender";
PROGMEM const char Engage_Equal_B[] = "You'll regret this";
PROGMEM const char Engage_Equal_C[] = "All hands to battle stations!";
PROGMEM const char Engage_Equal_D[] = "And my pension was so close";

PROGMEM const char Engage_Weaker_A[] = "You will make a great prize";
PROGMEM const char Engage_Weaker_B[] = "I'll take your ship as a trophy";
PROGMEM const char Engage_Weaker_C[] = "Prepare to be destroyed";
PROGMEM const char Engage_Weaker_D[] = "For the Empire!";

//const char* EngageText[] {Engage_Stronger_A, Engage_Stronger_B, Engage_Stronger_C, Engage_Stronger_D,
//                          Engage_Equal_A, Engage_Equal_B, Engage_Equal_C, Engage_Equal_D,
//                          Engage_Weaker_A, Engage_Weaker_B, Engage_Weaker_C, Engage_Weaker_D};
//
//PROGMEM const char Projection_A[] = "Cpt. we can destroy them easily";
//PROGMEM const char Projection_B[] = "Cpt. we are evenly matched";
//PROGMEM const char Projection_C[] = "Cpt. I advise caution...";
//PROGMEM const char Projection_D[] = "Cpt. they are very powerful";
//PROGMEM const char Projection_E[] = "Cpt. attacking is suicide!";
//
//const char* ProjectionText[] { Projection_A, Projection_B, Projection_C, Projection_D, Projection_E };

PROGMEM const char Engage_Combat_A[] = "Open Fire";
PROGMEM const char Engage_Combat_B[] = "Fire all weapons!";
PROGMEM const char Engage_Combat_C[] = "Target and Fire!";

const char* Engage_Combat[] { Engage_Combat_A, Engage_Combat_B, Engage_Combat_C };

PROGMEM const char Repair_Combat_A[] = "Status Report";
PROGMEM const char Repair_Combat_B[] = "System Status";
PROGMEM const char Repair_Combat_C[] = "Situation Report";

const char* Repair_Combat[] { Repair_Combat_A, Repair_Combat_B, Repair_Combat_C };

PROGMEM const char Flee_Combat_A[] = "Ensign, get us out of here";
PROGMEM const char Flee_Combat_B[] = "Shields! Full retreat!";
PROGMEM const char Flee_Combat_C[] = "Full Reverse, go!";

const char* Flee_Combat[] { Flee_Combat_A, Flee_Combat_B, Flee_Combat_C };

PROGMEM const char Result_Positive_A[] = "Enemy vessel destroyed";
PROGMEM const char Result_Positive_B[] = "Destruction of enemy confirmed";

const char* Result_Positive[] { Result_Positive_A, Result_Positive_B };

PROGMEM const char Result_Negative_A[] = "Damage reports coming in";
PROGMEM const char Result_Negative_B[] = "All stations reporting damage";

const char* Result_Negative[] { Result_Negative_A, Result_Negative_B };

PROGMEM const char Hail[] = "B: Establish communication";
PROGMEM const char Yes[] = "Yes";
PROGMEM const char No[] = "No";

PROGMEM const char Negative_Response_A[] = "Surrender or else!";
PROGMEM const char Negative_Response_B[] = "We have you now scum!";
PROGMEM const char Negative_Response_C[] = "For the Empire I will\ndestroy you!";

const char* Negative_Response[] { Negative_Response_A, Negative_Response_B, Negative_Response_C };

PROGMEM const char Neutral_Response_A[] = "It's nice to meet new species";
PROGMEM const char Neutral_Response_B[] = "... *no response*";
PROGMEM const char Neutral_Response_C[] = "We're not buying, leave!";
PROGMEM const char Neutral_Response_D[] = "Greetings Traveller...";

const char* Neutral_Response[] { Neutral_Response_A, Neutral_Response_B, Neutral_Response_C, Neutral_Response_D };

PROGMEM const char Victory_A[] = "Cpt. we are victorious";
PROGMEM const char Victory_B[] = "Cpt. targets neutralized";

const char* Positive_Response[] { Positive_Response_A, Positive_Response_B, Positive_Response_C };
                                          //12345678901234567890123456789012"
PROGMEM const char Positive_Response_A[] = "Stand down, we mean no harm";
PROGMEM const char Positive_Response_B[] = "We want to help. Have this..";
PROGMEM const char Positive_Response_C[] = "Your people need you. Take this";

const char* Victory[] { Victory_A, Victory_B };

PROGMEM const char Defeat_A[] = "Cpt. multiple breaches\nDeploying Escape Pods!";
PROGMEM const char Defeat_B[] = "Cpt. we're losing core\ncontainment! We can't...";
PROGMEM const char Defeat_C[] = "The crew of the USS Arduino\nwere never heard from again";
PROGMEM const char Defeat_D[] = "Cpt, we've recieved a message\nfrom home. They're new orders" ;
PROGMEM const char Defeat_E[] = "...Final orders...\nTo take what's left and run\nThe phage took everyone\nIt's over...";

PROGMEM const char BeamRecovery[] = "We beamed recovery crews down";
PROGMEM const char Upgrade[] = " upgrade";
PROGMEM const char DiscoveredUpgrade[] = "We discovered an upgrade\n for the: ";
PROGMEM const char DiscoveredGood[] = "Recovery crews found:\n";
PROGMEM const char DiscoveredNothing[] = "Sir, nothing here is\nof any use to us";
PROGMEM const char CapturedCrew[] = "captured crew";

PROGMEM const char CombatTurn[] = "Combat Turn: ";
PROGMEM const char CombatTakeDamage1[] = "We've taken ";
PROGMEM const char CombatTakeDamage2[] = " damage!\n";
PROGMEM const char CombatTakeDamage3[] = "Shields absorbed ";
PROGMEM const char CombatTakeDamage4[] = "Weapons did ";
PROGMEM const char ShieldsHolding[] = "Shields holding...";
PROGMEM const char ShieldsDown[] = "Shields are down!\n";
PROGMEM const char DamageReportCrew[] = "Casualty reports coming in!";
PROGMEM const char DamageReportHull[] = "Hull damage reported!";
PROGMEM const char DamageReportWeapons[] = "Weapon systems hit, Atk down!";
PROGMEM const char DamageReportShields[] = "Shield arrays hit Cpt!";
PROGMEM const char DamageReportEngines[] = "Engines are damaged sir!";

PROGMEM const char CombatMenu_Player[] = "Player";
PROGMEM const char CombatMenu_Atk[] = "Atk:";
PROGMEM const char CombatMenu_Shld[] = "Shld:";
PROGMEM const char CombatMenu_Hull[] = "Hull:";
PROGMEM const char CombatMenu_Enemy[] = "Enemy";
PROGMEM const char CombatMenu_Def[] = "Def:";

PROGMEM const char Random_Encounter_1[] = "Enemy dropping out of warp!";
PROGMEM const char Random_Encounter_2[] = "Enemy decloaking off port!\nThey're charging weapons!";
PROGMEM const char Random_Encounter_3[] = "Enemy ship! They found us!";

PROGMEM const char SectorReachedA[] = "Entering Sector ";
PROGMEM const char SectorReachedB[] = " reached";

PROGMEM const char Colon[] = ":";

/*
Char  Dec  Oct  Hex | Char  Dec  Oct  Hex | Char  Dec  Oct  Hex | Char Dec  Oct   Hex
-------------------------------------------------------------------------------------
(nul)   0 0000 0x00 | (sp)   32 0040 0x20 | @      64 0100 0x40 | `      96 0140 0x60
(soh)   1 0001 0x01 | !      33 0041 0x21 | A      65 0101 0x41 | a      97 0141 0x61
(stx)   2 0002 0x02 | "      34 0042 0x22 | B      66 0102 0x42 | b      98 0142 0x62
(etx)   3 0003 0x03 | #      35 0043 0x23 | C      67 0103 0x43 | c      99 0143 0x63
(eot)   4 0004 0x04 | $      36 0044 0x24 | D      68 0104 0x44 | d     100 0144 0x64
(enq)   5 0005 0x05 | %      37 0045 0x25 | E      69 0105 0x45 | e     101 0145 0x65
(ack)   6 0006 0x06 | &      38 0046 0x26 | F      70 0106 0x46 | f     102 0146 0x66
(bel)   7 0007 0x07 | '      39 0047 0x27 | G      71 0107 0x47 | g     103 0147 0x67
(bs)    8 0010 0x08 | (      40 0050 0x28 | H      72 0110 0x48 | h     104 0150 0x68
(ht)    9 0011 0x09 | )      41 0051 0x29 | I      73 0111 0x49 | i     105 0151 0x69
(nl)   10 0012 0x0a | *      42 0052 0x2a | J      74 0112 0x4a | j     106 0152 0x6a
(vt)   11 0013 0x0b | +      43 0053 0x2b | K      75 0113 0x4b | k     107 0153 0x6b
(np)   12 0014 0x0c | ,      44 0054 0x2c | L      76 0114 0x4c | l     108 0154 0x6c
(cr)   13 0015 0x0d | -      45 0055 0x2d | M      77 0115 0x4d | m     109 0155 0x6d
(so)   14 0016 0x0e | .      46 0056 0x2e | N      78 0116 0x4e | n     110 0156 0x6e
(si)   15 0017 0x0f | /      47 0057 0x2f | O      79 0117 0x4f | o     111 0157 0x6f
(dle)  16 0020 0x10 | 0      48 0060 0x30 | P      80 0120 0x50 | p     112 0160 0x70
(dc1)  17 0021 0x11 | 1      49 0061 0x31 | Q      81 0121 0x51 | q     113 0161 0x71
(dc2)  18 0022 0x12 | 2      50 0062 0x32 | R      82 0122 0x52 | r     114 0162 0x72
(dc3)  19 0023 0x13 | 3      51 0063 0x33 | S      83 0123 0x53 | s     115 0163 0x73
(dc4)  20 0024 0x14 | 4      52 0064 0x34 | T      84 0124 0x54 | t     116 0164 0x74
(nak)  21 0025 0x15 | 5      53 0065 0x35 | U      85 0125 0x55 | u     117 0165 0x75
(syn)  22 0026 0x16 | 6      54 0066 0x36 | V      86 0126 0x56 | v     118 0166 0x76
(etb)  23 0027 0x17 | 7      55 0067 0x37 | W      87 0127 0x57 | w     119 0167 0x77
(can)  24 0030 0x18 | 8      56 0070 0x38 | X      88 0130 0x58 | x     120 0170 0x78
(em)   25 0031 0x19 | 9      57 0071 0x39 | Y      89 0131 0x59 | y     121 0171 0x79
(sub)  26 0032 0x1a | :      58 0072 0x3a | Z      90 0132 0x5a | z     122 0172 0x7a
(esc)  27 0033 0x1b | ;      59 0073 0x3b | [      91 0133 0x5b | {     123 0173 0x7b
(fs)   28 0034 0x1c | <      60 0074 0x3c | \      92 0134 0x5c | |     124 0174 0x7c
(gs)   29 0035 0x1d | =      61 0075 0x3d | ]      93 0135 0x5d | }     125 0175 0x7d
(rs)   30 0036 0x1e | >      62 0076 0x3e | ^      94 0136 0x5e | ~     126 0176 0x7e
(us)   31 0037 0x1f | ?      63 0077 0x3f | _      95 0137 0x5f | (del) 127 0177 0x7f
*/
