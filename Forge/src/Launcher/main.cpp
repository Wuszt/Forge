//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Renderer/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/IInput.h"

Int32 main()
{
	Time::Initialize();

	StopWatch stopWatch;

	stopWatch.Reset();
	auto window = IWindow::CreateNewWindow( 700, 700 );
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
		{ InputPosition( -0.5f, -0.5f, 0.5f ), InputColor( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ InputPosition( -0.5f, 0.5f, 0.5f ), InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ InputPosition( 0.5f, 0.5f, 0.5f ), InputColor( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ InputPosition( 0.5f, -0.5f, 0.5f ), InputColor( 0.0f, 1.0f, 0.0f, 1.0f ) },
	};

	stopWatch.Reset();

	auto vertexBuffer = renderer->GetVertexBuffer( vertices );
	vertexBuffer->Set();

	FORGE_LOG( "VertexBuffer creation time: %f sec", stopWatch.GetDuration() );

	auto inputLayout = renderer->GetInputLayout( *vertexShader, *vertexBuffer );
	inputLayout->Set();

	Uint32 indices[] =
	{
		0,1,2,0,2,3
	};

	auto indexBuffer = renderer->GetIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
	indexBuffer->Set( 0 );

	while( true )
	{
		Time::Update();
		window->Update();

		if( window->GetInput()->GetMouseButtonDown( IInput::MouseButton::LeftButton ) )
		{
			FORGE_LOG( "Clicked on: %s", window->GetInput()->GetMouseCurrentAxises().ToDebugString().c_str() );
		}

		renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
		renderer->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
		renderer->GetSwapchain()->Present();
	}
}