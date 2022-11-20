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
			Uint32 index = static_cast< Uint32 >( it - vec.begin() );
			return RemoveReorder( vec, index );
		}

		template< class T >
		FORGE_INLINE typename std::vector< T >::iterator RemoveValueReorder( std::vector< T >& vec, const T& val )
		{
			auto it = std::find( vec.begin(), vec.end(), val );
			return RemoveReorder( vec, it );
		}
	}

	class RawSmartPtr
	{
	public:
		RawSmartPtr( Uint64 size = 0u )
		{
			m_size = size;
			m_data = new Byte[ m_size ];
		}

		RawSmartPtr( RawSmartPtr&& ptr )
		{
			m_data = ptr.m_data;
			ptr.m_data = new Byte[ 0 ];
		}

		RawSmartPtr& operator=( RawSmartPtr&& ptr )
		{
			this->~RawSmartPtr();
			m_data = ptr.m_data;
			m_size = ptr.m_size;

			ptr.m_size = 0u;
			ptr.m_data = new Byte[ ptr.m_size ];

			return *this;
		}

		~RawSmartPtr()
		{
			delete[] m_data;
		}

		FORGE_INLINE Byte* GetData() const
		{
			return m_data;
		}

		FORGE_INLINE Uint32 GetSize() const
		{
			return m_size;
		}

	private:
		Uint32 m_size = 0u;
		Byte* m_data = nullptr;
	};
}