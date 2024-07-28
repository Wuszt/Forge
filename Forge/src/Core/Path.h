#pragma once
namespace forge
{
	class Path
	{
		RTTI_DECLARE_CLASS( Path );

	public:
		Path() = default;
		Path( std::string path )
			: m_path ( std::move( path ) )
		{
			m_extensionStart = static_cast< Uint32 >( m_path.find_last_of( '.' ) );
			m_fileNameStart = static_cast< Uint32 >( m_path.find_last_of( '/' ) );
		}

		std::string GetExtension() const
		{
			FORGE_ASSERT( m_extensionStart > 0 && m_extensionStart < m_path.size() );
			return m_path.substr( m_extensionStart );
		}

		std::string GetFileName() const
		{
			FORGE_ASSERT( m_extensionStart > 0 && m_extensionStart < m_path.size() );
			FORGE_ASSERT( m_fileNameStart > 0 && m_fileNameStart < m_path.size() );
			FORGE_ASSERT( m_extensionStart > m_fileNameStart );
			return m_path.substr( m_fileNameStart, m_extensionStart - m_fileNameStart );
		}

		const Char* Get() const
		{
			return m_path.c_str();
		}

		std::string m_path;
		Uint32 m_extensionStart = static_cast< Uint32 >( -1 );
		Uint32 m_fileNameStart = static_cast< Uint32 >( -1 );
	};
}