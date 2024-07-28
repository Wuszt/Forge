#pragma once
#include "TypeDrawer.h"

namespace editor
{
	class TypeDrawer_Int32 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Int32, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Int32 >::GetInstance(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Int32() = default;
	};

	class TypeDrawer_Uint32 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Uint32, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Uint32 >::GetInstance(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Uint32() = default;
	};

	class TypeDrawer_Float : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Float, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Float >::GetInstance(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Float() = default;
	};

	class TypeDrawer_Vector2 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector2, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return Vector2::GetTypeStatic(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Vector2() = default;
	};

	class TypeDrawer_Vector3 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector3, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return Vector3::GetTypeStatic(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Vector3() = default;
	};

	class TypeDrawer_Vector4 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector4, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override { return Vector4::GetTypeStatic(); }
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return false; }
	private:
		TypeDrawer_Vector4() = default;
	};

	class TypeDrawer_Array : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawChildren( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return true; }
	private:
		TypeDrawer_Array() = default;
	};

	class TypeDrawer_Vector : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
		virtual void OnDrawChildren( const Drawable& drawable ) const override;
		virtual Bool HasChildren( const Drawable& drawable ) const override { return true; }
	private:
		TypeDrawer_Vector() = default;
	};

	class TypeDrawer_Enum : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_Enum() = default;
	};

	class TypeDrawer_String : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_String() = default;
	};

	class TypeDrawer_RawPointer : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_RawPointer() = default;
	};

	class TypeDrawer_UniquePointer : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_UniquePointer() = default;
	};

	class TypeDrawer_SharedPointer : public editor::TypeDrawer
	{
	public:
		using editor::TypeDrawer::TypeDrawer;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_SharedPointer() = default;
	};

	class TypeDrawer_DataComponent : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_DataComponent, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override;
		virtual void OnDrawChildren( const Drawable& drawable ) const override;
	private:
		TypeDrawer_DataComponent() = default;
	};

	class TypeDrawer_Path : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Path, editor::CustomTypeDrawer );

	public:
		using editor::CustomTypeDrawer::CustomTypeDrawer;
		virtual const rtti::Type& GetSupportedType() const override;
		virtual void OnDrawValue( const Drawable& drawable ) const override;
	private:
		TypeDrawer_Path() = default;
	};
}

