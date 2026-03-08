#include "fpch.h"
#include "ApplicationArgs.h"

forge::ApplicationArgs::ApplicationArgs( int argc, char* argv[] )
{
	for ( Int32 index = 1; index < argc; ++index )
	{
		const Char* current = argv[ index ];
		if ( current[ 0 ] == '-' )
		{
			std::string name(current + 1);
			if ( !name.empty() )
			{
				std::string value = "";
				if ( index < argc - 1 )
				{
					const Char* next = argv[ index + 1 ];
					if ( next[ 0 ] != '-' )
					{
						value = next;
						++index;
					}
				}

				m_arguments.emplace( std::move( name ), std::move( value ) );
			}
		}
	}
}

Bool forge::ApplicationArgs::HasArgument( std::string_view argument ) const
{
	return m_arguments.contains( argument );
}

const std::string* forge::ApplicationArgs::GetArgumentValue( std::string_view argument ) const
{
	auto it = m_arguments.find( argument );
	if ( it != m_arguments.end() )
	{
		return &it->second;
	}

	return nullptr;
}