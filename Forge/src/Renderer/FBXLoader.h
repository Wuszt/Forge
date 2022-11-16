#pragma once
#include "../Core/IAssetsLoader.h"
#include "../Core/IAsset.h"

namespace renderer
{
	class TMPAsset : public forge::IAsset
	{
	public:
		using forge::IAsset::IAsset;

		struct BoneInfo
		{
			Matrix m_boneOffset;
			std::vector< Vector3 > m_translationAnim;
			std::vector< Quaternion > m_rotationAnim;
		};

		std::vector< BoneInfo > m_boneInfo;

		Float m_animDuration = 0.0f;

		struct BoneWeights
		{
			Uint32 m_boneIDs[10u] = { 0u };
			Float m_weights[10u] = { 0.0f };
		};

		std::vector<BoneWeights> m_bones;
	};

	class FBXLoader : public forge::IAssetsLoader
	{

	public:
		FBXLoader( renderer::IRenderer& renderer );
		virtual std::vector< std::shared_ptr< forge::IAsset > > LoadAssets( const std::string& path ) const override;
		virtual forge::ArraySpan<const char *> GetHandledExtensions() const override;

	private:
		IRenderer& m_renderer;
	};
}

