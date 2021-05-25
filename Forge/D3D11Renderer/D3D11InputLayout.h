#pragma once

class D3D11Device;
class D3D11VertexShader;
class D3D11Context;
struct ID3D11InputLayout;
enum D3D_PRIMITIVE_TOPOLOGY;

class D3D11InputLayout
{
public:
	D3D11InputLayout( D3D11Context* contextPtr, const D3D11Device& device, const D3D11VertexShader& vertexShader, const D3D11VertexBuffer& vertexBuffer );
	~D3D11InputLayout();

	ID3D11InputLayout* const& GetLayout() const
	{
		return m_layout;
	}

	D3D_PRIMITIVE_TOPOLOGY GetTopology() const;

	void Set();

private:
	ID3D11InputLayout* m_layout = nullptr;
	D3D11Context* m_contextPtr = nullptr;
};

