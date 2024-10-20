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

	class Serializer;
	class Deserializer;

	class UniqueRawPtr
	{
		RTTI_DECLARE_CLASS( UniqueRawPtr );

	public:
		UniqueRawPtr( Uint64 size = 0u )
		{
			m_size = size;
			m_data = ::operator new( size );
		}

		UniqueRawPtr( UniqueRawPtr&& ptr )
		{
			m_data = ptr.m_data;
			m_size = ptr.m_size;

			ptr.Release();
		}

		UniqueRawPtr& operator=( UniqueRawPtr&& ptr )
		{
			Reset();
			m_data = ptr.m_data;
			m_size = ptr.m_size;

			ptr.m_size = 0u;
			ptr.m_data = ::operator new( 0 );

			return *this;
		}

		~UniqueRawPtr()
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
				::operator delete( m_data );
				m_size = 0u;
			}
		}

		void Release()
		{
			m_data = ::operator new( 0 );
			m_size = 0u;
		}

		void Serialize( forge::Serializer& serializer ) const;
		void Deserialize( forge::Deserializer& deserializer );

	private:
		Uint64 m_size = 0u;
		void* m_data = nullptr;
	};

	class InstanceUniquePtr
	{
		RTTI_DECLARE_CLASS( InstanceUniquePtr );

	public:
		InstanceUniquePtr() = default;
		InstanceUniquePtr( const rtti::Type& type )
			: m_type( &type )
			, m_memory( type.Construct() )
		{}

		template< class T >
		InstanceUniquePtr( T data )
			: m_type( T::GetTypeStatic() )
			, m_memory( m_type->ConstructWithMove( &data ) )
		{}

		~InstanceUniquePtr()
		{
			if ( m_memory )
			{
				FORGE_ASSERT( m_type );
				m_type->Destroy( m_memory );
			}
		}

		const rtti::Type* GetStoredType() const { return m_type; }
		void* GetMemory() {	return m_memory; }
		const void* GetMemory() const {	m_memory; }

		void Serialize( forge::Serializer& serializer ) const;
		void Deserialize( forge::Deserializer& deserializer );

		template< class T >
		T* GetPtr()
		{
			if ( m_type && ( m_type->IsA< T >() || m_type->InheritsFrom< T >() ) )
			{
				return static_cast< T* >( m_memory );
			}

			return nullptr;
		}

		template< class T >
		const T* GetPtr() const
		{
			if ( m_type && ( m_type->IsA< T >() || m_type->InheritsFrom< T >() ) )
			{
				return static_cast< const T* >( m_memory );
			}

			return nullptr;
		}

		template< class T >
		T& Get()
		{
			T* ptr = GetPtr< T >();
			FORGE_ASSERT( ptr );
			return *ptr;
		}

		template< class T >
		const T& Get() const
		{
			const T* ptr = GetPtr< T >();
			FORGE_ASSERT( ptr );
			return *ptr;
		}

	private:
		const rtti::Type* m_type = nullptr;
		void* m_memory = nullptr;
	};
}