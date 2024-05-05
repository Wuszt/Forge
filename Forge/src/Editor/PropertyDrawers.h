#pragma once
#include "PropertyDrawer.h"

namespace editor
{
	class PropertyDrawer_Int32 : public editor::CustomPropertyDrawer
	{
		RTTI_DECLARE_CLASS( PropertyDrawer_Int32, editor::CustomPropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Int32 >::GetInstance(); }

	protected:
		virtual void DrawValue( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Float : public editor::CustomPropertyDrawer
	{
		RTTI_DECLARE_CLASS( PropertyDrawer_Float, editor::CustomPropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return rtti::PrimitiveType< Float >::GetInstance(); }

	protected:
		virtual void DrawValue( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Vector2 : public editor::CustomPropertyDrawer
	{
		RTTI_DECLARE_CLASS( PropertyDrawer_Vector2, editor::CustomPropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector2::GetTypeStatic(); }

	protected:
		virtual void DrawValue( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Vector3 : public editor::CustomPropertyDrawer
	{
		RTTI_DECLARE_CLASS( PropertyDrawer_Vector3, editor::CustomPropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector3::GetTypeStatic(); }

	protected:
		virtual void DrawValue( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Vector4 : public editor::CustomPropertyDrawer
	{
		RTTI_DECLARE_CLASS( PropertyDrawer_Vector4, editor::CustomPropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const override { return Vector4::GetTypeStatic(); }

	protected:
		virtual void DrawValue( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Class : public editor::PropertyDrawer
	{
	public:
		virtual void Draw( void* owner, const rtti::Property& property ) const override;
	};

	class PropertyDrawer_Array : public editor::PropertyDrawer
	{
	public:
		virtual void Draw( void* owner, const rtti::Property& property ) const override;
	};
}

