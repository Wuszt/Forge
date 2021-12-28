#pragma once

namespace renderer
{
	class ITexturesLoader
	{
	public:
		virtual std::shared_ptr< renderer::ITexture > LoadTexture( const std::string& path ) = 0;
	};
}

