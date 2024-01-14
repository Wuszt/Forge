#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"
#include "../Editor/EditorInstance.h"


#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/PhysicsComponent.h"
#include "../Renderer/ModelAsset.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/Model.h"
#include "../Physics/PhysicsShape.h"
#include "../Systems/PhysicsSystem.h"

Int32 main()
{
	forge::EditorInstance editorInstance( "Editor" );
	forge::EngineInstance engineInstance( editorInstance );

	for( Uint32 i = 0u; i < 10; ++i )
	{
		engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ &, i ]( forge::Object* obj )
		{
			obj->AddComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >();

			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

			transformComponent->SetWorldPosition( { static_cast< Float >( i * 5 ), 0.0f, 0.0f } );

			auto* physicsComponent = obj->GetComponent< forge::PhysicsStaticComponent >();
			auto modelAsset = engineInstance.GetAssetsManager().GetAsset< renderer::ModelAsset >( "Models\\cube.obj" );

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

			Uint32 index = 0u;
			for ( renderer::Model::Shape& shape : model->GetShapes() )
			{
				physicsComponent->AddShape( physics::PhysicsShape( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), verts, shape.m_indices ) );
				++index;
			}
		} );
	}


	engineInstance.Run();
}