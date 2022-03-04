#pragma once

namespace renderer
{
	struct ShaderDefine
	{
		std::string m_name;
		std::string m_define;

		Bool operator==( const ShaderDefine& define ) const
		{
			return define.m_define == m_define && m_name == define.m_name;
		}
	};

	class IShader
	{
	public:
		virtual ~IShader() = default;
	};
}

namespace std
{
	template <>
	struct hash<renderer::ShaderDefine>
	{
		Uint64 operator()( const renderer::ShaderDefine& sd ) const
		{			
			return Math::CombineHashes( Math::CalculateHash( sd.m_name ), Math::CalculateHash( sd.m_define ) );
		}
	};
}