#pragma once

namespace forge
{
	class DepotsContainer;
	class Path;
}

namespace renderer
{
	class ShaderDefine;

	template< class ShaderType >
	class ShaderPack;

	class ShadersManager
	{
	public:
		ShadersManager( const forge::DepotsContainer& depotsContainer );
		virtual ~ShadersManager() = 0;

		void SetBaseShaderDefines( std::vector< ShaderDefine > shaderDefines );
		forge::ArraySpan< const ShaderDefine > GetBaseShaderDefines() const
		{
			return m_baseShaderDefines;
		}

		std::shared_ptr< ShaderPack< IVertexShader > > GetVertexShader( const forge::Path& path, std::vector< ShaderDefine > defines, Bool withSubshaders );
		std::shared_ptr< ShaderPack< IPixelShader > > GetPixelShader( const forge::Path& path, std::vector< ShaderDefine > defines, Bool withSubshaders );

		void ClearCache();

		forge::CallbackToken RegisterCacheClearingListener( const forge::Callback<>::TFunc& func );

	protected:
		virtual std::unique_ptr< IVertexShader > CreateVertexShader( const forge::Path& path, forge::ArraySpan< const ShaderDefine > defines ) const = 0;
		virtual std::unique_ptr< IPixelShader > CreatePixelShader( const forge::Path& path, forge::ArraySpan< const ShaderDefine > defines ) const = 0;

		const forge::DepotsContainer& m_depotsContainer;

	private:
		std::unordered_map< Uint32, std::shared_ptr< ShaderPack< IVertexShader > > > m_vertexShaders;
		std::unordered_map< Uint32, std::shared_ptr< ShaderPack< IPixelShader > > > m_pixelShaders;

		std::vector< ShaderDefine > m_baseShaderDefines;
		forge::Callback<> m_onCacheClear;
	};
}

