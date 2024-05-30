#include "Fpch.h"
#include "TypeDrawer.h"
#include "../../External/imgui/imgui.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui_internal.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer );
RTTI_IMPLEMENT_TYPE( editor::CustomTypeDrawer );

void editor::TypeDrawer::Draw( void* owner, const rtti::Property& property ) const
{
	const auto startPos = ImGui::GetCursorPos();

	ImGui::Indent();
	if ( ImGui::BeginTable( property.GetName(), 2 ) )
	{
		ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed );

		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		DrawPropertyName( owner, property );
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::PushItemWidth( -1.0f );
		DrawPropertyValue( owner, property );
		ImGui::EndTable();
	}
	ImGui::Unindent();

	const Float height = ImGui::GetCursorPos().y - startPos.y;

	if ( HasChildren( owner, property.GetType() ) )
	{
		ImGui::SetCursorPos( startPos );
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.f, ( height ) * 0.5f - 6.5f ) );

		auto prev = ImGui::GetCurrentWindow()->WorkRect.Max.x;
		ImGui::GetCurrentWindow()->WorkRect.Max.x = ImGui::GetCurrentWindow()->WorkRect.Min.x + ImGui::GetStyle().IndentSpacing;
		if ( ImGui::TreeNodeEx( property.GetName(), ImGuiTreeNodeFlags_FramePadding, "" ) )
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;

			DrawChildren( property.GetAddress( owner ), property.GetType() );
			ImGui::TreePop();
		}
		else
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;
		}
	}
}

void editor::TypeDrawer::Draw( void* address, const rtti::Type& type ) const
{
	DrawChildren( address, type );
}

static std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomTypeDrawer > > CollectCustomTypeDrawers()
{
	std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomTypeDrawer > > drawers;
	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.InheritsFrom< editor::CustomTypeDrawer >() )
			{
				auto drawer = static_cast< const editor::CustomTypeDrawer::Type& >( type ).Construct();
				const auto& supportedType = drawer->GetSupportedType();
				drawers.emplace( &supportedType, std::move( drawer ) );
			}

			return rtti::VisitOutcome::Continue;
		} );

	return drawers;
}

template< class TFunc >
static void VisitTypeDrawer( const rtti::Type& type, const TFunc& func )
{
	static const auto drawers = CollectCustomTypeDrawers();

	{
		const rtti::Type* tmpTypePtr = &type;
		while ( tmpTypePtr )
		{
			auto found = drawers.find( tmpTypePtr );
			if ( found != drawers.end() )
			{
				func(*found->second);
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
		func(editor::TypeDrawer());
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
		func(editor::TypeDrawer_Array());
		break;

	case rtti::Type::Kind::Vector:
		func(editor::TypeDrawer_Vector());
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

void editor::TypeDrawer::DrawProperty( void* owner, const rtti::Property& property )
{
	VisitTypeDrawer( property.GetType(), [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.Draw(owner, property); } );
}

void editor::TypeDrawer::DrawType( void* address, const rtti::Type& type )
{
	VisitTypeDrawer( type, [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.Draw( address, type ); } );
}

void editor::TypeDrawer::DrawPropertyName( void* owner, const rtti::Property& property ) const
{
	ImGui::Text( property.GetName() );
}

Bool editor::TypeDrawer::HasChildren( void* owner, const rtti::Type& type ) const
{
	return type.GetPropertiesAmount() > 0;
}

void editor::TypeDrawer::DrawChildren( void* address, const rtti::Type& type ) const
{
	for ( Uint32 i = 0u; i < type.GetPropertiesAmount(); ++i )
	{
		const auto* prop = type.GetProperty( i );
		DrawProperty( address, *prop );
	}
}
