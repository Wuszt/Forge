#include "Fpch.h"
#include "Scene.h"
#include "../Core/Serializer.h"
#include "ObjectsManager.h"
#include "../Core/Streams.h"

forge::Scene::~Scene()
{
	for ( forge::ObjectID objectID : m_objects )
	{
		m_objectsManager.RequestDestructingObject( objectID );
	}
}

void forge::Scene::Serialize( forge::Stream& stream ) const
{
	const Uint32 objectsAmount = static_cast< Uint32 >( m_objects.size() );
	forge::Serializer serializer( stream );
	serializer.Serialize( objectsAmount );

	for ( forge::ObjectID objectID : m_objects )
	{
		auto* object = m_objectsManager.GetObject( objectID );
		serializer.Serialize( object->GetType().GetID() );
	}

	for ( forge::ObjectID objectID : m_objects )
	{
		auto* object = m_objectsManager.GetObject( objectID );
		serializer.Serialize( *object );
	}
}

void forge::Scene::Deserialize( forge::Stream& stream )
{
	std::shared_ptr< forge::MemoryStream > memoryStream = std::make_shared< forge::MemoryStream >( stream.GetSize() );
	memoryStream->AppendUninitializedData( stream.GetSize() );
	memoryStream->SetPos( 0u );
	stream.Read( memoryStream->GetData(), stream.GetSize() );

	forge::Deserializer deserializer( *memoryStream );
	const Uint32 objectsAmount = deserializer.Deserialize< Uint32 >();

	for ( Uint32 i = 0; i < objectsAmount; ++i )
	{
		const rtti::ID objectTypeID = deserializer.Deserialize< rtti::ID >();
		if ( const auto* type = rtti::Get().FindType( objectTypeID ) )
		{
			FORGE_ASSERT( type->InheritsFromOrIsA< forge::Object >() );
			m_objectsManager.RequestCreatingObject( static_cast< const forge::Object::Type& >( *type ), { .m_preInitFunc = [ memoryStream ]( forge::Object& obj )
				{
					forge::Deserializer deserializer( *memoryStream );
					obj.Deserialize( deserializer );
				} } );
		}
	}
}
