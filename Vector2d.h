#ifndef VECTOR2D_H
#define VECTOR2D_H

class Vector2d
{
  public:
  Vector2d();
  Vector2d(int PosX, int PosY);
  ~Vector2d();

  float Magnitude();
  float MagnitudeSquared();
  void Normalize();

  Vector2d operator*(const Vector2d &rhs);
  Vector2d operator*(const float rhs);
  Vector2d operator*=(const Vector2d &rhs);
  Vector2d operator+(const Vector2d &rhs);
  Vector2d operator+=(const Vector2d &rhs);
  Vector2d operator-(const Vector2d &rhs);
  Vector2d operator-=(const Vector2d &rhs);
  float Dot( Vector2d b );
  Vector2d Rotate( float angle );
  
  float x;
  float y;
};

static float Deg2Rad( float Deg );

#endif
