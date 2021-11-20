#pragma once

namespace forge
{
	namespace utils
	{
		template< class T >
		FORGE_INLINE typename std::vector< T >::iterator RemoveReorder( std::vector< T >& vec, Uint32 index )
		{
			std::swap( vec[ index ], vec.back() );
			vec.pop_back();
			return vec.begin() + index;
		}

		template< class T >
		FORGE_INLINE typename std::vector< T >::iterator RemoveReorder( std::vector< T >& vec, typename std::vector< T >::iterator it )
		{
			Uint32 index = it - vec.begin();
			return RemoveReorder( vec, index );
		}

		template< class T >
		FORGE_INLINE typename std::vector< T >::iterator RemoveValueReorder( std::vector< T >& vec, const T& val )
		{
			auto it = std::find( vec.begin(), vec.end(), val );
			return RemoveReorder( vec, it );
		}
	}
}