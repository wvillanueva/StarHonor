#include "SelectionArrow.h"
#include "Globals.h"
#include "GUI.h"

SelectionLocation::SelectionLocation() {}

SelectionLocation::SelectionLocation( Vector2d* loc )
{
  Location = loc;
}

SelectionLocation::~SelectionLocation()
{
  delete( Location );
}

SelectionArrow::SelectionArrow() {}

SelectionArrow::SelectionArrow( Vector2d** array, int size )
{
  NumberOfPositions = size;
  Position = 0;
//  Type = ArrowSelection;
  
  SelectionLocation* tmp;
  for ( int i(0); i < size; i ++ )
  {
    if ( i == 0 )
    {
      CurrentSelection = new SelectionLocation( array[i] );
      CurrentSelection->Next = CurrentSelection->Previous =  CurrentSelection;
      FirstLocation = CurrentSelection;
      tmp = CurrentSelection;
    }
    else
    {
      SelectionLocation* newLocation = new SelectionLocation( array[i] );
      
      newLocation->Previous = tmp;
      newLocation->Next = tmp->Next;
      
      tmp->Next = newLocation;
      tmp = newLocation;
    }

    if ( i == (size - 1) )
    {
      FirstLocation->Previous = tmp;
    }
  }
}

SelectionArrow::~SelectionArrow()
{
  
}

void SelectionArrow::Draw()
{
//  if ( Type == ArrowSelection )
    arduboy.drawBitmap( CurrentSelection->Location->x, CurrentSelection->Location->y, SelectionArrow_8_8, 8, 8, 1);
//  else if ( Type == BoxSelection )
//    arduboy.drawRect( CurrentSelection->Location->x, CurrentSelection->Location->y, 29, 9, 1 );
}

void SelectionArrow::SelectionMoveUp()
{
  CurrentSelection = CurrentSelection->Previous;
  Position--;
  Position < 0 ? Position = (NumberOfPositions - 1) : Position; 
}

void SelectionArrow::SelectionMoveDown()
{
  CurrentSelection = CurrentSelection->Next;
  Position++;
  Position %= NumberOfPositions;
}

void SelectionArrow::Enable( bool e )
{
  _Enabled = e;
}

//void SelectionArrow::SetBoxType()
//{
//  this->Type = BoxSelection;
//}
//
//void SelectionArrow::SetArrowType()
//{
//  this->Type = ArrowSelection;
//}

