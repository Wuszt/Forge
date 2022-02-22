#pragma once

namespace renderer
{
	class IRenderTargetView;
	struct IRawRenderablesPack;
	struct LightData;

	class IRenderingPass
	{
	public:
		IRenderingPass( IRenderer& renderer );
		virtual ~IRenderingPass() = default;

		virtual void ClearTargetTexture();

		virtual FORGE_INLINE void SetTargetTexture( ITexture& targetTexture )
		{
			if( m_targetTexture != &targetTexture )
			{
				OnTargetTextureResized( targetTexture.GetTextureSize() );
			}

			m_targetTexture = &targetTexture;
			m_onTargetTextureResized = m_targetTexture->GetOnResizedCallback().AddListener( [&]( const Vector2& size ) { OnTargetTextureResized( size ); } );		
		}

		FORGE_INLINE ITexture* GetTargetTexture()
		{
			return m_targetTexture;
		}

	protected:
		FORGE_INLINE IRenderer& GetRenderer()
		{
			return m_renderer;
		}

		virtual void OnTargetTextureResized( const Vector2& size ) {}

	private:
		ITexture* m_targetTexture;	
		IRenderer& m_renderer;
		forge::CallbackToken m_onTargetTextureResized;
	};

	struct LightingData
	{
		Vector3 m_ambientLight;
		forge::ArraySpan< const LightData > m_worldLights;
	};

	class IMeshesRenderingPass : public IRenderingPass
	{
	public:
		using IRenderingPass::IRenderingPass;

		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData& lightingData ) = 0;
		virtual void ClearTargetTexture() override;

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

