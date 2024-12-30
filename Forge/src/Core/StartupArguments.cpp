#include "fpch.h"
#include "StartupArguments.h"

Bool IsKey( const Char* arg )
{
	return arg[ 0 ] == '-';
}

forge::StartupArguments::StartupArguments( Int32 argc, Char* argv[] )
{
	m_args.reserve( argc );

	for ( Uint32 i = 0; i < static_cast< Uint32 >( argc ); i += 2 )
	{
		const Char* key = argv[ i ];
		const Char* value = ( i + 1 < static_cast< Uint32 >( argc ) ) ? argv[ i + 1 ] : "";
		if ( !IsKey( key ) )
		{
			i -= 1;
			continue;
		}

		if ( IsKey( value ) )
		{
			i -= 1;
			value = "";
		}

		m_args.push_back( { key + 1, value } );
	}
}

Bool forge::StartupArguments::Contains( const Char* key ) const
{
	return GetValue( key );
}

const Char* forge::StartupArguments::GetValue( const Char* key ) const
{
	auto it = std::find_if( m_args.begin(), m_args.end(), [key]( const Argument& arg )
		{
			return arg.m_key == key;
		} );

	if ( it != m_args.end() )
	{
		return it->m_value.c_str();
	}

	return nullptr;
}
