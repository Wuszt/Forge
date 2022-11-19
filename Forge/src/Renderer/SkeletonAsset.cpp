#include "Fpch.h"
#include "SkeletonAsset.h"

renderer::SkeletonAsset::SkeletonAsset( const std::string& path, std::vector< renderer::InputBlendWeights > blendWeights, std::vector< renderer::InputBlendIndices > blendIndices, std::vector< Matrix > bonesOffsets )
	: m_blendWeights( std::move( blendWeights ) )
	, m_blendIndices( std::move( blendIndices ) )
	, m_bonesOffsets( std::move( bonesOffsets ) )
	, IAsset( path )
{}