#include "fpch.h"
#include "Streams.h"
#include "fstream"

RTTI_IMPLEMENT_TYPE( forge::Stream );

forge::MemoryStream::MemoryStream( Uint64 initialCapacity )
{
	m_buffer.reserve( initialCapacity );
}

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

forge::FileStream::FileStream( const Char* filePath, Bool append, Uint64 bufferSize )
	: m_stream( std::make_unique< std::fstream >() )
{
	m_stream->open( filePath, std::ios::out | std::ios::app );
	m_stream->close();
	m_stream->open( filePath, std::ios::in | std::ios::out | std::ios::binary |  (append ? std::ios::app : 0 ) );

	m_stream->rdbuf()->pubsetbuf( nullptr, bufferSize );
	m_stream->sync_with_stdio( false );
}

forge::FileStream::~FileStream()
{
	m_stream->close();
}

void forge::FileStream::Write( const void* data, Uint64 size )
{
	m_stream->write( reinterpret_cast< const char* >( data ), size );
	m_stream->seekg( m_stream->tellp(), std::ios_base::beg);
}

void forge::FileStream::Read( void* data, Uint64 size )
{
	m_stream->read( reinterpret_cast< char* >( data ), size );
	m_stream->seekp( m_stream->tellg(), std::ios_base::beg );
}

Uint64 forge::FileStream::GetSize() const
{
	Uint64 currentPos = GetPos();

	m_stream->seekg( 0, std::ios::beg );
	auto begin = m_stream->tellg();

	m_stream->seekg( 0, std::ios::end );
	auto end = m_stream->tellg();

	Uint64 size = end - begin;
	m_stream->seekg( currentPos );

	return size;
}

Uint64 forge::FileStream::GetPos() const
{
	FORGE_ASSERT( m_stream->tellg() == m_stream->tellp() );
	return m_stream->tellg();
}

void forge::FileStream::SetPos( Uint64 pos )
{
	m_stream->seekg( pos, std::ios_base::beg );
	m_stream->seekp( pos, std::ios_base::beg );
}
