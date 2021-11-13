#include "Fpch.h"
#include "IModelsLoader.h"

renderer::IModelsLoader::IModelsLoader( IRenderer& renderer )
	: m_renderer( renderer )
{}

renderer::IModelsLoader::~IModelsLoader() = default;
