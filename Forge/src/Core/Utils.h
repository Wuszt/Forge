#pragma once

namespace forge
{
	namespace utils
	{
		template< class T >
		typename std::vector< T >::iterator RemoveReorder( std::vector< T >& vec, Uint32 index )
		{
			std::swap( vec[ index ], vec.back() );
			vec.pop_back();
			return vec.begin() + index;
		}

		template< class T >
		typename std::vector< T >::iterator RemoveReorder( std::vector< T >& vec, typename std::vector< T >::iterator it )
		{
			Uint32 index = static_cast< Uint32 >( it - vec.begin() );
			return RemoveReorder( vec, index );
		}

		template< class T >
		typename std::vector< T >::iterator RemoveValueReorder( std::vector< T >& vec, const T& val )
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
			m_size = ptr.m_size;

			ptr.m_data = new Byte[ 0 ];
			ptr.m_size = 0u;
		}

		RawSmartPtr& operator=( RawSmartPtr&& ptr )
		{
			Reset();
			m_data = ptr.m_data;
			m_size = ptr.m_size;

			ptr.m_size = 0u;
			ptr.m_data = new Byte[ 0u ];

			return *this;
		}

		~RawSmartPtr()
		{
			Reset();
		}

		void* GetData() const
		{
			return m_data;
		}

		Uint64 GetSize() const
		{
			return m_size;
		}

		void Reset()
		{
			if( m_size > 0u )
			{
				delete[] m_data;
				m_size = 0u;
			}
		}

	private:
		Uint64 m_size = 0u;
		Byte* m_data = nullptr;
	};
}