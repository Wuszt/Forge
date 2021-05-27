//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"

#include <Windows.h>

#include "../Renderer/PublicDefaults.h"
#include "../D3D11Renderer/D3D11VertexBuffer.h"

Int32 main()
{
	auto renderer = IRenderer::CreateRenderer( 700, 700 );

	auto vertexShader = renderer->GetVertexShader( "Effects.fx" );
	auto pixelShader = renderer->GetPixelShader( "Effects.fx" );

	vertexShader->Set();
	pixelShader->Set();

	D3D11Vertices< D3D11Vertex > vertices;
	vertices.m_vertices =
	{
		D3D11Vertex( Vector3( -0.5f, -0.5f, 0.5f ), Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) ),
		D3D11Vertex( Vector3( -0.5f, 0.5f, 0.5f ), Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) ),
		D3D11Vertex( Vector3( 0.5f, 0.5f, 0.5f ), Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) ),
		D3D11Vertex( Vector3( 0.5f, -0.5f, 0.5f ), Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) ),
	};

	auto vertexBuffer = renderer->GetVertexBuffer( vertices );
	vertexBuffer->Set();

	auto inputLayout = renderer->GetInputLayout( *vertexShader, *vertexBuffer );
	inputLayout->Set();

	Uint32 indices[] =
	{
		0,1,2,0,2,3
	};

	auto indexBuffer = renderer->GetIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
	indexBuffer->Set( 0 );

	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	while( true )
	{
		BOOL PeekMessageL(
			LPMSG lpMsg,
			HWND hWnd,
			Uint32 wMsgFilterMin,
			Uint32 wMsgFilterMax,
			Uint32 wRemoveMsg
		);

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT )
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			renderer->GetRenderTargetView()->Clear( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
			renderer->GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
			renderer->GetSwapchain()->Present();
		}
	}
}