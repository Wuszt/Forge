#pragma once
#include "../Core/IAsset.h"

namespace renderer
{
	class ITexture;
	class Path;

	class TextureAsset : public forge::IAsset
	{
	public:
		TextureAsset( const forge::Path& path, std::unique_ptr< renderer::ITexture >&& texture );

		std::shared_ptr< const renderer::ITexture > GetTexture() const
		{
			return m_texture;
		}

	private:
		std::shared_ptr< renderer::ITexture > m_texture;
	};
}

