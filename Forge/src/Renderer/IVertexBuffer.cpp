#include "Fpch.h"
#include "IVertexBuffer.h"

renderer::Vertices renderer::VerticesBuilder::Build()
{
	return Vertices( *this );
}