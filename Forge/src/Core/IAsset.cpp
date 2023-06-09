#include "fpch.h"
#include "IAsset.h"

RTTI_IMPLEMENT_TYPE( forge::IAsset );

forge::IAsset::IAsset( const std::string& path )
	: m_path( path )
{}
