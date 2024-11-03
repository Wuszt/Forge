#pragma once

namespace renderer
{
	class IConstantBufferImpl
	{
	public:
		virtual ~IConstantBufferImpl() {}

		virtual void SetVS( Uint32 index ) = 0;
		virtual void SetPS( Uint32 index ) = 0;
		virtual void CreateBuffer( Uint32 dataSize ) = 0;
		virtual void UpdateBuffer( void* data ) = 0;
	};

	enum class VSConstantBufferType
	{
		Invalid = -1,
		Frame = 0,
		Material = 1,
		Mesh = 2,
		Camera = 3,
		RenderingPass = 4,
		Light = 5,
		SkeletalMesh = 6
	};

	enum class PSConstantBufferType
	{
		Invalid = -1,
		Frame = 0,
		Material = 1,
		Mesh = 2,
		Camera = 3,
		RenderingPass = 4,
		Light = 5,
	};

	class IConstantBuffer
	{
	public:
		IConstantBuffer();
		IConstantBuffer( IConstantBuffer&& );
		IConstantBuffer& operator=( IConstantBuffer&& ) = default;
		virtual ~IConstantBuffer() = 0;

		virtual void SetImpl( std::unique_ptr< IConstantBufferImpl > impl )
		{
			m_impl = std::move( impl );
		}

		void SetVS( VSConstantBufferType type ) const
		{
			m_impl->SetVS( static_cast< Uint32 >( type ) );
		}

		void SetPS( PSConstantBufferType type ) const
		{
			m_impl->SetPS( static_cast< Uint32 >( type ) );
		}

		void UpdateBuffer()
		{
			GetImpl()->UpdateBuffer( GetRawData() );
		}

		IConstantBufferImpl* GetImpl() const
		{
			return m_impl.get();
		}

		virtual void* GetRawData() = 0;

	protected:
		virtual void CreateBuffer() = 0;

	private:
		std::unique_ptr< IConstantBufferImpl > m_impl;
	};

	template< class T >
	class StaticConstantBuffer : public IConstantBuffer
	{
		static_assert( sizeof( T ) % 16 == 0, "Constant Buffer's size must be multiplication of 16" );
	public:
		virtual void SetImpl( std::unique_ptr< IConstantBufferImpl > impl ) override
		{
			IConstantBuffer::SetImpl( std::move( impl ) );
			CreateBuffer();
		}

		T& GetData()
		{
			return m_data;
		}

		const T& GetData() const
		{
			return m_data;
		}

		virtual void* GetRawData() override
		{
			return &m_data;
		}

	protected:
		virtual void CreateBuffer()
		{
			GetImpl()->CreateBuffer( sizeof( T ) );
		}

	private:
		T m_data;
	};

	class ConstantBuffer : public IConstantBuffer
	{
	public:
		ConstantBuffer() = default;

		void CopyDataFrom( const ConstantBuffer& toCopy )
		{
			m_elements = std::vector< Element >( toCopy.m_elements );
			m_rawData = std::move( forge::UniqueRawPtr( toCopy.m_rawData.GetSize() ) );
			memcpy( m_rawData.GetData(), toCopy.m_rawData.GetData(), m_rawData.GetSize() );

			CreateBuffer();
			UpdateBuffer();
		}

		template< class T >
		void AddData( std::string name, const T& data )
		{
			const Uint32 offset = sizeof( T );
			const Uint64 prevSize = m_rawData.GetSize() - m_currentPadding;

			FORGE_ASSURE( !ContainsElement( name ) );
			m_elements.push_back( { std::move( name ), rtti::GetTypeInstanceOf< T >() } );

			forge::UniqueRawPtr prevData = std::move( m_rawData );

			const Uint64 newSize = prevData.GetSize() + offset;
			m_currentPadding = 16 - ( newSize % 16 );
			m_currentPadding = m_currentPadding == 16 ? 0 : m_currentPadding;
			m_rawData = std::move( forge::UniqueRawPtr( prevSize + offset + m_currentPadding ) );
			memcpy( m_rawData.GetData(), prevData.GetData(), prevSize );
			memcpy( static_cast< Byte* >( m_rawData.GetData() ) + prevSize, &data, offset );

			CreateBuffer();
		}

		template< class T >
		Bool SetData( const std::string& name, const T& data )
		{
			Uint32 offset = 0u;
			for ( const Element& element : m_elements )
			{
				if ( element.m_name == name )
				{
					if ( element.m_type != rtti::GetTypeInstanceOf< T >() )
					{
						return false;
					}

					memcpy( static_cast< Byte* >( m_rawData.GetData() ) + offset, &data, sizeof( T ) );
					return true;
				}

				offset += static_cast< Uint32 >( element.m_type.GetSize() );
			}

			return false;
		}

		template< class T >
		Bool TryToGetData( const std::string& name, T& output ) const
		{
			Uint32 offset = 0u;
			for ( const Element& element : m_elements )
			{
				if ( element.m_name == name )
				{
					if ( element.m_type != rtti::GetTypeInstanceOf< T >() )
					{
						return false;
					}

					memcpy( &output, static_cast< Byte* >( m_rawData.GetData() ) + offset, sizeof( T ) );
					return true;
				}

				offset += static_cast< Uint32 >( element.m_type.GetSize() );
			}

			return false;
		}

		virtual void* GetRawData() override
		{
			return m_rawData.GetData();
		}

	protected:
		virtual void CreateBuffer() override
		{
			GetImpl()->CreateBuffer( static_cast< Uint32 >( m_rawData.GetSize() ) );
		}

	private:
		Bool ContainsElement( const std::string& name ) const;

		struct Element
		{
			std::string m_name;
			const rtti::Type& m_type;
		};

		std::vector< Element > m_elements;
		forge::UniqueRawPtr m_rawData;

		Uint8 m_currentPadding = 0u;
	};
}