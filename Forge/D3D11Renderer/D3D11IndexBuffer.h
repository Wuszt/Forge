#pragma once
class D3D11Device;
class D3D11Context;
struct ID3D11Buffer;

class D3D11IndexBuffer
{
public:
	D3D11IndexBuffer( D3D11Context* contextPtr, const D3D11Device& device, const Uint32* indices, Uint32 amount );
	~D3D11IndexBuffer();

	FORGE_INLINE ID3D11Buffer* const& GetBuffer() const
	{
		return m_buffer;
	}

	void Set( Uint32 offset );

private:
	ID3D11Buffer* m_buffer = nullptr;
	D3D11Context* m_contextPtr = nullptr;
};

