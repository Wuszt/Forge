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
		IConstantBuffer() = default;
		IConstantBuffer( IConstantBuffer&& ) = default;
		IConstantBuffer& operator=( IConstantBuffer&& ) = default;
		virtual ~IConstantBuffer() = default;

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

	// WARNING - CB's size must be multiplication of 16 bytes, nothing is checking that right now...
	class ConstantBuffer : public IConstantBuffer
	{
	public:
		ConstantBuffer() = default;

		void CopyDataFrom( const ConstantBuffer& toCopy )
		{
			m_dataLUT = toCopy.m_dataLUT;
			m_offsets = toCopy.m_offsets;
			m_dataSize = toCopy.m_dataSize;
			m_elementsAmount = toCopy.m_elementsAmount;
			m_rawData = std::move( forge::RawSmartPtr( m_dataSize ) );
			memcpy( m_rawData.GetData(), toCopy.m_rawData.GetData(), m_dataSize );

			CreateBuffer();
			UpdateBuffer();
		}

		template< class T >
		void AddData( const std::string& name, const T& data )
		{
			Uint32 offset = sizeof( T );
			Uint32 prevSize = m_dataSize;
			m_dataSize += offset;

			FORGE_ASSURE( m_dataLUT.emplace( name, m_elementsAmount++ ).second );
			m_offsets.emplace_back( prevSize );

			forge::RawSmartPtr prevData = std::move( m_rawData );

			m_rawData = std::move( forge::RawSmartPtr( m_dataSize ) );
			memcpy( m_rawData.GetData(), prevData.GetData(), prevSize );
			memcpy( m_rawData.GetData() + prevSize, &data, offset );

			CreateBuffer();
		}

		template< class T >
		Bool SetData( const std::string& name, const T& data )
		{
			auto it = m_dataLUT.find( name );
			if ( it != m_dataLUT.end() )
			{
				Uint32 offset = m_offsets[ it->second ];

				memcpy( m_rawData.GetData() + offset, &data, sizeof( T ) );
				return true;
			}

			return false;
		}

		template< class T >
		Bool TryToGetData( const std::string& name, T& output ) const
		{
			auto it = m_dataLUT.find( name );
			if ( it != m_dataLUT.end() )
			{
				Uint32 offset = m_offsets[ it->second ];

				memcpy( &output, m_rawData.GetData() + offset, sizeof( T ) );
				return true;
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
			GetImpl()->CreateBuffer( m_dataSize );
		}

	private:
		std::unordered_map< std::string, Uint32 > m_dataLUT;

		std::vector< Uint32 > m_offsets;

		Uint32 m_dataSize = 0u;
		Uint32 m_elementsAmount = 0u;
		forge::RawSmartPtr m_rawData;
	};
}