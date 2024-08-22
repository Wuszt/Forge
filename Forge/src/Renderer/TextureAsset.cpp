#include "Fpch.h"
#include "TextureAsset.h"

renderer::TextureAsset::TextureAsset( const forge::Path& path, std::unique_ptr< renderer::ITexture >&& texture )
	: m_texture( std::move( texture ) )
	, IAsset( path )
{}
