#pragma once
#include "../Renderer/ISamplerState.h"

struct ID3D11SamplerState;

namespace d3d11
{
	class D3D11SamplerState : public renderer::ISamplerState
	{
	public:
		D3D11SamplerState( const D3D11Device& device, renderer::SamplerStateFilterType filterType, renderer::SamplerStateComparisonType comparisonType = renderer::SamplerStateComparisonType::ALWAYS );

		FORGE_INLINE ID3D11SamplerState* GetSamplerState() const
		{
			return m_samplerState;
		}

	private:
		ID3D11SamplerState* m_samplerState;
	};
}

