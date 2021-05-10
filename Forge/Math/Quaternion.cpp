#include "Fpch.h"
#include "Quaternion.h"

Quaternion::~Quaternion() = default;

Quaternion Quaternion::operator*( const Quaternion & q ) const
{
    return Quaternion( q.vec * r + vec * q.r + vec.Cross( q.vec ),
                       r * q.r - vec.Dot( q.vec ) );
}
