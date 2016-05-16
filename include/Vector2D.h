//Borrowed from terathon.com

#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

class Vector2D
{
    public:
        float   x;
        float   y;

        Vector2D() {}

        Vector2D(float r, float s)
        {
            x = r;
            y = s;
        }

        Vector2D& Set(float r, float s)
        {
            x = r;
            y = s;
            return (*this);
        }

        float& operator [](long k)
        {
            return ((&x)[k]);
        }

        const float& operator [](long k) const
        {
            return ((&x)[k]);
        }

        Vector2D& operator +=(const Vector2D& v)
        {
            x += v.x;
            y += v.y;
            return (*this);
        }

        Vector2D& operator -=(const Vector2D& v)
        {
            x -= v.x;
            y -= v.y;
            return (*this);
        }

        Vector2D& operator *=(const Vector2D& v)
        {
            x *= v.x;
            y *= v.y;
            return (*this);
        }

        Vector2D& operator /=(const Vector2D& v)
        {
            x /= v.x;
            y /= v.y;
            return (*this);
        }

        Vector2D& operator *=(float t)
        {
            x *= t;
            y *= t;
            return (*this);
        }

        Vector2D& operator /=(float t)
        {
            float f = 1.0F / t;
            x *= f;
            y *= f;
            return (*this);
        }

        Vector2D operator -(void) const
        {
            return (Vector2D(-x, -y));
        }

        Vector2D operator +(const Vector2D& v) const
        {
            return (Vector2D(x + v.x, y + v.y));
        }

        Vector2D operator -(const Vector2D& v) const
        {
            return (Vector2D(x - v.x, y - v.y));
        }

        Vector2D operator *(const Vector2D& v) const
        {
            return (Vector2D(x * v.x, y * v.y));
        }

        Vector2D operator /(const Vector2D& v) const
        {
            return (Vector2D(x / v.x, y / v.y));
        }

        Vector2D operator *(float t) const
        {
            return (Vector2D(x * t, y * t));
        }

        Vector2D operator /(float t) const
        {
            float f = 1.0F / t;
            return (Vector2D(x * f, y * f));
        }

        bool operator ==(const Vector2D& v) const
        {
            return ((x == v.x) && (y == v.y));
        }

        bool operator !=(const Vector2D& v) const
        {
            return ((x != v.x) || (y != v.y));
        }

        Vector2D& Normalize(void)
        {
            return (*this /= sqrtf(x * x + y * y));
        }
};

inline float Magnitude(const Vector2D& v)
{
    return (sqrtf(v.x * v.x + v.y * v.y));
}

inline float InverseMag(const Vector2D& v)
{
    return (1.0F / sqrtf(v.x * v.x + v.y * v.y));
}

inline float SquaredMag(const Vector2D& v)
{
    return (v.x * v.x + v.y * v.y);
}

#endif // VECTOR2D_H
