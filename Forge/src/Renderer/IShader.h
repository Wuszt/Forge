#pragma once
#include "IRenderingResource.h"

namespace renderer
{
	class ShaderDefine
	{
		RTTI_DECLARE_CLASS( ShaderDefine )

	public:
		ShaderDefine() = default;

		ShaderDefine( const ShaderDefine& shaderDefine ) = default;

		ShaderDefine( std::string name, std::string define = "")
			: m_name( std::move( name ) )
			, m_define( std::move( define ) )
		{
			UpdateHash();
		}

		Bool operator==( const ShaderDefine& define ) const
		{
			return m_hash == define.m_hash;
		}

		ShaderDefine& operator=( const ShaderDefine& shaderDefine ) = default;

		Bool IsValid() const
		{
			return !m_name.empty();
		}

		const std::string& GetName() const
		{
			return m_name;
		}

		const std::string& GetDefine() const
		{
			return m_define;
		}

		Uint64 GetHash() const
		{
			return m_hash;
		}

	private:
		void UpdateHash()
		{
			m_hash = Math::CombineHashes( Math::CalculateHash( m_name ), Math::CalculateHash( m_define ) );
		}

		void PostDeserialize() { UpdateHash(); }

		std::string m_name;
		std::string m_define;
		Uint64 m_hash = 0u;
	};

	class IShader : public IRenderingResource
	{
	};
}

namespace std
{
	template <>
	struct hash<renderer::ShaderDefine>
	{
		Uint64 operator()( const renderer::ShaderDefine& sd ) const
		{			
			return sd.GetHash();
		}
	};
}