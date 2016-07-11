#ifndef STARFIELD_H
#define STARFIELD_H

#include "Globals.h"
#include "Vector2d.h"

struct Star
{
  float xPosition;
  float yPosition;
  int velocity;
};

class StarField
{
  public:
  StarField();
  void Draw();
  void Move( Vector2d v );
  void BoundsCheck(Star* star);

//  float MinSpeed = 1.0f;
//  float MaxSpeed = 25.0f;

  private:
  Star* _Stars;
};

#endif
