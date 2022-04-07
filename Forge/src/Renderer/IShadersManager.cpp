#include "Fpch.h"
#include "IShadersManager.h"
#include "IVertexShader.h"
#include "IPixelShader.h"
#include "IShader.h"
#include "ShaderPack.h"

namespace renderer
{
	IShadersManager::IShadersManager() = default;
	IShadersManager::~IShadersManager() = default;

	void IShadersManager::SetBaseShaderDefines( std::vector< ShaderDefine > shaderDefines )
	{
		m_baseShaderDefines = std::move( shaderDefines );

		for( auto it = shaderDefines.begin(); it != shaderDefines.end(); ++it )
		{
			m_baseShaderDefines.erase( std::remove( it + 1, shaderDefines.end(), *it ), m_baseShaderDefines.end() );
		}

		ClearCache();
	}

	Uint32 CalcShaderHash( const std::string& path, forge::ArraySpan< const ShaderDefine > defines )
	{
		Uint64 result = Math::CalculateHash( path );
		for( auto& define : defines )
		{
			result = Math::CombineHashes( result, Math::CombineHashes( Math::CalculateHash( define.m_name ), Math::CalculateHash( define.m_define ) ) );
		}

		return static_cast<Uint32>( result );
	}

	std::shared_ptr< ShaderPack< IVertexShader > > IShadersManager::GetVertexShader( const std::string& path, std::vector< ShaderDefine > defines, Bool withSubshaders )
	{
		FORGE_ASSERT( !withSubshaders || std::count_if( m_baseShaderDefines.begin(), m_baseShaderDefines.end(), [ &defines ]( const ShaderDefine& define ) { return std::count( defines.begin(), defines.end(), define ) > 0u; } ) == 0u );

		Uint32 shaderHash = CalcShaderHash( path, defines );
		auto& shaderPack = m_vertexShaders[ shaderHash ];

		if( !shaderPack )
		{
			std::shared_ptr< IVertexShader > mainShader = CreateVertexShader( path, defines );

			std::unordered_map< ShaderDefine, std::shared_ptr< IVertexShader > > subShaders;
			if( withSubshaders )
			{
				defines.emplace_back();
				for( auto baseShaderDefine : m_baseShaderDefines )
				{
					*defines.rbegin() = baseShaderDefine;
					subShaders.emplace( baseShaderDefine, CreateVertexShader( path, defines ) );
				}
			}

			shaderPack = std::make_shared< ShaderPack< IVertexShader > >( mainShader, std::move( subShaders ) );
		}

		FORGE_ASSERT( shaderPack->GetSubShaders().empty() || withSubshaders );

		return shaderPack;
	}

	std::shared_ptr< ShaderPack< IPixelShader > > IShadersManager::GetPixelShader( const std::string& path, std::vector< ShaderDefine > defines, Bool withSubshaders )
	{
		Uint32 shaderHash = CalcShaderHash( path, defines );
		auto& shaderPack = m_pixelShaders[ shaderHash ];

		if( !shaderPack )
		{
			std::shared_ptr< IPixelShader > mainShader = CreatePixelShader( path, defines );

			std::unordered_map< ShaderDefine, std::shared_ptr< IPixelShader > > subShaders;
			if( withSubshaders )
			{
				defines.emplace_back();
				for( auto baseShaderDefine : m_baseShaderDefines )
				{
					*defines.rbegin() = baseShaderDefine;
					subShaders.emplace( baseShaderDefine, CreatePixelShader( path, defines ) );
				}
			}

			shaderPack = std::make_shared< ShaderPack< IPixelShader > >( mainShader, std::move( subShaders ) );
		}

		FORGE_ASSERT( shaderPack->GetSubShaders().empty() || withSubshaders );

		return shaderPack;
	}

	void IShadersManager::ClearCache()
	{
		m_vertexShaders.clear();
		m_pixelShaders.clear();

		m_onCacheClear.Invoke();
	}

	forge::CallbackToken IShadersManager::RegisterCacheClearingListener( const forge::Callback<>::TFunc& func )
	{
		return m_onCacheClear.AddListener( func );
	}
}
