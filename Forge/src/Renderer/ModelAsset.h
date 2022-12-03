#pragma once
#include "../Core/IAsset.h"

namespace renderer
{
	class ConstantBuffer;

	class ModelAsset : public forge::IAsset
	{
		DECLARE_POLYMORPHIC_CLASS( ModelAsset, forge::IAsset );
	public:
		struct MaterialData
		{
			std::unique_ptr< renderer::ConstantBuffer > m_buffer;
			std::string m_diffuseTextureName;
			std::string m_normalTextureName;
			std::string m_alphaTextureName;
		};

		ModelAsset();
		ModelAsset( const std::string& name, std::unique_ptr< renderer::Model >&& model, std::vector< MaterialData >&& materialsData )
			: m_model( std::move( model ) )
			, m_materialsData( std::move( materialsData ) )
			, forge::IAsset( name )
		{}

		const std::shared_ptr< renderer::Model > GetModel() const
		{
			return m_model;
		}

		const std::vector< MaterialData >& GetMaterialsData() const
		{
			return m_materialsData;
		}

	private:
		std::shared_ptr< renderer::Model > m_model;
		std::vector< MaterialData > m_materialsData;
	};
}

