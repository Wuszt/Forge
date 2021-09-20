//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Renderer/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "../Math/Random.h"
#include "../Core/FPSCounter.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#endif
#include "../Systems/TransformSystem.h"

Int32 main()
{
	const Uint32 width = 1600;
	const Uint32 height = 900;

	forge::GameInstance gameInstance;

	systems::SystemsManager::BootContext ctx;
	gameInstance.GetSystemsManager().Boot( ctx );

	forge::Time::Initialize();

	forge::StopWatch stopWatch;

	stopWatch.Reset();
	auto window = forge::IWindow::CreateNewWindow( width, height );
	FORGE_LOG( "Window creating time duration: %.2f sec", stopWatch.GetDuration() );

	stopWatch.Reset();
	auto rendererInstance = renderer::IRenderer::CreateRenderer( *window, renderer::RendererType::D3D11 );

	FORGE_LOG( "Renderer creating time duration: %.2f sec", stopWatch.GetDuration() );

	std::unique_ptr< forge::IMGUISystem > imguiSystem = std::make_unique< forge::IMGUISystem >( *window, *rendererInstance );

	std::unique_ptr< forge::ICamera > camera = std::make_unique< forge::PerspectiveCamera >( window->GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 2000.0f );
	camera->SetPosition( { 0.0f, 0.0f, 50.0f } );

	forge::CallbackToken windowEventCallback = window->RegisterEventListener( [&]( const forge::IWindow::IEvent& event )
	{
		if( event.GetEventType() == forge::IWindow::EventType::OnWindowResized )
		{
			auto prevCamera = std::move( camera );
			camera = std::make_unique< forge::PerspectiveCamera >( window->GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 2000.0f );
			camera->SetTransform( prevCamera->GetTransform() );
		}
	} );

	auto fpsOverlayDrawingToken = imguiSystem->AddOverlayListener( []()
	{
		Float fps = forge::FPSCounter::GetAverageFPS( 1u );
		Vector4 color{ 1.0f, 0.0f, 0.0f, 1.0f };

		if( fps > 30.0f )
		{
			if( fps > 6.0f )
			{
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				color = { 1.0f, 1.0f, 0.0f, 1.0f };
			}
		}

		Uint32 bufferSize = 0u;
		Uint32 currentOffset = 0u;
		Float* buff = forge::FPSCounter::GetFPSCounter().GetBuffer( currentOffset, bufferSize );

		ImGui::PlotHistogram( "", buff, bufferSize, currentOffset, NULL, 0.0f, 120.0f, ImVec2( 100.0f, 40.0f ) );
		ImGui::SameLine();

		ImGui::SetWindowFontScale( 1.5f );
		ImGui::PushStyleColor( ImGuiCol_Text, { color.X, color.Y, color.Z, color.W } );
		ImGui::Text( "FPS: %.2f", fps );
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale( 1.0f );
	} );

	auto cameraDataOverlayDrawingToken = imguiSystem->AddOverlayListener( [ &camera ]()
	{
		ImGui::Text( "Cam Pos:%s", camera->GetPosition().ToDebugString().c_str() );
		ImGui::Text( "Cam Fwd:%s", ( camera->GetOrientation() * Vector3::EY() ).ToDebugString( 3 ).c_str() );
	} );

	stopWatch.Reset();

	auto vertexShader = rendererInstance->GetVertexShader( "Effects.fx" );
	auto pixelShader = rendererInstance->GetPixelShader( "Effects.fx" );

	FORGE_LOG( "Shaders compilation time: %.2f sec", stopWatch.GetDuration() );

	vertexShader->Set();
	pixelShader->Set();

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

	stopWatch.Reset();

	auto vertexBuffer = rendererInstance->CreateVertexBuffer( vertices );
	vertexBuffer->Set();

	FORGE_LOG( "VertexBuffer creation time: %.2f sec", stopWatch.GetDuration() );

	auto inputLayout = rendererInstance->CreateInputLayout( *vertexShader, *vertexBuffer );
	inputLayout->Set();

	Uint32 indices[ 6 * 6 ] =
	{
		0, 3, 1, 3, 2, 1,
		1, 2, 5, 2, 6, 5,
		5, 6, 4, 6, 7, 4,
		4, 7, 0, 7, 3, 0,
		3, 7, 2, 7, 6, 2,
		4, 0, 5, 0, 1, 5
	};

	auto indexBuffer = rendererInstance->CreateIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
	indexBuffer->Set( 0 );

	Bool imguiExample = false;

	Vector3 cameraEulerAngles;

	Bool wasShiftAndWheelPressed = false;

	Float currentSpeed = 3.0f;

	Math::Random rng;

	const Uint32 dim = 50u;
	for( Uint32 i = 0; i < dim * dim; ++i )
	{
		if( rng.GetFloat() > 0.9f )
		{
			continue;
		}

		forge::Entity* entity = gameInstance.GetEntitiesManager().CreateEntity();
		auto* transformComponent = entity->AddComponent< forge::DataComponent< forge::TransformComponentData > >();

		Matrix m;

		Float scaleZ = rng.GetFloat( 20.0f, 100.0f );
		Float scaleXY = rng.GetFloat( 10.0f, 20.0f );
		transformComponent->GetData().m_scale = { scaleXY, scaleXY, scaleZ };

		Float x = static_cast<Float>( i % dim ) * 50.0f;
		Float y = static_cast<Float>( i / dim ) * 50.0f;
		transformComponent->GetData().m_transform.SetPosition( { x - dim * 25, y - dim * 25, scaleZ } );
	}

	Uint32 colorSeed = rng.GetRaw();
	while( true )
	{
		forge::Time::Update();
		forge::FPSCounter::OnUpdate( forge::Time::GetDeltaTime() );
		window->Update();
		imguiSystem->OnNewFrame();

		if( window->GetInput()->GetMouseButtonDown( forge::IInput::MouseButton::LeftButton ) )
		{
			FORGE_LOG( "Clicked on: %s", window->GetInput()->GetMouseCurrentAxises().ToDebugString( 0u ).c_str() );
		}

		if( window->GetInput()->GetKey( forge::IInput::Key::Shift ) && window->GetInput()->GetMouseButton( forge::IInput::MouseButton::MiddleButton ) )
		{
			if( !wasShiftAndWheelPressed )
			{
				window->GetInput()->LockCursor( !window->GetInput()->IsCursorLocked() );
			}

			wasShiftAndWheelPressed = true;
		}
		else
		{
			wasShiftAndWheelPressed = false;
		}

		if( window->GetInput()->IsCursorLocked() )
		{
			Vector3 delta;

			if( window->GetInput()->GetKey( forge::IInput::Key::D ) )
			{
				delta.X = 1.0f;
			}

			if( window->GetInput()->GetKey( forge::IInput::Key::A ) )
			{
				delta.X = -1.0f;
			}

			if( window->GetInput()->GetKey( forge::IInput::Key::W ) )
			{
				delta.Y = 1.0f;
			}

			if( window->GetInput()->GetKey( forge::IInput::Key::S ) )
			{
				delta.Y = -1.0f;
			}

			if( window->GetInput()->GetKey( forge::IInput::Key::Q ) )
			{
				delta.Z = -1.0f;
			}

			if( window->GetInput()->GetKey( forge::IInput::Key::E ) )
			{
				delta.Z = 1.0f;
			}

			currentSpeed += static_cast< Float >( window->GetInput()->GetMouseDeltaAxises().Z ) * 0.003f;
			currentSpeed = Math::Max( 0.1f, currentSpeed );

			Vector3 pos = camera->GetPosition();
			delta *= Math::Pow( 2.0f, currentSpeed );

			if( window->GetInput()->GetKey( forge::IInput::Key::Shift ) )
			{
				delta *= 4.0f;
			}

			delta = camera->GetOrientation().Transform( delta );
			pos += delta * forge::Time::GetDeltaTime();
			camera->SetPosition( pos );

			Vector3 deltaRot = window->GetInput()->GetMouseDeltaAxises() * 0.001f;

			deltaRot.Z = -deltaRot.X;
			deltaRot.X = deltaRot.Y;
			deltaRot.Y = 0.0f;

			cameraEulerAngles += deltaRot;

			camera->SetOrientation( Quaternion( 0.0f, 0.0f, cameraEulerAngles.Z ) * Quaternion( cameraEulerAngles.X, 0.0f, 0.0f ) );

			if( window->GetInput()->GetKey( forge::IInput::Key::R ) )
			{
				camera->SetTransform( Transform::IDENTITY() );
			}
		}

		rendererInstance->BeginScene();
		rendererInstance->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
		if( rendererInstance->GetDepthStencilBuffer() )
		{
			rendererInstance->GetDepthStencilBuffer()->Clear();
		}

		struct cbPerObject
		{
			Matrix WVP;
			Vector4 color;
		};

		auto& archetypes = gameInstance.GetSystemsManager().GetArchetypesWithDataType< forge::TransformComponentData >();

		rng.SetSeed( colorSeed );
		for( systems::Archetype* archetype : archetypes )
		{
			systems::DataPackage< forge::TransformComponentData >& dataPackage = archetype->GetData< forge::TransformComponentData >();

			for( forge::TransformComponentData& transform : dataPackage )
			{
				auto buff = rendererInstance->GetConstantBuffer< cbPerObject >();

				buff->GetData().WVP = transform.ToMatrix() * camera->GetViewProjectionMatrix();
				buff->GetData().color = Vector4( rng.GetFloat(), rng.GetFloat(), rng.GetFloat(), 1.0f );
				buff->SetVS( 1 );

				rendererInstance->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
			}
		}

		//Ground
		{
			auto buff = rendererInstance->GetConstantBuffer< cbPerObject >();

			Matrix m;
			m.SetTranslation( 0.0f, 0.0f, 0.0f );
			m.SetScale( { 2000.0f, 2000.0f, 0.01f } );
			buff->GetData().WVP = m * camera->GetViewProjectionMatrix();
			buff->GetData().color = Vector4( 0.0f, 0.6f, 0.0f, 1.0f );
			buff->SetVS( 1 );
			rendererInstance->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
		}		

		ImGui::ShowDemoWindow( &imguiExample );
		imguiSystem->Render();

		rendererInstance->GetSwapchain()->Present();
	}
}