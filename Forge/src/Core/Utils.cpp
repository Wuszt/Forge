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
	deserializer.Deserialize( m_size );
	deserializer.Deserialize( m_data, m_size );
}
