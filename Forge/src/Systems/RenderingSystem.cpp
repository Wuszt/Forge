#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"
#include "../Renderer/IRenderer.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) ) );
	m_drawToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) ) );
	m_presentToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) ) );

	m_vertexShader = m_renderer->GetVertexShader( "Effects.fx" );
	m_pixelShader = m_renderer->GetPixelShader( "Effects.fx" );

	m_vertexShader->Set();
	m_pixelShader->Set();

	renderer::Vertices< renderer::IVertex< renderer::InputPosition, renderer::InputColor > > vertices;

	vertices.m_vertices =
	{
		{ renderer::InputPosition( -1.0f, -1.0f, -1.0f ), renderer::InputColor( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ renderer::InputPosition( 1.0f,	-1.0f, -1.0f ), renderer::InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ renderer::InputPosition( 1.0f, 1.0f, -1.0f ), renderer::InputColor( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ renderer::InputPosition( -1.0f, 1.0f, -1.0f ), renderer::InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },

		{ renderer::InputPosition( -1.0f, -1.0f, 1.0f ), renderer::InputColor( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ renderer::InputPosition( 1.0f,	-1.0f, 1.0f ), renderer::InputColor( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ renderer::InputPosition( 1.0f, 1.0f, 1.0f ), renderer::InputColor( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ renderer::InputPosition( -1.0f, 1.0f, 1.0f ), renderer::InputColor( 0.3f, 0.7f, 0.6f, 1.0f ) },
	};

	m_vertexBuffer = m_renderer->CreateVertexBuffer( vertices );
	m_vertexBuffer->Set();

	m_inputLayout = m_renderer->CreateInputLayout( *m_vertexShader, *m_vertexBuffer );
	m_inputLayout->Set();

	Uint32 indices[ 6 * 6 ] =
	{
		0, 3, 1, 3, 2, 1,
		1, 2, 5, 2, 6, 5,
		5, 6, 4, 6, 7, 4,
		4, 7, 0, 7, 3, 0,
		3, 7, 2, 7, 6, 2,
		4, 0, 5, 0, 1, 5
	};

	m_indexBuffer = m_renderer->CreateIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
	m_indexBuffer->Set( 0 );

	m_buffer = m_renderer->GetConstantBuffer< cbPerObject >();
}

void systems::RenderingSystem::OnBeforeDraw()
{
	m_renderer->BeginScene();
	m_renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
	if( m_renderer->GetDepthStencilBuffer() )
	{
		m_renderer->GetDepthStencilBuffer()->Clear();
	}
}

void systems::RenderingSystem::OnDraw()
{
	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypeOfSystem< systems::RenderingSystem >();

	Matrix vp = m_camerasSystem->GetActiveCamera()->GetCamera().GetViewProjectionMatrix();

	for( systems::Archetype* archetype : archetypes )
	{
		const DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
		const DataPackage< forge::RenderingComponentData >& renderables = archetype->GetData< forge::RenderingComponentData >();

		for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
		{
			m_buffer->GetData().WVP = transforms[ i ].ToMatrix() * vp;
			m_buffer->GetData().color = renderables[ i ].m_color;
			m_buffer->SetVS( 1 );

			m_renderer->GetContext()->Draw( m_indexBuffer->GetIndicesAmount(), 0 );
		}
	}
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
