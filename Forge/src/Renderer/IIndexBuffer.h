#pragma once

namespace renderer
{
	class IIndexBuffer
	{
	public:
		virtual ~IIndexBuffer() {}

		virtual void Set( Uint32 offset ) const = 0;
		virtual Uint32 GetIndicesAmount() const = 0;
	};
}
