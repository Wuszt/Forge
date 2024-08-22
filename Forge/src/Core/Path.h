#pragma once
namespace forge
{
	class Path
	{
		RTTI_DECLARE_CLASS( Path );

	public:
		Path() = default;
		explicit Path( std::string path )
			: m_path ( std::move( path ) )
		{
			UpdateIndices();
		}

		explicit Path( const Char* path )
			: m_path( path )
		{
			UpdateIndices();
		}

		bool operator==( const Path& ) const = default;

		std::string GetExtension() const
		{
			FORGE_ASSERT( m_extensionStart > 0 && m_extensionStart < m_path.size() );
			return m_path.substr( m_extensionStart + 1 );
		}

		std::string GetFileName( Bool withExtension ) const
		{
			if ( IsFile() )
			{
				FORGE_ASSERT( m_extensionStart < m_path.size() );
				FORGE_ASSERT( m_fileNameStart < m_path.size() );
				FORGE_ASSERT( m_extensionStart > m_fileNameStart );

				if ( withExtension )
				{
					return m_path.substr( m_fileNameStart + 1 );
				}
				else
				{
					return m_path.substr( m_fileNameStart + 1, m_extensionStart - m_fileNameStart );
				}
			}

			return {};
		}

		forge::Path GetFolder() const
		{
			if ( !IsFile() )
			{
				return *this;
			}

			return forge::Path( m_path.substr( 0u, m_fileNameStart ) );
		}

		const Char* Get() const
		{
			return m_path.c_str();
		}

		const std::string& AsString() const
		{
			return m_path;
		}

		void Append( const forge::Path& path )
		{
			FORGE_ASSERT( m_path.empty() || m_path[ m_path.size() - 1] == '\\' );
			m_path += path.AsString();
			UpdateIndices();
		}

		Uint64 GetHash() const
		{
			return std::hash< std::string >()( m_path );
		}

		Bool IsFile() const
		{
			return m_extensionStart != static_cast< Uint32 >( -1 );
		}

		Bool IsEmpty() const
		{
			return m_path.empty();
		}

	private:
		void UpdateIndices()
		{
			m_extensionStart = static_cast< Uint32 >( m_path.find_last_of( '.' ) );

			if ( m_extensionStart < m_path.size() )
			{
				m_fileNameStart = static_cast< Uint32 >( m_path.find_last_of( '\\' ) );
			}
			else
			{
				m_fileNameStart = static_cast< Uint32 >( -1 );
			}
			

			if ( !IsFile() && !m_path.empty() && m_path[ m_path.size() - 1 ] != '\\' )
			{
				 m_path += '\\';
			}
		}

		std::string m_path;
		Uint32 m_extensionStart = static_cast< Uint32 >( -1 );
		Uint32 m_fileNameStart = static_cast< Uint32 >( -1 );
	};
}

namespace std
{
	template <>
	struct hash< forge::Path >
	{
		size_t operator() ( const forge::Path& path ) const { return path.GetHash(); }
	};
}