#pragma once
#include "VolumeSceneObject.h"
#include "../ECS/Tag.h"

namespace forge::ai
{
	class NavmeshVolume : public VolumeSceneObject
	{
		RTTI_DECLARE_CLASS( NavmeshVolume, forge::VolumeSceneObject );

	public:
		struct NavmeshVolumeTag : public ecs::Tag
		{
			RTTI_DECLARE_STRUCT( NavmeshVolumeTag, ecs::Tag );
		};

	protected:
		virtual void OnInit( ObjectInitData& initData ) override;

		virtual LinearColor GetBoundsColor() const override
		{
			return LinearColor( 0.69f, 0.85f, 0.9f, 1.0f );
		}
	};
}