#pragma once

struct Vertex
{
	Vertex();
	Vertex( Vector3 pos, Vector4 clr );
	~Vertex();

	Vector3 m_pos;
	Vector4 m_clr;

	static const D3D11_INPUT_ELEMENT_DESC c_layout[];

	static const D3D11_INPUT_ELEMENT_DESC* const& GetLayout();
	static Uint32 GetElementsAmount();
};


class D3D11VertexBuffer
{
public:
	using ElementType = Vertex;

	D3D11VertexBuffer( D3D11Context* contextPtr, const D3D11Device& device, const ElementType* vertices, Uint32 amount );
	~D3D11VertexBuffer();

	ID3D11Buffer* const& GetBuffer() const
	{
		return m_buffer;
	}

	Uint32 GetStride() const
	{
		return sizeof( ElementType );
	}

	void Set();

private:
	ID3D11Buffer* m_buffer;
	D3D11Context* m_contextPtr;
};

