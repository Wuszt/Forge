#include "Fpch.h"
#include "D3D11BlendState.h"
#include "D3D11Device.h"

D3D11_BLEND GetAsD3D11Blend( renderer::BlendOperand operand )
{
	switch( operand )
	{
	case renderer::BlendOperand::BLEND_ZERO:
		return D3D11_BLEND_ZERO;

	case renderer::BlendOperand::BLEND_ONE:
		return D3D11_BLEND_ONE;

	case renderer::BlendOperand::BLEND_SRC_COLOR:
		return D3D11_BLEND_SRC_COLOR;

	case renderer::BlendOperand::BLEND_INV_SRC_COLOR:
		return D3D11_BLEND_INV_SRC_COLOR;

	case renderer::BlendOperand::BLEND_SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;

	case renderer::BlendOperand::BLEND_INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;

	case renderer::BlendOperand::BLEND_DEST_ALPHA:
		return D3D11_BLEND_DEST_ALPHA;

	case renderer::BlendOperand::BLEND_INV_DEST_ALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;

	case renderer::BlendOperand::BLEND_DEST_COLOR:
		return D3D11_BLEND_DEST_COLOR;

	case renderer::BlendOperand::BLEND_INV_DEST_COLOR:
		return D3D11_BLEND_INV_DEST_COLOR;

	case renderer::BlendOperand::BLEND_SRC_ALPHA_SAT:
		return D3D11_BLEND_SRC_ALPHA_SAT;

	case renderer::BlendOperand::BLEND_BLEND_FACTOR:
		return D3D11_BLEND_BLEND_FACTOR;

	case renderer::BlendOperand::BLEND_INV_BLEND_FACTOR:
		return D3D11_BLEND_INV_BLEND_FACTOR;

	case renderer::BlendOperand::BLEND_SRC1_COLOR:
		return D3D11_BLEND_SRC1_COLOR;

	case renderer::BlendOperand::BLEND_INV_SRC1_COLOR:
		return D3D11_BLEND_INV_SRC1_COLOR;

	case renderer::BlendOperand::BLEND_SRC1_ALPHA:
		return D3D11_BLEND_SRC1_ALPHA;

	case renderer::BlendOperand::BLEND_INV_SRC1_ALPHA:
		return D3D11_BLEND_INV_SRC1_ALPHA;

	default:
		FORGE_FATAL( "Unknown operand" );
	}

	return D3D11_BLEND_ZERO;
}

D3D11_BLEND_OP GetAsD3D11BlendOp( renderer::BlendOperation operation )
{
	switch( operation )
	{
	case renderer::BlendOperation::BLEND_OP_ADD:
		return D3D11_BLEND_OP_ADD;

	case renderer::BlendOperation::BLEND_OP_SUBTRACT:
		return D3D11_BLEND_OP_SUBTRACT;

	case renderer::BlendOperation::BLEND_OP_REV_SUBTRACT:
		return D3D11_BLEND_OP_REV_SUBTRACT;

	case renderer::BlendOperation::BLEND_OP_MIN:
		return D3D11_BLEND_OP_MIN;

	case renderer::BlendOperation::BLEND_OP_MAX:
		return D3D11_BLEND_OP_MAX;

	default:
		FORGE_FATAL( "Unknown operation" );
	}

	return D3D11_BLEND_OP_ADD;
}

d3d11::D3D11BlendState::D3D11BlendState( d3d11::D3D11Device& device, d3d11::D3D11RenderContext& context, const renderer::BlendOperationDesc& rgbOperation, const renderer::BlendOperationDesc& alphaOperation )
	: m_context( context )
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof( rtbd ) );
	rtbd.BlendEnable = true;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	rtbd.SrcBlend = GetAsD3D11Blend( rgbOperation.m_source );
	rtbd.BlendOp = GetAsD3D11BlendOp( rgbOperation.m_operation );
	rtbd.DestBlend = GetAsD3D11Blend( rgbOperation.m_destination );

	rtbd.SrcBlendAlpha = GetAsD3D11Blend( alphaOperation.m_source );
	rtbd.BlendOpAlpha = GetAsD3D11BlendOp( alphaOperation.m_operation );
	rtbd.DestBlendAlpha = GetAsD3D11Blend( alphaOperation.m_destination );

	blendDesc.RenderTarget[0] = rtbd;

	FORGE_ASSURE( device.GetDevice()->CreateBlendState( &blendDesc, &m_blendState ) == S_OK );
}

d3d11::D3D11BlendState::~D3D11BlendState()
{
	m_blendState->Release();
}

void d3d11::D3D11BlendState::Set()
{
	m_context.GetDeviceContext()->OMSetBlendState( m_blendState, Vector4::ZEROS().AsArray(), 0xffffffff );
}

void d3d11::D3D11BlendState::Clear()
{
	m_context.GetDeviceContext()->OMSetBlendState( nullptr, Vector4::ZEROS().AsArray(), 0xffffffff );
}
