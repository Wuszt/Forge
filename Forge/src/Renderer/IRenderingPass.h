#pragma once

namespace renderer
{
	class IRenderTargetView;
	struct IRawRenderablesPack;
	struct PointLightData;
	struct SpotLightData;
	struct DirectionalLightData;
	struct ShaderDefine;
	class ITexture;
	class IDepthStencilBuffer;
	class IDepthStencilView;
	class ICamera;

	template< class T >
	struct LightData;

	template< class T >
	class StaticConstantBuffer;

	struct CBCamera
	{
		Matrix VP = Matrix( Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS() );
		Vector3 CameraPosition;
		Float ProjectionA;
		Vector3 CameraDirection;
		Float ProjectionB;
		Float NearPlane;
		Float FarPlane;
		Float padding[2];
	};

	class IRenderingPass
	{
	public:
		IRenderingPass( IRenderer& renderer );
		virtual ~IRenderingPass() = default;

		virtual void ClearTargetTexture();

		virtual void SetTargetTexture( ITexture& targetTexture );

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
		forge::ArraySpan< LightData< PointLightData > > m_pointLights;
		forge::ArraySpan< LightData< SpotLightData > > m_spotLights;
		forge::ArraySpan< LightData< DirectionalLightData > > m_directionalLights;
	};

	class IMeshesRenderingPass : public IRenderingPass
	{
	public:
		IMeshesRenderingPass( IRenderer& renderer );

		virtual void Draw( const renderer::ICamera& camera, const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData ) = 0;
		virtual void ClearTargetTexture() override;

		FORGE_INLINE void SetDepthStencilBuffer( IDepthStencilBuffer* depthStencilBuffer, Uint32 dsvIndex = 0u )
		{
			m_dsvIndex = dsvIndex;
			m_depthStencilBuffer = depthStencilBuffer;
		}

		FORGE_INLINE IDepthStencilBuffer* GetDepthStencilBuffer() const
		{
			return m_depthStencilBuffer;
		}

	protected:
		IDepthStencilView& GetDepthStencilView() const;

		void AdjustViewportSize();
		void UpdateCameraConstantBuffer( const renderer::ICamera& camera );

	private:
		Uint32 m_dsvIndex = 0u;
		IDepthStencilBuffer* m_depthStencilBuffer = nullptr;
		std::unique_ptr< renderer::StaticConstantBuffer< CBCamera > > m_cameraCB;
	};
}

