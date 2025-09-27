#include "Fpch.h"
#include "Utils.h"
#include "Streams.h"
#include "Serializer.h"

RTTI_IMPLEMENT_TYPE( forge::UniqueRawPtr,
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);

RTTI_IMPLEMENT_TYPE( forge::InstanceUniquePtr,
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);

forge::InstanceUniquePtr::InstanceUniquePtr( InstanceUniquePtr&& other )
	: m_type( other.m_type )
	, m_memory( other.m_memory )
{
	other.m_type = nullptr;
	other.m_memory = nullptr;
}

forge::InstanceUniquePtr& forge::InstanceUniquePtr::operator=( InstanceUniquePtr&& other )
{
	m_type = other.m_type;
	m_memory = other.m_memory;

	other.m_type = nullptr;
	other.m_memory = nullptr;

	return *this;
}

void forge::InstanceUniquePtr::Serialize( forge::Serializer& serializer ) const
{
	serializer.Serialize( m_type->GetID() );
	serializer.Serialize( m_memory, *m_type );
}

void forge::InstanceUniquePtr::Deserialize( forge::Deserializer& deserializer )
{
	rtti::ID typeId;
	deserializer.Deserialize( typeId );
	deserializer.Deserialize( m_memory, *m_type );
}

void forge::UniqueRawPtr::Serialize( forge::Serializer& serializer ) const
{
	serializer.Serialize( m_size );
	serializer.Serialize( m_data, m_size );
}

void forge::UniqueRawPtr::Deserialize( forge::Deserializer& deserializer )
{
	Uint64 size = 0u;
	deserializer >> size;
	*this = forge::UniqueRawPtr( size );
	deserializer.Deserialize( m_data, size );
}
