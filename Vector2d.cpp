#include "Vector2d.h"
#include <Arduboy2.h>

//#define Deg2Rad( Deg ) { Deg * PI / 180 };

Vector2d::Vector2d()
{
  x = 0;
  y = 0;
}

Vector2d::Vector2d( int PosX, int PosY )
{
  x = PosX;
  y = PosY;
}

Vector2d::~Vector2d()
{
}

float Vector2d::Magnitude()
{
  return sqrt( MagnitudeSquared() );
}

float Vector2d::MagnitudeSquared()
{
  return ( x * x + y * y );
}

void Vector2d::Normalize()
{
  float mag = this->Magnitude();
  x = x / mag;
  y = y / mag;
}

Vector2d Vector2d::operator*( const Vector2d &rhs )
{
  Vector2d result;
  result.x = x * rhs.x;
  result.y = y * rhs.y;
  return result;
}

Vector2d Vector2d::operator*( const float rhs )
{
  Vector2d result;
  result.x = x * rhs;
  result.y = y * rhs;
  return result;
}

Vector2d Vector2d::operator*=( const Vector2d &rhs )
{
  x = x * rhs.x;
  y = y * rhs.y;
}

Vector2d Vector2d::operator+( const Vector2d &rhs )
{
  Vector2d result;
  result.x = x + rhs.x;
  result.y = y + rhs.y;
  return result;
}

Vector2d Vector2d::operator+=( const Vector2d &rhs )
{
  x = x + rhs.x;
  y = y + rhs.y;
}

Vector2d Vector2d::operator-( const Vector2d &rhs )
{
  Vector2d result;
  result.x = x - rhs.x;
  result.y = y - rhs.y;
  return result;
}

Vector2d Vector2d::operator-=( const Vector2d &rhs )
{
  x = x - rhs.x;
  y = y - rhs.y;
}

float Vector2d::Dot( Vector2d b )
{
  return ( x * b.x + y * b.y );
}

Vector2d Vector2d::Rotate( float angle )
{
  float radian = Deg2Rad(angle);
  float cs = cos( radian );
  float sn = sin( radian );

  float px = x * cs - y * sn;
  float py = x * sn + y * cs;

  x = px;
  y = py;
}

float Deg2Rad( float Deg )
{
  return ( Deg * PI / 180 );
}

