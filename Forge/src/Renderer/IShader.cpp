#include "Fpch.h"
#include "IShader.h"

RTTI_IMPLEMENT_TYPE( renderer::ShaderDefine,
	RTTI_REGISTER_PROPERTY( m_name );
	RTTI_REGISTER_PROPERTY( m_define );
	RTTI_REGISTER_METHOD( PostDeserialize );
);