#include "Fpch.h"
#include "ModelAsset.h"

RTTI_IMPLEMENT_TYPE( renderer::ModelAsset );

renderer::ModelAsset::ModelAsset()
	: IAsset( {} )
{}

renderer::ModelAsset::MaterialData::MaterialData() = default;
renderer::ModelAsset::MaterialData::MaterialData( MaterialData&& ) = default;
renderer::ModelAsset::MaterialData::~MaterialData() = default;