#pragma once
namespace renderer
{
	class IMesh;
	class Material;
	class IRenderer;
	class IInputLayout;

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

		FORGE_INLINE void SetMaterial( std::unique_ptr< const Material >&& material )
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

		FORGE_INLINE const IInputLayout* GetInputLayout() const
		{
			return m_inputLayout.get();
		}

		FORGE_INLINE const renderer::ConstantBuffer* GetConstantBuffer() const
		{
			return m_constantBuffer.get();
		}

		FORGE_INLINE renderer::ConstantBuffer* GetConstantBuffer()
		{
			return m_constantBuffer.get();
		}

		void UpdateInputLayout();

	private:
		IRenderer& m_renderer;
		std::unique_ptr< const IMesh > m_mesh;
		std::unique_ptr < const Material > m_material;
		std::unique_ptr< const IInputLayout > m_inputLayout;
		std::unique_ptr< renderer::ConstantBuffer > m_constantBuffer;
	};
}

