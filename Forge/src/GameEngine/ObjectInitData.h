#pragma once
#include "../Core/Utils.h"

namespace forge
{
	class ObjectInitData
	{
		RTTI_DECLARE_CLASS( ObjectInitData );

	public:
		void* AddData( const rtti::Type& type );

		template< class T >
		T& AddData()
		{
			return static_cast< T& >( *AddData( T::GetTypeStatic() ) );
		}

		void* GetData( const ::rtti::Type& type );

		template< class T >
		T* GetData()
		{
			return static_cast< T* >( GetData( T::GetTypeStatic() ) );
		}

	private:
		std::vector< forge::InstanceUniquePtr > m_data;
	};
}
