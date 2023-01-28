#include "../Core/IAsset.h"
#include "IVertexBuffer.h"

namespace renderer
{
	class SkeletonAsset : public forge::IAsset
	{
		DECLARE_POLYMORPHIC_CLASS( SkeletonAsset, forge::IAsset );

	public:
		SkeletonAsset();
		SkeletonAsset( const std::string& path, std::vector< Matrix > bonesOffsets );

		forge::ArraySpan< const Matrix > GetBonesOffsets() const
		{
			return m_bonesOffsets;
		}

	private:
		std::vector< Matrix > m_bonesOffsets;
	};
}