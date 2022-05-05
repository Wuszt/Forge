#pragma once
#include "EngineInstance.h"

namespace forge
{
	class UpdateManager : public IManager
	{
	public:
		enum class BucketType : Uint32
		{
			PreUpdate,
			Update,
			PostUpdate,
			PreRendering,
			Rendering,
			PostRendering,
			Present,
			Count
		};

		using IManager::IManager;

		void Update();

		CallbackToken RegisterUpdateFunction( BucketType bucket, const forge::Callback<>::TFunc& func );

	private:
		void UpdateBucket( BucketType bucket );
		std::array< forge::Callback<>, static_cast< Uint32 >( BucketType::Count ) > m_buckets;
	};
}

