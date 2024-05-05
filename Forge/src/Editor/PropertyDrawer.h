#pragma once
namespace forge
{
	class EditorInstance;
}

namespace editor
{
	class PropertyDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( PropertyDrawer );

	public:
		static void DrawProperty( void* owner, const rtti::Property& property );
		virtual void Draw( void* owner, const rtti::Property& property ) const;

	protected:
		virtual void DrawName( void* owner, const rtti::Property& property ) const;
		virtual void DrawValue( void* owner, const rtti::Property& property ) const {}
	};

	class CustomPropertyDrawer : public PropertyDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( CustomPropertyDrawer, editor::PropertyDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const = 0;
	};
}