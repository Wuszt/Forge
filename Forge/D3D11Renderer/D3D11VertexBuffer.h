#pragma once
#include "../Renderer/IVertexBuffer.h"

class D3D11RenderContext;
class D3D11Device;
struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11Buffer;

struct D3D11Vertex : public IVertex
{
	D3D11Vertex() {}

	D3D11Vertex( Vector3 pos, Vector4 clr )
		: m_pos( pos )
		, m_clr( clr )
	{}

	~D3D11Vertex() {}

	Vector3 m_pos;
	Vector4 m_clr;

	static const D3D11_INPUT_ELEMENT_DESC c_layout[];

	static const D3D11_INPUT_ELEMENT_DESC* const& GetLayout();

	static Uint32 GetElementsAmount();
};

struct ID3D11Vertices : public IVertices
{
	virtual const D3D11_INPUT_ELEMENT_DESC* const& GetLayout() = 0;
	virtual Uint32 GetElementsAmount() = 0;
};

template< class VertexType >
struct D3D11Vertices : public ID3D11Vertices
{
	std::vector< VertexType > m_vertices;

	FORGE_INLINE virtual Uint32 GetByteWidth() const override
	{
		return m_vertices.size() * sizeof( VertexType );
	}

	FORGE_INLINE virtual const void* GetData() const override
	{
		return static_cast<const void*>( m_vertices.data() );
	}

	virtual const D3D11_INPUT_ELEMENT_DESC* const& GetLayout() override
	{
		return VertexType::GetLayout();
	}

	virtual Uint32 GetElementsAmount() override
	{
		return VertexType::GetElementsAmount();
	}
};


class D3D11VertexBuffer : public IVertexBuffer
{
public:
	using ElementType = D3D11Vertex;

	D3D11VertexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const IVertices& vertices );
	~D3D11VertexBuffer();

	ID3D11Buffer* const& GetBuffer() const
	{
		return m_buffer;
	}

	Uint32 GetStride() const
	{
		return sizeof( ElementType );
	}

	virtual void Set() override;

private:
	ID3D11Buffer* m_buffer;
	D3D11RenderContext* m_contextPtr;
};

