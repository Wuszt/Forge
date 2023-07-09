#pragma once
namespace forge
{
	class Stream
	{
	public:
		virtual ~Stream() = default;

		virtual void Write( const void* data, Uint64 size ) = 0;
		virtual void Read( void* data, Uint64 size ) = 0;

		virtual Uint64 GetReadPos() const = 0;
		virtual Uint64 GetWritePos() const = 0;

		virtual void SetReadPos( Uint64 pos ) = 0;
		virtual void SetWritePos( Uint64 pos ) = 0;

		virtual void ResetPos() = 0;

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
		MemoryStream();
		~MemoryStream();

		virtual void Write( const void* data, Uint64 size ) override;
		virtual void Read( void* data, Uint64 size ) override;

		virtual Uint64 GetReadPos() const override;
		virtual Uint64 GetWritePos() const override;

		virtual void SetReadPos( Uint64 pos ) override;
		virtual void SetWritePos( Uint64 pos ) override;

		virtual void ResetPos() override;

	private:
		std::unique_ptr< std::stringstream > m_stream;
	};
}

