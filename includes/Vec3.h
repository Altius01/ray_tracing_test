#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <ostream>

class Vec3
{
private:
    float x_;
    float y_;
    float z_;

public:
    Vec3() {}
    Vec3(float x, float y, float z)
    {
        this->x_ = x;
        this->y_ = y;
        this->z_ = z;
    }

    inline float x() const { return this->x_; }
    inline float y() const { return this->y_; }
    inline float z() const { return this->z_; }

    inline void x(float val) { this->x_ = val; }
    inline void y(float val) { this->y_ = val; }
    inline void z(float val) { this->z_ = val; }

    inline float length() const
    {
        return std::hypot(this->x(), this->y(), this->z());
    }

    inline float squared_length() const
    {
        return x() * x() + y() * y() + z() * z();
    }
};

class FreeVec3 final : public Vec3
{
    using Vec3::Vec3;

public:
    explicit FreeVec3(const Vec3 &Vec3) : Vec3::Vec3{Vec3} {}

    inline float dot(const Vec3 &other) const
    {
        return this->x() * other.x() + this->y() * other.y() + this->z() * other.z();
    }

    inline FreeVec3 cross(const Vec3 &other) const
    {
        return FreeVec3(this->y() * other.z() - this->z() * other.y(),
                        this->z() * other.x() - this->x() * other.z(),
                        this->x() * other.y() - this->y() * other.x());
    }

    inline FreeVec3 hadamar(const Vec3 &other)
    {
        return FreeVec3(this->x() * other.x(),
                        this->y() * other.y(),
                        this->z() * other.z());
    }

    inline FreeVec3 &operator+=(const FreeVec3 &other)
    {
        this->x(this->x() + other.x());
        this->y(this->y() + other.y());
        this->z(this->z() + other.z());
        return *this;
    }

    inline FreeVec3 &operator-=(const FreeVec3 &other)
    {
        this->x(this->x() - other.x());
        this->y(this->y() - other.y());
        this->z(this->z() - other.z());
        return *this;
    }

    inline FreeVec3 &operator*=(const float scalar)
    {
        this->x(this->x() * scalar);
        this->y(this->y() * scalar);
        this->z(this->z() * scalar);
        return *this;
    }

    inline FreeVec3 &operator/=(const float scalar)
    {
        this->x(this->x() / scalar);
        this->y(this->y() / scalar);
        this->z(this->z() / scalar);
        return *this;
    }
};
inline FreeVec3 operator+(const FreeVec3 &v) { return v; }

inline FreeVec3 operator-(const FreeVec3 &v)
{
    return FreeVec3{-v.x(), -v.y(), -v.z()};
}

inline FreeVec3 operator+(FreeVec3 v1, const FreeVec3 &v2)
{
    return v1 += v2;
}

inline FreeVec3 operator-(FreeVec3 v1, const FreeVec3 &v2)
{
    return v1 -= v2;
}

inline FreeVec3 operator*(FreeVec3 v, const float scalar)
{
    return v *= scalar;
}

inline FreeVec3 operator/(FreeVec3 v, const float scalar)
{
    return v /= scalar;
}

class BoundVec3 final : public Vec3
{
public:
    explicit BoundVec3(const Vec3 &Vec3) : Vec3::Vec3{Vec3} {}

    inline float dot(const Vec3 &other) const
    {
        return this->x() * other.x() + this->y() * other.y() + this->z() * other.z();
    }

    inline BoundVec3 &operator+=(const FreeVec3 &other)
    {
        this->x(this->x() + other.x());
        this->y(this->y() + other.y());
        this->z(this->z() + other.z());
        return *this;
    }

    inline BoundVec3 &operator-=(const FreeVec3 &other)
    {
        return *this += (-other);
    }
};

inline FreeVec3 operator-(const BoundVec3 &v1, const BoundVec3 &v2)
{
    return FreeVec3{v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z()};
}

inline BoundVec3 operator+(BoundVec3 v1, const FreeVec3 &v2)
{
    return v1 += v2;
}

inline BoundVec3 operator-(BoundVec3 v1, const FreeVec3 &v2)
{
    return v1 -= v2;
}

class UnitVec3 final
{
public:
    UnitVec3(float x, float y, float z)
        : UnitVec3{FreeVec3{x, y, z}} {}
    explicit UnitVec3(const Vec3 &vec3) : UnitVec3{FreeVec3{vec3}} {}
    explicit UnitVec3(const FreeVec3 &free_vec3) : inner_{free_vec3 / free_vec3.length()} {}

    inline float x() const { return this->to_free().x(); }
    inline float y() const { return this->to_free().y(); }
    inline float z() const { return this->to_free().z(); }
    inline const FreeVec3 &to_free() const { return inner_; }

private:
    const FreeVec3 inner_;
};

inline FreeVec3 operator*(const UnitVec3 &v, const float scalar)
{
    return v.to_free() * scalar;
}

inline FreeVec3 operator/(const UnitVec3 &v, const float scalar)
{
    return v.to_free() / scalar;
}

class Ray final
{
public:
    Ray(const BoundVec3 &origin, const UnitVec3 &direction)
        : origin_{origin}, direction_{direction} {}

    // Represents the function p(t) = origin + t * direction,
    // where p is a 3-dimensional position, and t is a scalar.
    inline BoundVec3 point_at_parameter(const float t) const
    {
        return this->origin_ + (this->direction_ * t);
    }

    inline BoundVec3 origin() const { return this->origin_; }
    inline UnitVec3 direction() const { return this->direction_; }

private:
    // The origin of the ray.
    const BoundVec3 origin_;
    // The normalized direction of the ray.
    const UnitVec3 direction_;
};

#endif