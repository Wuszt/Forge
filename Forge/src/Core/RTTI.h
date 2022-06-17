#pragma once

namespace rtti
{
	class IType
	{
	public:
		~IType() = default;
		virtual const char* GetName() const = 0;

		Bool IsA( const IType& type ) const
		{
			return this == &type;
		}

		template< class T >
		Bool IsA() const
		{
			return IsA( T::GetTypeStatic() );
		}

		virtual Bool InheritsFrom( const IType& type ) const = 0;

		template< class T >
		Bool InheritsFrom() const
		{
			return InheritsFrom( T::GetTypeStatic() );
		}

		template< class T >
		static Bool InheritsFromStatic()
		{
			return false;
		}
	};
}

#define INHERITS_FROM_BODY_true(ParentClassName) return ParentClassName##::GetTypeStatic().IsA( type ) || ParentClassName##::GetTypeStatic().InheritsFrom( type );
#define INHERITS_FROM_BODY_false(ParentClassName) return false;

#define VIRTUAL_true virtual
#define VIRTUAL_false

#define INHERITS_FROM_STATIC_BODY_true return Super::GetTypeStatic().IsA< T >() || Super::InheritsFromStatic< T >();
#define INHERITS_FROM_STATIC_BODY_false return false;

#define DECLARE_TYPE_INTERNAL_PARENT(ClassName, NamespaceParentClassName, ParentClassName, Inherits, Virtual) \
public: \
class ClassName##Type : public NamespaceParentClassName##ParentClassName##Type \
{ \
public: \
	using BaseClassType = NamespaceParentClassName##ParentClassName##Type; \
	virtual const char* GetName() const override; \
	virtual Bool InheritsFrom( const rtti::IType& type ) const override \
	{ \
		INHERITS_FROM_BODY_##Inherits##(NamespaceParentClassName##ParentClassName) \
	} \
private: \
	const std::string m_typeName = #ClassName; \
}; \
	static const ClassName##Type& GetTypeStatic() \
	{ \
		return s_typeInstance; \
	} \
	template< class T > \
	Bool IsA() const \
	{ \
		return GetType().IsA< T >(); \
	} \
	template< class T > \
	Bool InheritsFrom() const \
	{ \
		return GetType().InheritsFrom< T >(); \
	} \
	template< class T > \
	Bool InheritsFromOrIsA() const \
	{ \
		return IsA< T >() || InheritsFrom< T >(); \
	} \
	VIRTUAL_##Virtual const rtti::IType& GetType() const \
	{ \
		return s_typeInstance; \
	} \
	template< class T > \
	static Bool InheritsFromOrIsAStatic() \
	{ \
		return GetTypeStatic().IsA< T >() || InheritsFromStatic< T >(); \
	} \
	template< class T > \
	static Bool InheritsFromStatic() \
	{ \
		INHERITS_FROM_STATIC_BODY_##Inherits; \
	} \
private: \
	static ClassName##Type s_typeInstance;

#define DECLARE_TYPE_INTERNAL_PARENT_NAMESPACE_DIRECT(ClassName, NamespaceParentClassName, ParentClassName) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName, NamespaceParentClassName##::##ParentClassName##::, ParentClassName, true, true) \
	using Super = NamespaceParentClassName##::##ParentClassName;

#define DECLARE_TYPE_INTERNAL(ClassName) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName,, rtti::I, false, false)

#define DECLARE_TYPE_INTERNAL_PARENT_DIRECT(ClassName, ParentClassName) \
DECLARE_TYPE_INTERNAL_PARENT( ClassName,, ParentClassName, true, true ) \
	using Super = ParentClassName;

#define EXPAND( x ) x

#define GET_DECLARE_TYPE_MACRO(_1,_2,_3,NAME,...) NAME
#define DECLARE_TYPE(...) EXPAND(GET_DECLARE_TYPE_MACRO(__VA_ARGS__, DECLARE_TYPE_INTERNAL_PARENT_NAMESPACE_DIRECT, DECLARE_TYPE_INTERNAL_PARENT_DIRECT, DECLARE_TYPE_INTERNAL)(__VA_ARGS__))

#define DECLARE_POLYMORPHIC_BASE_TYPE(ClassName) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName,, rtti::I, false, true)

#define IMPLEMENT_TYPE_INTERNAL(NamespaceClassName, ClassName) NamespaceClassName##::##ClassName##Type NamespaceClassName##::s_typeInstance; \
const char* NamespaceClassName##::##ClassName##Type::GetName() const \
{ \
	return #NamespaceClassName; \
}

#define IMPLEMENT_TYPE_INTERNAL_1(_0,_1) IMPLEMENT_TYPE_INTERNAL(_0##::##_1, _1)
#define IMPLEMENT_TYPE_INTERNAL_0(_0) IMPLEMENT_TYPE_INTERNAL(_0,_0)

#define GET_IMPLEMENT_TYPE_MACRO(_1,_2,NAME,...) NAME
#define IMPLEMENT_TYPE(...) EXPAND(GET_IMPLEMENT_TYPE_MACRO(__VA_ARGS__, IMPLEMENT_TYPE_INTERNAL_1, IMPLEMENT_TYPE_INTERNAL_0)(__VA_ARGS__))