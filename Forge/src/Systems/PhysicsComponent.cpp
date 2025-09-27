#include "Fpch.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "../Physics/PhysicsShape.h"
#include "PhysicsUserData.h"
#include "../Renderer/ModelAsset.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Renderer/Model.h"
#include "../Core/PropertiesChain.h"

RTTI_IMPLEMENT_TYPE( forge::PhysicsComponent,
	RTTI_REGISTER_PROPERTY( m_modelPath,
		RTTI_ADD_METADATA( "Extensions", "obj,fbx" );
		RTTI_ADD_METADATA( "InDepot" );
		RTTI_ADD_METADATA( "Editable" );
	);
	RTTI_REGISTER_METHOD( OnPropertyChanged );
);

RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticComponent );

RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicComponent );

void forge::PhysicsComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	const forge::ObjectID objectId = GetOwner().GetObjectID();
	const ecs::EntityID entityId = engineInstance.GetObjectsManager().GetOrCreateEntityId( objectId );

	physics::UserData userData{ entityId, objectId };
	GetActor().Initialize( physicsSystem.GetPhysicsProxy(), static_cast< Uint32 >( physics::PhysicsGroupFlags::Default ), GetOwner().GetComponent< forge::TransformComponent >()->GetWorldTransform(), userData.GetAsPointerSizeType() );

	physicsSystem.RegisterActor( GetActor() );

	if ( !m_modelPath.IsEmpty() )
	{
		SetModel( std::move( m_modelPath ) );
	}
}

void forge::PhysicsComponent::OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	physicsSystem.UnregisterActor( GetActor() );
}

void forge::PhysicsComponent::AddShape( physics::PhysicsShape&& shape )
{
	GetActor().AddShape( std::move( shape ) );
}

void forge::PhysicsComponent::SetModel( forge::Path path )
{
	GetActor().RemoveAllShapes();
	m_modelPath = std::move( path );

	auto& engineInstance = GetOwner().GetEngineInstance();
	auto modelAsset = engineInstance.GetAssetsManager().GetAsset< renderer::ModelAsset >( m_modelPath );

	auto model = modelAsset->GetModel();
	const renderer::Vertices& vertices = model->GetVertices();
	std::vector< Vector3 > verts;
	verts.resize( vertices.GetVerticesAmount() );

	FORGE_ASSERT( vertices.GetInputElements().begin()->m_inputType == renderer::InputType::Position );
	const Byte* address = static_cast< const Byte* >( vertices.GetData() );
	for ( Vector3& vec : verts )
	{
		vec = *reinterpret_cast< const Vector3* >( address );
		address += vertices.GetVertexSize();
	}

	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	Uint32 index = 0u;
	for ( renderer::Model::Shape& shape : model->GetShapes() )
	{
		AddShape( physics::PhysicsShape( physicsSystem.GetPhysicsProxy(), verts, shape.m_indices ) );
		++index;
	}

	GetActor().ChangeScale( GetOwner().GetComponent< forge::TransformComponent >()->GetWorldScale() );
}

void forge::PhysicsComponent::SetGroup( physics::PhysicsGroupFlags group )
{
	GetActor().SetGroup( static_cast< Uint32 >( group ) );
}

physics::PhysicsStaticActor& forge::PhysicsStaticComponent::GetActor()
{
	return GetMutableData()->m_actor;
}

physics::PhysicsDynamicActor& forge::PhysicsDynamicComponent::GetActor()
{
	return GetMutableData()->m_actor;
}

void forge::PhysicsDynamicComponent::UpdateDensity( Float density )
{
	m_density = density;
	GetActor().SetDensity( m_density );
}

void forge::PhysicsDynamicComponent::AddShape( physics::PhysicsShape&& shape )
{
	Super::AddShape( std::move( shape ) );
	GetActor().SetDensity( m_density );
}

void forge::PhysicsComponent::OnPropertyChanged( const forge::PropertiesChain& propertiesChain )
{
	if ( std::strcmp( ( *propertiesChain.Get().back() )->GetName(), "m_modelPath" ) == 0 )
	{
		SetModel( m_modelPath );
	}
}