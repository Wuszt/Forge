#pragma once
namespace forge
{
	class Stream
	{
	public:
		virtual ~Stream() = default;

		virtual void Write( const void* data, Uint64 size ) = 0;
		virtual void Read( void* data, Uint64 size ) = 0;

		virtual Uint64 GetPos() const = 0;
		virtual void SetPos( Uint64 pos ) = 0;

		virtual void ResetPos()
		{
			SetPos( 0u );
		}

		template< class T >
		void Write( const T& data )
		{
			Write( &data, sizeof( T ) );
		}

		template< class T >
		T Read()
		{
			T data;
			Read( &data, sizeof( T ) );
			return data;
		}
	};

	class MemoryStream : public Stream
	{
	public:
		MemoryStream( Uint64 initialCapacity );

		virtual void Write( const void* data, Uint64 size ) override;
		virtual void Read( void* data, Uint64 size ) override;

		virtual Uint64 GetPos() const override;
		virtual void SetPos( Uint64 pos ) override;

	private:
		std::vector<Byte> m_buffer;
		Uint64 m_pos = 0u;
	};

	class FileStream : public Stream
	{
	public:
		FileStream( const Char* filePath, Bool append );
		~FileStream();

		void Write( const void* data, Uint64 size ) override;
		void Read( void* data, Uint64 size ) override;

		Uint64 GetPos() const override;
		void SetPos( Uint64 pos ) override;

	private:
		std::unique_ptr< std::fstream > m_stream;
	};
}

