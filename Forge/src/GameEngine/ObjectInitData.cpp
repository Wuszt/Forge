#include "fpch.h"
#include "ObjectInitData.h"

void* forge::ObjectInitData::AddData( const rtti::Type& type )
{
	FORGE_ASSERT( GetData( type ) == nullptr );
	m_data.emplace_back( type );
	return m_data.back().GetMemory();
}

void forge::ObjectInitData::RemoveData( const rtti::Type& type )
{
	auto it = std::find_if( m_data.begin(), m_data.end(), [ &type ]( const auto& data ) { return data.GetStoredType() == &type; } );
	if ( it != m_data.end() )
	{
		forge::utils::RemoveReorder( m_data, it );
	}
}

void* forge::ObjectInitData::GetData( const ::rtti::Type& type )
{
	auto it = std::find_if( m_data.begin(), m_data.end(), [ &type ]( const auto& data ) { return data.GetStoredType() == &type; } );
	if ( it != m_data.end() )
	{
		return it->GetMemory();
	}

	return nullptr;
}
