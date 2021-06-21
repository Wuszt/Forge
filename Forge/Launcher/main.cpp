//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"

#include <windows.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


using namespace DirectX;

//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

ID3D11Buffer* VertBuffer;
ID3D11Buffer* IndexBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;

//Global Declarations - Others//
LPCTSTR WndClassName = "firstwindow";
HWND hwnd = NULL;
HRESULT hr;

const Int32 Width = 300;
const Int32 Height = 300;

//Function Prototypes//
bool InitializeDirect3d11App( HINSTANCE hInstance );
void CleanUp();
bool InitScene();
void UpdateScene();
void DrawScene();

bool InitializeWindow( HINSTANCE hInstance,
	Int32 ShowWnd,
	Int32 width, Int32 height,
	bool windowed );
WPARAM messageloop();

LRESULT CALLBACK WndProc( HWND hWnd,
	Uint32 msg,
	WPARAM wParam,
	LPARAM lParam );

struct Vertex
{
	Vertex() {}
	Vertex( Float x, Float y, Float z,
		Float cr, Float cg, Float cb, Float ca )
		: pos( x, y, z ), color( cr, cg, cb, ca ) {}

	Vector3 pos;
	Vector4 color;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
Uint32 numElements = ARRAYSIZE( layout );
///////////////**************new**************////////////////////

Int32 WINAPI WinMain( HINSTANCE hInstance,	//Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	Int32 nShowCmd )
{

	if( !InitializeWindow( hInstance, nShowCmd, Width, Height, true ) )
	{
		MessageBox( 0, "Window Initialization - Failed",
			"Error", MB_OK );
		return 0;
	}

	if( !InitializeDirect3d11App( hInstance ) )	//Initialize Direct3D
	{
		MessageBox( 0, "Direct3D Initialization - Failed",
			"Error", MB_OK );
		return 0;
	}

	if( !InitScene() )	//Initialize our scene
	{
		MessageBox( 0, "Scene Initialization - Failed",
			"Error", MB_OK );
		return 0;
	}

	messageloop();

	CleanUp();

	return 0;
}

bool InitializeWindow( HINSTANCE hInstance,
	Int32 ShowWnd,
	Int32 width, Int32 height,
	bool windowed )
{
	typedef struct _WNDCLASS {
		Uint32 cbSize;
		Uint32 style;
		WNDPROC lpfnWndProc;
		Int32 cbClsExtra;
		Int32 cbWndExtra;
		HANDLE hInstance;
		HICON hIcon;
		HCURSOR hCursor;
		HBRUSH hbrBackground;
		LPCTSTR lpszMenuName;
		LPCTSTR lpszClassName;
	} WNDCLASS;

	WNDCLASSEX wc;

	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 2 );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

	if( !RegisterClassEx( &wc ) )
	{
		MessageBox( NULL, "Error registering class",
			"Error", MB_OK | MB_ICONERROR );
		return 1;
	}

	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		"Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if( !hwnd )
	{
		MessageBox( NULL, "Error creating window",
			"Error", MB_OK | MB_ICONERROR );
		return 1;
	}

	ShowWindow( hwnd, ShowWnd );
	UpdateWindow( hwnd );

	return true;
}

bool InitializeDirect3d11App( HINSTANCE hInstance )
{
	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory( &bufferDesc, sizeof( DXGI_MODE_DESC ) );

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon );

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&BackBuffer );

	//Create our Render Target
	hr = d3d11Device->CreateRenderTargetView( BackBuffer, NULL, &renderTargetView );
	BackBuffer->Release();

	//Set our Render Target
	d3d11DevCon->OMSetRenderTargets( 1, &renderTargetView, NULL );

	return true;
}

void CleanUp()
{
	//Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	VertBuffer->Release();
	IndexBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
}

bool InitScene()
{
	//Compile Shaders from shader file
	hr = D3DCompileFromFile( L"Effects.fx", 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &VS_Buffer, 0 );
	hr = D3DCompileFromFile( L"Effects.fx", 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &PS_Buffer, 0 );

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader( VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS );
	hr = d3d11Device->CreatePixelShader( PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS );

	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader( VS, 0, 0 );
	d3d11DevCon->PSSetShader( PS, 0, 0 );

	//Create the vertex buffer
	Vertex v[] =
	{
	Vertex( -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f ),
	Vertex( -0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
	Vertex( 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f ),
	Vertex( 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f ),
	};

	DWORD indices[] =
	{
		0,1,2,0,2,3
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof( indexBufferDesc ) );

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( DWORD ) * 2 * 3; //indices amount
	d3d11Device->CreateBuffer( &indexBufferDesc, &iinitData, &IndexBuffer );
	d3d11DevCon->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof( vertexBufferDesc ) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory( &vertexBufferData, sizeof( vertexBufferData ) );
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &VertBuffer );

	//Set the vertex buffer
	Uint32 stride = sizeof( Vertex );
	Uint32 offset = 0;
	d3d11DevCon->IASetVertexBuffers( 0, 1, &VertBuffer, &stride, &offset );

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout );

	//Set the Input Layout
	d3d11DevCon->IASetInputLayout( vertLayout );

	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;

	//Set the Viewport
	d3d11DevCon->RSSetViewports( 1, &viewport );

	return true;
}

void UpdateScene()
{

}

void DrawScene()
{
	//Clear our backbuffer
	Float bgColor[ 4 ] = { ( 0.0f, 0.0f, 0.0f, 0.0f ) };
	d3d11DevCon->ClearRenderTargetView( renderTargetView, bgColor );

	//Draw the triangle
	d3d11DevCon->DrawIndexed( 6, 0, 0 );

	//Present the backbuffer to the screen
	SwapChain->Present( 0, 0 );
}

WPARAM messageloop() {
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
		else {
			// run game code            
			UpdateScene();
			DrawScene();
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc( HWND hwnd,
	Uint32 msg,
	WPARAM wParam,
	LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ) {
			DestroyWindow( hwnd );
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hwnd,
		msg,
		wParam,
		lParam );
}