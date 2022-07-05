#pragma once
#include "ConstantBuffer.h"

namespace renderer
{
	class Model;
	class Material;
	class IRenderer;
	class IInputLayout;
	
	struct cbMesh
	{
		Matrix W = Matrix( Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS() );
	};

	class Renderable
	{
	public:
		Renderable( IRenderer& renderer, forge::AssetsManager& assetsManager, const std::string& path );
		~Renderable();

		void SetModel( forge::AssetsManager& assetsManager, const std::string& path );
		const Model& GetModel() const
		{
			return *m_model;
		}

		const std::vector< std::unique_ptr< Material > >& GetMaterials() const
		{
			return m_materials;
		}

		std::vector< std::unique_ptr< Material > >& GetMaterials()
		{
			return m_materials;
		}

		StaticConstantBuffer< cbMesh >& GetCBMesh()
		{
			return m_cbMesh;
		}

		const StaticConstantBuffer< cbMesh >& GetCBMesh() const
		{
			return m_cbMesh;
		}

	private:
		IRenderer& m_renderer;
		std::shared_ptr< Model > m_model;
		std::vector< std::unique_ptr< Material > > m_materials;
		StaticConstantBuffer< cbMesh > m_cbMesh;
	};
}

