#pragma once
#include "IRenderingResource.h"

namespace renderer
{
	class IShaderResourceView;
	class IRenderTargetView;

	class ITexture : public IRenderingResource
	{
	public:
		enum class Type
		{
			Texture2D,
			TextureCube
		};

		enum class Format
		{
			Unknown,
			R8G8B8A8_UNORM,
			R24G8_TYPELESS,
			R24_UNORM_X8_TYPELESS
		};

		enum class Flags
		{
			BIND_RENDER_TARGET = 1 << 0,
			BIND_DEPTH_STENCIL = 1 << 1,
			BIND_SHADER_RESOURCE = 1 << 2,
			Last = BIND_SHADER_RESOURCE
		};

		virtual void Resize( const Vector2& size ) = 0;
		virtual const IShaderResourceView* GetShaderResourceView() const = 0;
		virtual IRenderTargetView* GetRenderTargetView() const = 0;
		virtual forge::Callback< const Vector2& >& GetOnResizedCallback() = 0;
		virtual Vector2 GetSize() const = 0;
		virtual Type GetType() const = 0;
	};

	FORGE_INLINE ITexture::Flags operator|( ITexture::Flags l, ITexture::Flags r )
	{
		return static_cast<ITexture::Flags>( static_cast<Uint32>( l ) | static_cast<Uint32>( r ) );
	}
}
