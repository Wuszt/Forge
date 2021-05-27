#pragma once
class IIndexBuffer
{
public:
	virtual ~IIndexBuffer() {}

	virtual void Set( Uint32 offset ) = 0;
};

