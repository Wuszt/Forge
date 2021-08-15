#pragma once

namespace renderer
{
	class IConstantBufferImpl
	{
	public:
		virtual ~IConstantBufferImpl() {}

		virtual void SetVS( Uint32 index ) = 0;
		virtual void SetPS( Uint32 index ) = 0;
	};

	template< class T >
	class ConstantBuffer
	{
	public:
		void SetImpl( std::unique_ptr< IConstantBufferImpl > impl )
		{
			m_impl = std::move( impl );
		}

		T& GetData()
		{
			return m_data;
		}

		void SetVS( Uint32 index )
		{
			m_impl->SetVS( index );
		}

		void SetPS( Uint32 index )
		{
			m_impl->SetPS( index );
		}

	private:
		std::unique_ptr< IConstantBufferImpl > m_impl;
		T m_data;
	};
}