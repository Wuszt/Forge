#pragma once
#include "IRenderingResource.h"

namespace renderer
{
	class IIndexBuffer : public IRenderingResource
	{
	public:
		virtual void Set( Uint32 offset ) const = 0;
		virtual Uint32 GetIndicesAmount() const = 0;
	};
}
