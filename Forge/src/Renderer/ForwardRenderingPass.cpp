#include "Fpch.h"
#include "ForwardRenderingPass.h"
#include "LightData.h"
#include "IBlendState.h"

struct CBForwardRendering
{
	Vector4 AmbientLighting;
};

struct CBForwardLighting
{
	renderer::LightData LightingData;
};

namespace
{
	const renderer::ShaderDefine c_ambientLightDefine{ "__AMBIENT_LIGHT__" };
	const renderer::ShaderDefine c_pointLightDefine{ "__POINT_LIGHT__" };
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::ForwardRenderingPass::GetRequiredShaderDefines()
{
	static ShaderDefine shaderDefines[] = { c_ambientLightDefine, c_pointLightDefine };
	return shaderDefines;
}

renderer::ForwardRenderingPass::ForwardRenderingPass( IRenderer& renderer )
	: IMeshesRenderingPass( renderer )
{
	m_cbForwardRendering = GetRenderer().CreateStaticConstantBuffer< CBForwardRendering >();
	m_cbForwardLighting = GetRenderer().CreateStaticConstantBuffer< CBForwardLighting >();

	m_blendState = GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE } );
}

void renderer::ForwardRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture()->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	StaticConstantBuffer< CBForwardRendering >* cbRendering = static_cast<StaticConstantBuffer< CBForwardRendering >*>( m_cbForwardRendering.get() );
	const ShaderDefine* shaderDefine = nullptr;
	if( lightingData )
	{
		cbRendering->GetData().AmbientLighting = lightingData->m_ambientLight;
		shaderDefine = &c_ambientLightDefine;
	}

	cbRendering->UpdateBuffer();
	cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
	cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

	GetRenderer().Draw( rawRenderables, shaderDefine );

	if( lightingData )
	{
		m_blendState->Set();

		StaticConstantBuffer< CBForwardLighting >* cbLighting = static_cast<StaticConstantBuffer< CBForwardLighting >*>( m_cbForwardLighting.get() );
		cbLighting->SetVS( renderer::VSConstantBufferType::Light );
		cbLighting->SetPS( renderer::PSConstantBufferType::Light );

		for( const auto& light : lightingData->m_worldLights )
		{
			cbLighting->GetData().LightingData = light;
			cbLighting->UpdateBuffer();

			GetRenderer().Draw( rawRenderables, &c_pointLightDefine );
		}

		m_blendState->Clear();
	}
}
