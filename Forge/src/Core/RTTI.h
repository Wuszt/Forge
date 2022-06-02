#pragma once

class IType
{
public:
	~IType() = default;
	virtual const std::string& GetName() const = 0;
	
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

#define INHERITS_FROM_BODY_true(ParentClassName) return ParentClassName##::GetTypeStatic().IsA( type ) || ParentClassName##::GetTypeStatic().InheritsFrom( type );
#define INHERITS_FROM_BODY_false(ParentClassName) return false;

#define VIRTUAL_true virtual
#define VIRTUAL_false

#define INHERITS_FROM_STATIC_BODY_true return Super::GetTypeStatic().IsA< T >() || Super::InheritsFromStatic< T >();
#define INHERITS_FROM_STATIC_BODY_false return false;

#define DECLARE_TYPE_INTERNAL_PARENT(ClassName, ParentClassName, Inherits, Virtual) \
public: \
class ClassName##Type : public ParentClassName##Type \
{ \
public: \
	using BaseClassType = ParentClassName##Type; \
	virtual const std::string& GetName() const override \
	{\
		return m_typeName; \
	}\
	virtual Bool InheritsFrom( const IType& type ) const override \
	{ \
		INHERITS_FROM_BODY_##Inherits##(ParentClassName) \
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
	VIRTUAL_##Virtual const IType& GetType() const \
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

#define DECLARE_TYPE_INTERNAL_PARENT_DIRECT(ClassName, ParentClassName) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName, ParentClassName, true, true) \
	using Super = ParentClassName;

#define DECLARE_TYPE_INTERNAL(ClassName) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName,I, false, false)

#define EXPAND( x ) x
#define GET_MACRO(_1,_2,NAME,...) NAME
#define DECLARE_TYPE(...) EXPAND(GET_MACRO(__VA_ARGS__, DECLARE_TYPE_INTERNAL_PARENT_DIRECT, DECLARE_TYPE_INTERNAL)(__VA_ARGS__))

#define DECLARE_POLYMORPHIC_BASE_TYPE( ClassName ) \
DECLARE_TYPE_INTERNAL_PARENT(ClassName,I, false, true)

#define IMPLEMENT_TYPE(ClassName) ClassName##::##ClassName##Type ClassName##::s_typeInstance;