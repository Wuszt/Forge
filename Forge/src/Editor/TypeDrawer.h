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
		static void DrawType( void* address, const rtti::Type& type, const char* id );
		static void DrawTypeChildren( void* address, const rtti::Type& type, const char* id );
		static void DrawTypeChildren( void* address, const rtti::Type& type );

		void Draw( void* owner, const rtti::Property& property ) const;
		void Draw( void* address, const rtti::Type& type, const char* id ) const;
		void DrawChildren( void* address, const rtti::Type& type, const char* id ) const;
		void DrawChildren( void* address, const rtti::Type& type ) const;
		void DrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags, const char* id ) const;
		virtual Bool HasChildren( void* address, const rtti::Type& type ) const;

	protected:
		virtual void OnDrawName( void* owner, const rtti::Property& property ) const;
		virtual void OnDrawChildren( void* address, const rtti::Type& type ) const;
		virtual void OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const {}

		template< class T >
		static T& GetValue( void* address )
		{
			return *static_cast< T* >( address );
		}

	private:
		void DrawChildrenInternal( void* address, const rtti::Type& type, const char* id, const Vector2& startPos, Float height ) const;
	};

	class CustomTypeDrawer : public TypeDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( CustomTypeDrawer, editor::TypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const = 0;
	};
}