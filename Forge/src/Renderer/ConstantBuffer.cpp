#include "Fpch.h"
#include "ConstantBuffer.h"

void renderer::ConstantBuffer::UpdateBuffer()
{
	GetImpl()->UpdateBuffer( m_rawData.m_data );
}
