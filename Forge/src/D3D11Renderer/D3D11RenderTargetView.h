#pragma once
#include "../Renderer/IRenderTargetView.h"

class D3D11Device;
class D3D11Texture;
struct Vector4;
struct ID3D11RenderTargetView;

class D3D11RenderTargetView : public IRenderTargetView
{
public:
	D3D11RenderTargetView( const D3D11Device& device, const D3D11RenderContext& context, std::shared_ptr< D3D11Texture > texture );
	~D3D11RenderTargetView();

	FORGE_INLINE ID3D11RenderTargetView * const& GetRenderTargetView() const
	{
		return m_renderTargetView;
	}

	FORGE_INLINE std::shared_ptr< D3D11Texture > GetTexture() const
	{
		return m_texture;
	}

	virtual void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } ) override;

private:
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	const D3D11RenderContext& m_context;
	std::shared_ptr< D3D11Texture > m_texture = nullptr;
};

