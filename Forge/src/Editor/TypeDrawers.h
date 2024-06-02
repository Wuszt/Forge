#pragma once
#include "TypeDrawer.h"

namespace editor
{
	class TypeDrawer_Int32 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Int32, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Int32 >::GetInstance(); }

	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return false; }
	};

	class TypeDrawer_Float : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Float, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Float >::GetInstance(); }

	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return false; }
	};

	class TypeDrawer_Vector2 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector2, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector2::GetTypeStatic(); }

	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return false; }
	};

	class TypeDrawer_Vector3 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector3, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector3::GetTypeStatic(); }

	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return false; }
	};

	class TypeDrawer_Vector4 : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_Vector4, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector4::GetTypeStatic(); }

	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return false; }
	};

	class TypeDrawer_Array : public editor::TypeDrawer
	{
	public:
	protected:
		virtual void DrawChildren( void* address, const rtti::Type& type ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return true; }
	};

	class TypeDrawer_Vector : public editor::TypeDrawer
	{
	public:
	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
		virtual void DrawChildren( void* address, const rtti::Type& type ) const override;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const override { return true; }
	};

	class TypeDrawer_Enum : public editor::TypeDrawer
	{
	protected:
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const override;
	};

	class TypeDrawer_DataComponent : public editor::CustomTypeDrawer
	{
		RTTI_DECLARE_CLASS( TypeDrawer_DataComponent, editor::CustomTypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override;
	protected:
		virtual void DrawChildren( void* address, const rtti::Type& type ) const override;
	};
}

