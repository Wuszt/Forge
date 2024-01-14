#include "Fpch.h"
#include "PhysicsUserData.h"

physics::UserData::UserData( void* rawUserData )
{
	static_assert( sizeof( *this ) == sizeof( rawUserData ) );
	std::memcpy( this, &rawUserData, sizeof( *this ) );
}

void* physics::UserData::GetAsPointerSizeType() const
{
	void* result = nullptr;
	static_assert( sizeof( *this ) == sizeof( result ) );
	std::memcpy( &result, this, sizeof( *this ) );
	return result;
}