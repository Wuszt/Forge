//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"

#include <Windows.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "../D3D11Renderer/PublicDefaults.h"

Int32 main()
{
	D3D11Renderer renderer( 700, 700 );

	auto vertexShader = renderer.GetVertexShader( "Effects.fx" );
	auto pixelShader = renderer.GetPixelShader( "Effects.fx" );

	vertexShader->Set();
	pixelShader->Set();

	Vertex v[] =
	{
		Vertex( Vector3( -0.5f, -0.5f, 0.5f),Vector4(  1.0f, 0.0f, 0.0f, 1.0f) ),
		Vertex( Vector3( -0.5f,  0.5f, 0.5f),Vector4(  0.0f, 1.0f, 0.0f, 1.0f) ),
		Vertex( Vector3( 0.5f,  0.5f, 0.5f), Vector4( 0.0f, 0.0f, 1.0f, 1.0f )),
		Vertex( Vector3( 0.5f, -0.5f, 0.5f), Vector4( 0.0f, 1.0f, 0.0f, 1.0f )),
	};

	auto vertexBuffer = renderer.GetVertexBuffer( v, sizeof( v ) / sizeof( Vertex ) );
	vertexBuffer->Set();

	auto inputLayout = renderer.GetInputLayout( *vertexShader, *vertexBuffer );
	inputLayout->Set();

	Uint32 indices[] =
	{
		0,1,2,0,2,3
	};

	auto indexBuffer = renderer.GetIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
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
			renderer.GetRenderTargetView()->Clear();
			renderer.GetContext()->Draw( sizeof( indices ) / sizeof( Uint32 ), 0 );
			renderer.GetSwapchain()->Present();
		}
	}
}