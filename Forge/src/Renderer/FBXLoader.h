#pragma once
#include "../Core/IAssetsLoader.h"
#include "../Core/IAsset.h"

namespace renderer
{
	class TMPAsset : public forge::IAsset
	{
	public:
		struct Bone
		{
			Matrix m_matrix = Matrix::IDENTITY();
			Matrix m_offsetMatrix = Matrix::IDENTITY();
			const Bone* m_parent = nullptr;
			Uint64 m_id = 0u;
			std::string m_name;
			std::vector< Bone > m_children;
			std::vector< Matrix > m_animation;
		};

		using forge::IAsset::IAsset;
		Bone m_rootBone;
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

