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
		{}

		const Char* GetExtension() const
		{
			const Uint32 extensionStart = static_cast< Uint32 >( m_path.find_last_of( '.' ) );
			if ( extensionStart == std::string::npos )
			{
				return "";
			}

			return m_path.data() + extensionStart;
		}

		const Char* Get() const
		{
			return m_path.c_str();
		}

		std::string m_path;
	};
}