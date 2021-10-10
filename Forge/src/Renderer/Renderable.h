#pragma once

namespace renderer
{
	class IMesh;
	class Material;
	class IRenderer;
	class IInputLayout;
	struct cbMesh;

	class Renderable
	{
	public:
		Renderable( IRenderer& renderer );
		~Renderable();

		FORGE_INLINE void SetMesh( std::unique_ptr< const IMesh >&& mesh )
		{
			m_mesh = std::move( mesh );

			if( m_material )
			{
				UpdateInputLayout();
			}
		}

		FORGE_INLINE const IMesh* GetMesh() const
		{
			return m_mesh.get();
		}

		FORGE_INLINE void SetMaterial( std::unique_ptr< Material >&& material )
		{
			m_material = std::move( material );

			if( m_mesh )
			{
				UpdateInputLayout();
			}
		}

		FORGE_INLINE const Material* GetMaterial() const
		{
			return m_material.get();
		}

		FORGE_INLINE Material* GetMaterial()
		{
			return m_material.get();
		}


		FORGE_INLINE const IInputLayout* GetInputLayout() const
		{
			return m_inputLayout.get();
		}

		FORGE_INLINE const renderer::StaticConstantBuffer< renderer::cbMesh >& GetCBMesh() const
		{
			return m_cbMesh;
		}

		FORGE_INLINE renderer::StaticConstantBuffer< renderer::cbMesh >& GetCBMesh()
		{
			return m_cbMesh;
		}

		void UpdateInputLayout();

	private:
		IRenderer& m_renderer;
		std::unique_ptr< const IMesh > m_mesh;
		std::unique_ptr < Material > m_material;
		std::unique_ptr< const IInputLayout > m_inputLayout;
		StaticConstantBuffer< cbMesh > m_cbMesh;
	};
}

