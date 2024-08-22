#include "Fpch.h"
#include "SkeletonAsset.h"

RTTI_IMPLEMENT_TYPE( renderer::SkeletonAsset );

renderer::SkeletonAsset::SkeletonAsset()
	: forge::IAsset( {} )
{}

renderer::SkeletonAsset::SkeletonAsset( const forge::Path& path, std::vector< Matrix > bonesOffsets )
	: m_bonesOffsets( std::move( bonesOffsets ) )
	, IAsset( path )
{}