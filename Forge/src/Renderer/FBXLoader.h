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
			Matrix m_finalTransformation;
			std::vector<Matrix> m_anim;
		};

		std::unordered_map<std::string, Uint32> m_boneMapping;
		std::vector<BoneInfo> m_boneInfo;

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

