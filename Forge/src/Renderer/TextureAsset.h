#pragma once
#include "../Core/IAsset.h"

namespace renderer
{
	class ITexture;

	class TextureAsset : public forge::IAsset
	{
	public:
		TextureAsset( const std::string& path, std::unique_ptr< renderer::ITexture >&& texture );

		std::shared_ptr< const renderer::ITexture > GetTexture() const
		{
			return m_texture;
		}

	private:
		std::shared_ptr< renderer::ITexture > m_texture;
	};
}

