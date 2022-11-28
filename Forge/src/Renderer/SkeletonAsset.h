#include "../Core/IAsset.h"
#include "IVertexBuffer.h"

namespace renderer
{
	class SkeletonAsset : public forge::IAsset
	{
		DECLARE_CLASS( SkeletonAsset, forge, IAsset );

	public:
		SkeletonAsset();
		SkeletonAsset( const std::string& path, std::vector< renderer::InputBlendWeights > blendWeights, std::vector< renderer::InputBlendIndices > blendIndices, std::vector< Matrix > bonesOffsets );

	//private:
		std::vector< renderer::InputBlendWeights > m_blendWeights;
		std::vector< renderer::InputBlendIndices > m_blendIndices;
		std::vector< Matrix > m_bonesOffsets;
	};
}