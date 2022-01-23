#pragma once

namespace renderer
{
	class IRenderTargetView;
	struct IRawRenderablesPack;

	class IRenderingPass
	{
	public:
		IRenderingPass( IRenderer& renderer );
		virtual ~IRenderingPass() = default;

		virtual void ClearRenderTargetView();

		virtual FORGE_INLINE void SetRenderTargetView( IRenderTargetView* renderTargetView )
		{
			m_renderTargetView = renderTargetView;
		}

		FORGE_INLINE IRenderTargetView* GetRenderTargetView()
		{
			return m_renderTargetView;
		}

	protected:
		FORGE_INLINE IRenderer& GetRenderer()
		{
			return m_renderer;
		}

	private:
		IRenderTargetView* m_renderTargetView = nullptr;	
		IRenderer& m_renderer;
	};

	class IMeshesRenderingPass : public IRenderingPass
	{
	public:
		using IRenderingPass::IRenderingPass;

		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables ) = 0;
		virtual void ClearRenderTargetView() override;

		FORGE_INLINE void SetDepthStencilBuffer( IDepthStencilBuffer* depthStencilBuffer )
		{
			m_depthStencilBuffer = depthStencilBuffer;
		}

		FORGE_INLINE IDepthStencilBuffer* GetDepthStencilBuffer()
		{
			return m_depthStencilBuffer;
		}

	private:
		IDepthStencilBuffer* m_depthStencilBuffer = nullptr;
	};
}

