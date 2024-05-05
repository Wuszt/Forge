#include "Fpch.h"
#include "PropertyDrawer.h"
#include "../../External/imgui/imgui.h"
#include "PropertyDrawers.h"

RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer );
RTTI_IMPLEMENT_TYPE( editor::CustomPropertyDrawer );

void editor::PropertyDrawer::Draw( void* owner, const rtti::Property& property ) const
{
	if ( ImGui::BeginTable( property.GetName(), 2 ) )
	{
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		DrawName( owner, property ); 
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		DrawValue( owner, property );
		ImGui::TableNextRow();

		ImGui::EndTable();
	}
}

static std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomPropertyDrawer > > CollectCustomPropertyDrawers()
{
	std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomPropertyDrawer > > drawers;
	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.InheritsFrom< editor::CustomPropertyDrawer >() )
			{
				auto drawer = static_cast< const editor::CustomPropertyDrawer::Type& >( type ).Construct();
				const auto& supportedType =  drawer->GetSupportedType();
				drawers.emplace( &supportedType, std::move( drawer ) );
			}
		} );

	return drawers;
}

void editor::PropertyDrawer::DrawProperty( void* owner, const rtti::Property& property )
{
	static const auto drawers = CollectCustomPropertyDrawers();

	const rtti::Type& type = property.GetType();
	{
		const rtti::Type* tmpTypePtr = &type;
		while ( tmpTypePtr )
		{
			auto found = drawers.find( tmpTypePtr );
			if ( found != drawers.end() )
			{
				found->second->Draw( owner, property );
				return;
			}
			tmpTypePtr = tmpTypePtr->GetParent();
		}
	}

	switch ( type.GetKind() )
	{
	case rtti::Type::Kind::Class:
	case rtti::Type::Kind::RuntimeType:
	case rtti::Type::Kind::Struct:
		editor::PropertyDrawer_Class().Draw( owner, property );
		break;

	case rtti::Type::Kind::RawPointer:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::UniquePointer:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::SharedPointer:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Array:
		editor::PropertyDrawer_Array().Draw( owner, property );
		break;

	case rtti::Type::Kind::Vector:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Set:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Map:
		FORGE_ASSERT( false ); // TODO
		break;

	default:
		FORGE_ASSERT( false );
	}
}

void editor::PropertyDrawer::DrawName( void* owner, const rtti::Property& property ) const
{
	ImGui::Indent();
	ImGui::Text( property.GetName() );
	ImGui::Unindent();
}
