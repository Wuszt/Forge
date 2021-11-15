#pragma once

namespace renderer
{
	class Model;
	class Material;
	class IRenderer;
	class IInputLayout;
	struct cbMesh;

	class Renderable
	{
	public:
		Renderable( IRenderer& renderer, const std::string& path );
		~Renderable();

		void SetModel( const std::string& path );
		const Model& GetModel() const
		{
			return *m_model;
		}

		const std::vector< Material >& GetMaterials() const
		{
			return m_materials;
		}

		std::vector< Material >& GetMaterials()
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
		std::vector< Material > m_materials;
		StaticConstantBuffer< cbMesh > m_cbMesh;
	};
}

