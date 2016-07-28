#ifndef GUI_H
#define GUI_H

#include "Globals.h"
#include "SelectionArrow.h"

extern int _OverviewXPos;
extern int _OverviewYPos;
extern Vector2d** OverviewLocations;
extern Vector2d** CombatLocations;

extern int _StatusBarIndent;

extern SelectionArrow* RepairSelectionArrow;
extern SelectionArrow* CurrentSelectionArrow;
extern SelectionArrow* CombatSelectionArrow;

extern bool AcceptMenuInput;

#endif
