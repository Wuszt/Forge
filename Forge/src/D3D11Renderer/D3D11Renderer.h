#pragma once
#include "../Renderer/IRenderer.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderTargetView.h"
#include "D3D11DepthStencilBuffer.h"
#include "D3D11ShadersManager.h"
#include "D3D11Swapchain.h"

struct ID3D11RasterizerState;

namespace windows
{
	class WindowsWindow;
}

namespace renderer
{
	class IVertexShader;
	class IVertexBuffer;
	class IInputLayout;
	class IIndexBuffer;
	class IRenderTargetView;
	class ShaderDefine;
}

namespace ecs
{
	class EntityID;
	class ECSManager;
}

namespace forge
{
	class IWindow;
}

namespace d3d11
{
	class D3D11Device;
	class D3D11ShadersManager;
	class D3D11Swapchain;

	class D3D11Renderer : public renderer::IRenderer
	{
	public:
		D3D11Renderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window );
		~D3D11Renderer();

		D3D11Device* GetDevice() const
		{
			return m_device.get();
		}

		virtual D3D11RenderContext* GetContext() const override
		{
			return m_context.get();
		}

		virtual D3D11Swapchain* GetSwapchain() const override
		{
			return m_swapChain.get();
		}

		virtual D3D11ShadersManager* GetShadersManager() const override
		{
			return m_shadersManager.get();
		}

		virtual std::unique_ptr< renderer::IInputLayout > CreateInputLayout( const renderer::IVertexShader& vertexShader, const renderer::IVertexBuffer& vertexBuffer ) const override;
		virtual std::unique_ptr< renderer::IVertexBuffer > CreateVertexBuffer( const renderer::Vertices& vertices ) const override;
		virtual std::unique_ptr< renderer::IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const override;
		virtual std::unique_ptr< renderer::ITexture > CreateTexture( Uint32 width, Uint32 height, renderer::ITexture::Flags flags, renderer::ITexture::Format format, renderer::ITexture::Type type, renderer::ITexture::Format srvFormat = renderer::ITexture::Format::Unknown ) const override;
		virtual std::unique_ptr< renderer::IBlendState > CreateBlendState( const renderer::BlendOperationDesc& rgbOperation, const renderer::BlendOperationDesc& alphaDesc ) const override;
		virtual std::unique_ptr< renderer::IDepthStencilBuffer > CreateDepthStencilBuffer( Uint32 width, Uint32 height, Bool cubeTexture = false ) const override;
		virtual std::unique_ptr< renderer::IDepthStencilState > CreateDepthStencilState( renderer::DepthStencilComparisonFunc comparisonFunc ) const override;
		virtual std::unique_ptr< renderer::ISamplerState > CreateSamplerState( renderer::SamplerStateFilterType filterType, renderer::SamplerStateComparisonType comparisonType ) override;

		virtual void SetCullingMode( renderer::CullingMode mode );
		virtual renderer::CullingMode GetCullingMode() const override;
		virtual void SetDepthBias( Int32 bias, Float slopeScaledBias, Float clamp ) override;
		virtual void SetRenderTargets( const forge::ArraySpan< renderer::IRenderTargetView* >& rendererTargetViews, renderer::IDepthStencilView* depthStencilView ) override;
		virtual void SetSamplerStates( const forge::ArraySpan< renderer::ISamplerState* > samplerStates ) override;
		virtual void SetShaderResourceViews( const forge::ArraySpan< const renderer::IShaderResourceView* >& input, Uint32 startIndex = 0u ) override;
		virtual void ClearShaderResourceViews() override;

		virtual void OnBeforeDraw() override;

		virtual void SetViewportSize( const Vector2& size ) override;

		virtual void AddRenderableECSFragment( ecs::ECSManager& ecsManager, ecs::EntityID entityID ) const override;
		virtual void UpdateRenderableECSFragment( ecs::ECSManager& ecsManager, ecs::EntityID entityID, const renderer::Renderable& renderable ) const override;

		virtual void Draw( const renderer::IRawRenderableFragment& fragment, renderer::RenderingPass renderingPass, const renderer::ShaderDefine* shaderDefine = nullptr, forge::ArraySpan< const renderer::IShaderResourceView* > additionalSRVs = {} ) override;
		virtual void Draw( const ecs::Archetype& archetype, renderer::RenderingPass renderingPass, const renderer::ShaderDefine* shaderDefine = nullptr, forge::ArraySpan< const renderer::IShaderResourceView* > additionalSRVs = {} ) override;
		virtual void DrawRawVertices( Uint32 amount ) override;

		virtual renderer::RendererType GetType() const override
		{
			return renderer::RendererType::D3D11;
		}

		virtual std::unique_ptr< renderer::IConstantBufferImpl > CreateConstantBufferImpl() const override;

	private:
		void SetRenderTargets( const forge::ArraySpan< renderer::IRenderTargetView* >& rendererTargetViews, D3D11DepthStencilView* depthStencilView );
		void InitializeSwapChainAndContext( const windows::WindowsWindow& window, Uint32 renderingResolutionWidth, Uint32 renderingResolutionHeight );

		std::unique_ptr< D3D11Device > m_device;
		std::unique_ptr< D3D11RenderContext > m_context;
		std::unique_ptr< D3D11Swapchain > m_swapChain;

		std::unique_ptr< D3D11RenderTargetView > m_renderTargetView;

		std::unique_ptr< D3D11ShadersManager > m_shadersManager;

		ID3D11RasterizerState* m_rasterizerState = nullptr;

		forge::CallbackToken m_windowCallbackToken;
	};
}

