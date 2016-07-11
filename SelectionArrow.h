#ifndef SELECTIONARROW_H
#define SELECTIONARROW_H

#include "Vector2d.h"

typedef enum { ArrowSelection, BoxSelection } SelectionType;

class SelectionLocation
{
  public:
  Vector2d* Location;
  SelectionLocation* Next;
  SelectionLocation* Previous;

  SelectionLocation();
  SelectionLocation( Vector2d* loc );
  ~SelectionLocation();
};


class SelectionArrow
{
  public:
  int NumberOfPositions;
  int Position;
//  SelectionType Type;
  SelectionLocation* CurrentSelection;
  SelectionLocation* FirstLocation;
  
  SelectionArrow();
  SelectionArrow( Vector2d** array, int size );
  ~SelectionArrow();

  void Draw();

  void SelectionMoveUp();
  void SelectionMoveDown();

  void Enable( bool e );
//  void SetBoxType();
//  void SetArrowType();
  
//  private:
  bool _Enabled;
};

#endif
