#include "Fpch.h"
#include "UpdateManager.h"

void forge::UpdateManager::Update()
{
	for( Uint32 i = 0; i < static_cast<Uint32>( BucketType::Count ); ++i )
	{
		UpdateBucket( static_cast< BucketType >( i ) );
	}
}

forge::CallbackToken forge::UpdateManager::RegisterUpdateFunction( BucketType bucket, const forge::Callback<>::TFunc& func )
{
	return m_buckets[ static_cast<Uint32>( bucket ) ].AddListener( func );
}

void forge::UpdateManager::UpdateBucket( BucketType bucket )
{
	m_buckets[ static_cast< Uint32 >( bucket ) ].Invoke();
}
