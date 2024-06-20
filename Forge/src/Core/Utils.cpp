#include "Fpch.h"
#include "Utils.h"
#include "Streams.h"
#include "Serializer.h"

RTTI_IMPLEMENT_TYPE( forge::UniqueRawPtr );
RTTI_IMPLEMENT_TYPE( forge::InstanceUniquePtr,
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);

void forge::InstanceUniquePtr::Serialize( forge::Stream& stream ) const
{
	forge::Serializer serializer( stream );
	serializer.Serialize( m_type->GetID() );
	serializer.Serialize( *m_type, m_memory );
}

void forge::InstanceUniquePtr::Deserialize( forge::Stream& stream )
{
	forge::Deserializer deserializer( stream );
	rtti::ID typeId;
	deserializer.Deserialize( typeId );
	deserializer.Deserialize( *m_type, m_memory );
}
