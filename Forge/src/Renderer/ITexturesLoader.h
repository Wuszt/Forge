#pragma once

namespace renderer
{
	class ITexturesLoader
	{
	public:
		ITexturesLoader( const forge::DepotsContainer& depotsContainer );
		virtual std::shared_ptr< renderer::ITexture > LoadTexture( const std::string& path ) = 0;

	protected:
		const forge::DepotsContainer& m_depotsContainer;
	};
}

