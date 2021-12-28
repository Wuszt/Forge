#include "Fpch.h"
#include "D3D11SamplerState.h"
#include <d3d11.h>
#include "D3D11Device.h"

d3d11::D3D11SamplerState::D3D11SamplerState( const D3D11Device& device, renderer::SamplerStateFilterType filterType )
{
	D3D11_SAMPLER_DESC samplerDesc;

	switch( filterType )
	{
	case renderer::SamplerStateFilterType::MIN_MAG_MIP_LINEAR:
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	default:
		FORGE_FATAL( "Unknown filter type" );
	}

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[ 0 ] = 0;
	samplerDesc.BorderColor[ 1 ] = 0;
	samplerDesc.BorderColor[ 2 ] = 0;
	samplerDesc.BorderColor[ 3 ] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	FORGE_ASSURE( device.GetDevice()->CreateSamplerState( &samplerDesc, &m_samplerState ) == S_OK );
}
