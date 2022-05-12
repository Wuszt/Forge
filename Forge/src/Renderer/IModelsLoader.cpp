#include "Fpch.h"
#include "IModelsLoader.h"

renderer::IModelsLoader::IModelsLoader( const forge::DepotsContainer& depotsContainer, IRenderer& renderer )
	: m_depotsContainer( depotsContainer )
	, m_renderer( renderer )
{}

renderer::IModelsLoader::~IModelsLoader() = default;
