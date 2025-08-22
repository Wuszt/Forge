#pragma once
#include "ConstantBuffer.h"
#include "Renderer.h"

namespace forge
{
	class Path;
}

namespace renderer
{
	class Model;
	class Material;
	class Renderer;
	class IInputLayout;
	
	struct cbMesh
	{
		Matrix W = Matrix( Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS(), Vector4::ZEROS() );
	};

	class Renderable
	{
		RTTI_DECLARE_CLASS( Renderable );

	public:
		Renderable();
		~Renderable();

		Renderable( Renderable&& );
		Renderable& operator=( Renderable&& );

		void Initialize( renderer::Renderer& renderer );

		void SetModel( Renderer& renderer, forge::AssetsManager& assetsManager, const forge::Path& path );
		void SetModel( Model&& model );

		void SetDefaultMaterial( Renderer& renderer );

		const Model& GetModel() const
		{
			return *m_model;
		}

		void SetFillMode( FillMode mode )
		{
			m_fillMode = mode;
		}

		FillMode GetFillMode() const
		{
			return m_fillMode;
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

		struct AdditionalConstantBuffer
		{
			renderer::VSConstantBufferType m_vsBufferType = VSConstantBufferType::Invalid;
			renderer::PSConstantBufferType m_psBufferType = PSConstantBufferType::Invalid;
			IConstantBufferImpl* m_cbImpl = nullptr;

			Bool operator==( const AdditionalConstantBuffer& other ) const
			{
				return m_vsBufferType == other.m_vsBufferType
					&& m_psBufferType == other.m_psBufferType
					&& m_cbImpl == other.m_cbImpl;
			}
		};

		void AddConstantBuffer( const AdditionalConstantBuffer& cb )
		{
			FORGE_ASSERT( std::find( m_additionalCBs.begin(), m_additionalCBs.end(), cb ) == m_additionalCBs.end() );
			m_additionalCBs.emplace_back( cb );
		}

		void RemoveConstantBuffer( const AdditionalConstantBuffer& cb )
		{
			forge::utils::RemoveValueReorder( m_additionalCBs, cb );
		}

		forge::ArraySpan< const AdditionalConstantBuffer > GetAdditionalCBs() const
		{
			return m_additionalCBs;
		}

	private:
		std::shared_ptr< Model > m_model;
		std::vector< std::unique_ptr< Material > > m_materials;
		StaticConstantBuffer< cbMesh > m_cbMesh;
		FillMode m_fillMode = FillMode::Solid;

		std::vector< AdditionalConstantBuffer > m_additionalCBs;
	};
}

