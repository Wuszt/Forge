#pragma once

namespace renderer
{
	class IShaderResourceView;

	class ITexture
	{

	public:
		enum class Format
		{
			Unknown,
			R8G8B8A8_UNORM,
			R24G8_TYPELESS
		};

		enum class Flags
		{
			BIND_RENDER_TARGET = 1 << 0,
			BIND_SHADER_RESOURCE = 1 << 1,
			Last = BIND_SHADER_RESOURCE
		};

		virtual void Resize( Uint32 width, Uint32 height ) = 0;
		virtual IShaderResourceView* GetShaderResourceView() const = 0;
		virtual ~ITexture() = default;
	};

	FORGE_INLINE ITexture::Flags operator|( ITexture::Flags l, ITexture::Flags r )
	{
		return static_cast<ITexture::Flags>( static_cast<Uint32>( l ) | static_cast<Uint32>( r ) );
	}
}