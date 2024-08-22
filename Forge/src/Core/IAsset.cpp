#include "fpch.h"
#include "IAsset.h"

RTTI_IMPLEMENT_TYPE( forge::IAsset );

forge::IAsset::IAsset( const forge::Path& path )
	: m_path( path )
{}