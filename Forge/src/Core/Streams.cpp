#include "fpch.h"
#include "Streams.h"
#include "sstream"

forge::MemoryStream::MemoryStream()
	: m_stream( std::make_unique< std::stringstream >() )
{}

forge::MemoryStream::~MemoryStream() = default;

void forge::MemoryStream::Write( const void* data, Uint64 size )
{
	m_stream->write( reinterpret_cast< const char* >( data ), size );
}

void forge::MemoryStream::Read( void* data, Uint64 size )
{
	m_stream->read( reinterpret_cast< char* >( data ), size );
}

Uint64 forge::MemoryStream::GetReadPos() const
{
	return m_stream->tellg();
}

Uint64 forge::MemoryStream::GetWritePos() const
{
	return m_stream->tellp();
}

void forge::MemoryStream::SetReadPos( Uint64 pos )
{
	m_stream->seekg( pos, std::ios_base::beg );
}

void forge::MemoryStream::SetWritePos( Uint64 pos )
{
	m_stream->seekp( pos, std::ios_base::beg );
}

void forge::MemoryStream::ResetPos()
{
	m_stream->clear();
	SetReadPos( 0u );
	SetWritePos( 0u );
}
