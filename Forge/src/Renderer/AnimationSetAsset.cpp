#include "Fpch.h"
#include "AnimationSetAsset.h"

IMPLEMENT_TYPE( renderer::AnimationSetAsset );

renderer::AnimationSetAsset::AnimationSetAsset()
	: forge::IAsset("")
{}

renderer::AnimationSetAsset::AnimationSetAsset( const std::string& path, std::vector< Animation > animations )
	: m_animations( std::move( animations ) )
	, forge::IAsset( path )
{}

std::pair< Uint32, Uint32 > CalculateFramesToInterp( Float time, Float frameRate, Uint32 keysAmount, Bool loop, Float& outT )
{
	Uint32 unclampedFrame = static_cast< Uint32 >( time * frameRate );
	Uint32 unclampedNextFrame = unclampedFrame + 1u;
	outT = time * frameRate - static_cast< Float >( unclampedFrame );

	if( loop )
	{
		return std::make_pair( unclampedFrame % keysAmount, unclampedNextFrame % keysAmount );
	}
	else
	{
		return std::make_pair( Math::Min( unclampedFrame, keysAmount - 1u ), Math::Min( unclampedNextFrame, keysAmount - 1u ) );
	}
}

Matrix InterpolateBetweenKeys( const renderer::Animation::AnimationKey& prev, const renderer::Animation::AnimationKey& next, Float t )
{
	Vector3 finalTranslation = Math::Lerp( prev.m_translation, next.m_translation, t );
	Quaternion finalRotation = Quaternion::Slerp( prev.m_rotation, next.m_rotation, t );

	Matrix m( finalRotation );
	m.SetTranslation( finalTranslation );
	return m;
}

Matrix renderer::Animation::Sample( Float time, Uint32 boneIndex, Bool loop ) const
{
	Float t = 0.0f;
	auto [ frame, nextFrame ] = CalculateFramesToInterp( time, m_frameRate, static_cast< Uint32 >( m_bonesKeys[ 0 ].size() ), loop, t );

	AnimationKey prevKey = m_bonesKeys[ boneIndex ][ frame ];
	AnimationKey nextKey = m_bonesKeys[ boneIndex ][ nextFrame ];

	return InterpolateBetweenKeys( prevKey, nextKey, t );
}

void renderer::Animation::Sample( Float time, Bool loop, std::vector< Matrix >& outTransforms ) const
{
	outTransforms.resize( m_bonesKeys.size() );

	Float t = 0.0f;
	auto [ frame, nextFrame ] = CalculateFramesToInterp( time, m_frameRate, static_cast< Uint32 >( m_bonesKeys[ 0 ].size() ), loop, t );

	for( Uint32 i = 0u; i < m_bonesKeys.size(); ++i )
	{
		renderer::Animation::AnimationKey prevKey = m_bonesKeys[ i ][ frame ];
		renderer::Animation::AnimationKey nextKey = m_bonesKeys[ i ][ nextFrame ];

		outTransforms[ i ] = InterpolateBetweenKeys( prevKey, nextKey, t );
	}
}