#pragma once

namespace renderer
{
	template< class ShaderType >
	class ShaderPack
	{
	public:
		ShaderPack( std::shared_ptr< ShaderType > mainShader )
			: m_mainShader( mainShader )
		{}

		ShaderPack( std::shared_ptr< ShaderType > mainShader, std::unordered_map< ShaderDefine, std::shared_ptr< ShaderType > >&& subShaders )
			: m_mainShader( mainShader )
			, m_subShaders( std::move( subShaders ) )
		{}

		std::shared_ptr< const ShaderType > GetMainShader() const
		{
			return m_mainShader;
		}

		const std::unordered_map< ShaderDefine, std::shared_ptr< ShaderType > >& GetSubShaders() const
		{
			return m_subShaders;
		}

	private:
		std::shared_ptr< ShaderType > m_mainShader;
		std::unordered_map< ShaderDefine, std::shared_ptr< ShaderType > > m_subShaders;
	};
}

