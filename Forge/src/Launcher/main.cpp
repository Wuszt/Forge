//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Renderer/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "../Math/Random.h"
#include "../Core/FPSCounter.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#endif

void DrawFPSOverlay()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	const float PAD = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + PAD;
	window_pos.y = work_pos.y + PAD;
	window_pos_pivot.x = 0.0f;
	window_pos_pivot.y = 0.0f;
	ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha( 0.35f );
	Bool tmp = true;
	if( ImGui::Begin( "Overlay", &tmp, window_flags ) )
	{
		ImGui::Text( "FPS: %.2f", FPSCounter::GetAverageFPS( 1u ) );
	}
	ImGui::End();
}

Int32 main()
{
	const Uint32 width = 1600;
	const Uint32 height = 900;

	Time::Initialize();

	StopWatch stopWatch;

	stopWatch.Reset();
	auto window = IWindow::CreateNewWindow( width, height );
	FORGE_LOG( "Window creating time duration: %f sec", stopWatch.GetDuration() );

	stopWatch.Reset();
	auto renderer = IRenderer::CreateRenderer( *window, RendererType::D3D11 );

	FORGE_LOG( "Renderer creating time duration: %f sec", stopWatch.GetDuration() );

	std::unique_ptr< IMGUISystem > imguiSystem = std::make_unique< IMGUISystem >( *window, *renderer );

	std::unique_ptr< ICamera > camera = std::make_unique< PerspectiveCamera >( window->GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 2000.0f );
	camera->SetPosition( { 0.0f, 0.0f, 50.0f } );

	std::unique_ptr< CallbackToken > windowEventCallback = window->RegisterEventListener( [&]( const IWindow::IEvent& event )
	{
		if( event.GetEventType() == IWindow::EventType::OnWindowResized )
		{
			auto prevCamera = std::move( camera );
			camera = std::make_unique< PerspectiveCamera >( window->GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 2000.0f );
			camera->SetTransform( prevCamera->GetTransform() );
		}
	} );

	stopWatch.Reset();

	auto vertexShader = renderer->GetVertexShader( "Effects.fx" );
	auto pixelShader = renderer->GetPixelShader( "Effects.fx" );

	FORGE_LOG( "Shaders compilation time: %f sec", stopWatch.GetDuration() );

	vertexShader->Set();
	pixelShader->Set();

	Vertices< IVertex< InputPosition, InputColor > > vertices;

	vertices.m_vertices =
	{
		{ InputPosition( -1.0f, -1.0f, -1.0f ), InputColor( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ InputPosition( 1.0f,	-1.0f, -1.0f ), InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ InputPosition( 1.0f, 1.0f, -1.0f ), InputColor( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ InputPosition( -1.0f, 1.0f, -1.0f ), InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },

		{ InputPosition( -1.0f, -1.0f, 1.0f ), InputColor( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ InputPosition( 1.0f,	-1.0f, 1.0f ), InputColor( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ InputPosition( 1.0f, 1.0f, 1.0f ), InputColor( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ InputPosition( -1.0f, 1.0f, 1.0f ), InputColor( 0.3f, 0.7f, 0.6f, 1.0f ) },
	};

	stopWatch.Reset();

	auto vertexBuffer = renderer->CreateVertexBuffer( vertices );
	vertexBuffer->Set();

	FORGE_LOG( "VertexBuffer creation time: %f sec", stopWatch.GetDuration() );

	auto inputLayout = renderer->CreateInputLayout( *vertexShader, *vertexBuffer );
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

	auto indexBuffer = renderer->CreateIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
	indexBuffer->Set( 0 );

	Bool imguiExample = false;

	Vector3 cameraEulerAngles;

	Bool wasShiftAndWheelPressed = false;

	Float currentSpeed = 3.0f;

	Math::Random rng;
	Uint32 seed = rng.GetRaw();
	while( true )
	{
		Time::Update();
		FPSCounter::OnUpdate( Time::GetDeltaTime() );
		window->Update();
		imguiSystem->OnNewFrame();

		if( window->GetInput()->GetMouseButtonDown( IInput::MouseButton::LeftButton ) )
		{
			FORGE_LOG( "Clicked on: %s", window->GetInput()->GetMouseCurrentAxises().ToDebugString().c_str() );
		}

		if( window->GetInput()->GetKey( IInput::Key::Shift ) && window->GetInput()->GetMouseButton( IInput::MouseButton::MiddleButton ) )
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

			if( window->GetInput()->GetKey( IInput::Key::D ) )
			{
				delta.X = 1.0f;
			}

			if( window->GetInput()->GetKey( IInput::Key::A ) )
			{
				delta.X = -1.0f;
			}

			if( window->GetInput()->GetKey( IInput::Key::W ) )
			{
				delta.Y = 1.0f;
			}

			if( window->GetInput()->GetKey( IInput::Key::S ) )
			{
				delta.Y = -1.0f;
			}

			if( window->GetInput()->GetKey( IInput::Key::Q ) )
			{
				delta.Z = -1.0f;
			}

			if( window->GetInput()->GetKey( IInput::Key::E ) )
			{
				delta.Z = 1.0f;
			}

			currentSpeed += static_cast< Float >( window->GetInput()->GetMouseDeltaAxises().Z ) * 0.003f;
			currentSpeed = Math::Max( 0.1f, currentSpeed );

			Vector3 pos = camera->GetPosition();
			delta *= Math::Pow( 2.0f, currentSpeed );

			if( window->GetInput()->GetKey( IInput::Key::Shift ) )
			{
				delta *= 4.0f;
			}

			delta = camera->GetOrientation().Transform( delta );
			pos += delta * Time::GetDeltaTime();
			camera->SetPosition( pos );

			Vector3 deltaRot = window->GetInput()->GetMouseDeltaAxises() * 0.001f;

			deltaRot.Z = -deltaRot.X;
			deltaRot.X = deltaRot.Y;
			deltaRot.Y = 0.0f;

			cameraEulerAngles += deltaRot;

			camera->SetOrientation( Quaternion( 0.0f, 0.0f, cameraEulerAngles.Z ) * Quaternion( cameraEulerAngles.X, 0.0f, 0.0f ) );

			if( window->GetInput()->GetKey( IInput::Key::R ) )
			{
				camera->SetTransform( Transform::IDENTITY() );
			}
		}

		renderer->BeginScene();
		renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
		if( renderer->GetDepthStencilBuffer() )
		{
			renderer->GetDepthStencilBuffer()->Clear();
		}

		struct cbPerObject
		{
			Matrix WVP;
			Vector4 color;
		};


		//Ground
		{
			auto buff = renderer->GetConstantBuffer< cbPerObject >();

			Matrix m;
			m.SetTranslation( 0.0f, 0.0f, 0.0f );
			m.SetScale( { 2000.0f, 2000.0f, 0.01f } );
			buff->GetData().WVP = m * camera->GetViewProjectionMatrix();
			buff->GetData().color = Vector4( 0.0f, 0.6f, 0.0f, 1.0f );
			buff->SetVS( 1 );
			renderer->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
		}		

		Math::Random rng( seed );
		const Uint32 dim = 50u;
		for( Uint32 i = 0; i < dim * dim; ++i )
		{
			if( rng.GetFloat() > 0.9f )
			{
				continue;
			}

			auto buff = renderer->GetConstantBuffer< cbPerObject >();

			Matrix m;

			Float scaleZ = rng.GetFloat( 20.0f, 100.0f );
			Float scaleXY = rng.GetFloat( 10.0f, 20.0f );
			m.SetScale( { scaleXY, scaleXY, scaleZ } );

			Float x = static_cast<Float>( i % dim ) * 50.0f;
			Float y = static_cast<Float>( i / dim ) * 50.0f;
			m.SetTranslation( x - dim * 25, y - dim * 25, scaleZ );

			buff->GetData().WVP = m * camera->GetViewProjectionMatrix();
			buff->GetData().color = Vector4( rng.GetFloat(), rng.GetFloat(), rng.GetFloat(), 1.0f );
			buff->SetVS( 1 );

			renderer->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
		}

		ImGui::ShowDemoWindow( &imguiExample );
		DrawFPSOverlay();
		imguiSystem->Render();

		renderer->GetSwapchain()->Present();
	}
}