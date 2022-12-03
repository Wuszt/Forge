#include "../Core/IAsset.h"

namespace renderer
{
	struct Animation
	{
		Animation( Float framerate, Uint32 bonesAmount )
			: m_frameRate( framerate )
			, m_bonesKeys( bonesAmount )
		{}

		Float GetDuration() const
		{
			return static_cast< Float >( m_bonesKeys.size() ) / m_frameRate;
		}

		Float GetFrameRate() const
		{
			return m_frameRate;
		}

		struct AnimationKey
		{
			Vector3 m_translation;
			Quaternion m_rotation;

			Matrix ToMatrix() const
			{
				Matrix m( m_rotation );
				m.SetTranslation( m_translation );
				return m;
			}
		};

		Matrix Sample( Float time, Uint32 boneIndex, Bool loop ) const;
		void Sample( Float time, Bool loop, std::vector< Matrix >& outTransforms ) const;

		std::vector< AnimationKey >& GeyKeys( Uint32 boneIndex )
		{
			return m_bonesKeys[ boneIndex ];
		}

		AnimationKey& GetAnimationKey( Uint32 boneIndex, Uint32 frame )
		{
			return m_bonesKeys[ boneIndex ][ frame ];
		}

	private:
		Float m_frameRate = 0.0f;
		std::vector< std::vector< AnimationKey > > m_bonesKeys;
	};

	class AnimationSetAsset : public forge::IAsset
	{
		DECLARE_POLYMORPHIC_CLASS( AnimationSetAsset, forge::IAsset );

	public:
		AnimationSetAsset();
		AnimationSetAsset( const std::string& path, std::vector< Animation > animation );

		const std::vector< Animation >& GetAnimations() const
		{
			return m_animations;
		}

	private:
		std::vector< Animation > m_animations;
	};
}