//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Renderer/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "../Renderer/PerspectiveCamera.h"

Int32 main()
{
	const Uint32 width = 1600;
	const Uint32 height = 900;
	const Float aspectRatio = static_cast< Float >( width ) / static_cast< Float >( height );

	std::unique_ptr< ICamera > camera = std::make_unique< PerspectiveCamera >( aspectRatio, FORGE_PI / 3.0f, 0.1f, 100.0f );

	Time::Initialize();

	StopWatch stopWatch;

	stopWatch.Reset();
	auto window = IWindow::CreateNewWindow( 1600, 900 );
	FORGE_LOG( "Window creating time duration: %f sec", stopWatch.GetDuration() );

	stopWatch.Reset();
	auto renderer = IRenderer::CreateRenderer( *window );

	FORGE_LOG( "Renderer creating time duration: %f sec", stopWatch.GetDuration() );

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

	camera->SetPosition( { 0.0f, 0.0f, 0.0f } );

	while( true )
	{
		Time::Update();
		window->Update();

		if( window->GetInput()->GetMouseButtonDown( IInput::MouseButton::LeftButton ) )
		{
			FORGE_LOG( "Clicked on: %s", window->GetInput()->GetMouseCurrentAxises().ToDebugString().c_str() );
		}
	

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

		Vector3 pos = camera->GetPosition();
		delta *= 4.0f;
		delta = camera->GetOrientation().Transform( delta );
		pos += delta * Time::GetDeltaTime();
		camera->SetPosition( pos );

		Float xRot = 0.0f;
		Float yRot = 0.0f;
		Float zRot = 0.0f;

		if( window->GetInput()->GetKey( IInput::Key::UpArrow ) )
		{
			xRot += FORGE_PI_HALF;
		}

		if( window->GetInput()->GetKey( IInput::Key::DownArrow ) )
		{
			xRot -= FORGE_PI_HALF;
		}

		if( window->GetInput()->GetKey( IInput::Key::LeftArrow ) )
		{
			zRot += FORGE_PI_HALF;
		}

		if( window->GetInput()->GetKey( IInput::Key::RightArrow ) )
		{
			zRot -= FORGE_PI_HALF;
		}

		if( window->GetInput()->GetKey( IInput::Key::N ) )
		{
			yRot -= FORGE_PI_HALF;
		}

		if( window->GetInput()->GetKey( IInput::Key::M ) )
		{
			yRot += FORGE_PI_HALF;
		}

		Quaternion rot = camera->GetOrientation();
		camera->SetOrientation( rot * Quaternion( xRot * Time::GetDeltaTime(), yRot * Time::GetDeltaTime(), zRot * Time::GetDeltaTime() ) );

		if( window->GetInput()->GetKey( IInput::Key::R ) )
		{
			camera->SetTransform( Transform::IDENTITY() );
		}

		FORGE_LOG( "Camera pos: %s", pos.ToDebugString().c_str() );

		{
			struct cbPerObject
			{
				Matrix WVP;
			};

			auto buff = renderer->GetConstantBuffer< cbPerObject >();

			buff->GetData().WVP = camera->GetViewProjectionMatrix();
			buff->SetVS( 1 );
		}

		renderer->BeginScene();
		renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
		renderer->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
		renderer->GetSwapchain()->Present();
	}
}