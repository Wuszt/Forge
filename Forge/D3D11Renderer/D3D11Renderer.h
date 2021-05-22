#pragma once

class D3D11Window;

class D3D11Renderer
{
public:
	D3D11Renderer( Uint32 width, Uint32 height );
	~D3D11Renderer();

private:
	std::unique_ptr< D3D11Window > m_window;
};

