#pragma once

namespace forge
{
	class PropertiesChain;
}

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
		RTTI_DECLARE_CLASS( ConstantBuffer );

	public:
		ConstantBuffer() = default;
		ConstantBuffer( const ConstantBuffer& toCopy );

		ConstantBuffer& operator=( const ConstantBuffer& toCopy );

		template< class T >
		void AddData( std::string name, const T& data )
		{
			AddData( std::move( name ), rtti::GetTypeInstanceOf< T >(), &data );
		}

		void AddData( std::string name, const rtti::Type& type, const void* data );

		Bool SetData( std::string name, const rtti::Type& type, const void* data );

		template< class T >
		Bool SetData( std::string name, const T& data )
		{
			return SetData( std::move( name ), rtti::GetTypeInstanceOf< T >(), &data );
		}

		void AddOrSetData( std::string name, const rtti::Type& type, const void* data );

		template< class T >
		Bool TryToGetData( const std::string& name, T& output ) const
		{
			Uint32 offset = 0u;
			for ( const Element& element : m_elements )
			{
				if ( element.m_name == name )
				{
					if ( *element.m_type != rtti::GetTypeInstanceOf< T >() )
					{
						return false;
					}

					memcpy( &output, static_cast< Byte* >( m_rawData.GetData() ) + offset, sizeof( T ) );
					return true;
				}

				offset += static_cast< Uint32 >( element.m_type->GetSize() );
			}

			return false;
		}

		template< class TFunc >
		void VisitElements( const TFunc& func )
		{
			Uint32 offset = 0u;
			for ( const Element& el : m_elements )
			{
				Uint8* rawData = static_cast< Uint8* >( GetRawData() );
				func( el.m_name.c_str(), *el.m_type, rawData + offset );
				offset += static_cast< Uint32 >( el.m_type->GetSize() );
			}
		}

		virtual void* GetRawData() override
		{
			return m_rawData.GetData();
		}

		Bool ContainsElement( const Char* name ) const;

		struct Element
		{
			RTTI_DECLARE_STRUCT( Element );

			std::string m_name;
			const rtti::Type* m_type = nullptr;

		private:
			void Serialize( forge::Serializer& serializer ) const;
			void Deserialize( forge::Deserializer& deserializer );
		};

	protected:
		virtual void CreateBuffer() override
		{
			GetImpl()->CreateBuffer( static_cast< Uint32 >( m_rawData.GetSize() ) );
		}

	private:
		void OnPropertyChanged( const forge::PropertiesChain& propertiesChain );

		void Serialize( forge::Serializer& serializer ) const;
		void Deserialize( forge::Deserializer& deserializer );

		std::vector< Element > m_elements;
		forge::UniqueRawPtr m_rawData;

		Uint8 m_currentPadding = 0u;
	};
}