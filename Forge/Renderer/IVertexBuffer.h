#pragma once
//
//template< class T >
//struct IVertex
//{
//	//T m_data;
//
//	virtual Uint32 GetByteWidth() const = 0;
//	virtual const void* GetData() const = 0;
//};
//
//template< class T, class ... Types >
//struct IVertex : IVertex< Types... >
//{
//	//T m_data;
//};

struct IVertex
{};


struct IVertices
{
	~IVertices() {}

	virtual Uint32 GetByteWidth() const = 0;
	virtual const void* GetData() const = 0;
};

class IVertexBuffer
{
public:
	~IVertexBuffer() {}

	virtual void Set() = 0;
};

