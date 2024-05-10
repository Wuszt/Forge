#pragma once
namespace forge
{
	class EditorInstance;
}

namespace editor
{
	class TypeDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( TypeDrawer );

	public:
		static void DrawProperty( void* owner, const rtti::Property& property );
		static void DrawType( void* address, const rtti::Type& type );

		void Draw( void* owner, const rtti::Property& property ) const;
		void Draw( void* address, const rtti::Type& type ) const;

	protected:
		virtual void DrawPropertyName( void* owner, const rtti::Property& property ) const;
		virtual void DrawPropertyValue( void* owner, const rtti::Property& property ) const {}

		virtual Bool HasChildren( void* address, const rtti::Type& type ) const;
		virtual void DrawChildren( void* address, const rtti::Type& type ) const;
	};

	class CustomTypeDrawer : public TypeDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( CustomTypeDrawer, editor::TypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const = 0;
	};
}