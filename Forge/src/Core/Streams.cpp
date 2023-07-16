#include "fpch.h"
#include "Streams.h"
#include "sstream"

forge::MemoryStream::MemoryStream( Uint64 initialCapacity )
{
	m_buffer.reserve( initialCapacity );
}

forge::MemoryStream::~MemoryStream() = default;

void forge::MemoryStream::Write( const void* data, Uint64 size )
{
	m_buffer.resize( m_buffer.size() + size );
	memcpy( &m_buffer[ m_pos ], data, size );
	m_pos += size;
}

void forge::MemoryStream::Read( void* data, Uint64 size )
{
	memcpy( data, &m_buffer[ m_pos ], size );
	m_pos += size;
}

Uint64 forge::MemoryStream::GetPos() const
{
	return m_pos;
}

void forge::MemoryStream::SetPos( Uint64 pos )
{
	m_pos = pos;
}

void forge::MemoryStream::ResetPos()
{
	SetPos( 0u );
}
