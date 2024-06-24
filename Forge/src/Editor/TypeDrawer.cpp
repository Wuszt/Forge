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
		OnDrawName( owner, property );
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::PushItemWidth( -1.0f );
		DrawValue( property.GetAddress( owner ), property.GetType(), property.GetFlags(), property.GetName() );
		ImGui::EndTable();
	}
	ImGui::Unindent();

	
	DrawChildrenInternal( property.GetAddress( owner ), property.GetType(), property.GetName(), { startPos.x, startPos.y }, Math::Min(16.0f, ImGui::GetCursorPos().y - startPos.y));
}

void editor::TypeDrawer::DrawChildren( void* address, const rtti::Type& type, const char* id ) const
{
	ImGui::PushID( id );
	DrawChildren( address, type );
	ImGui::PopID();
}

void editor::TypeDrawer::DrawChildren( void* address, const rtti::Type& type ) const
{
	OnDrawChildren( address, type );
}

void editor::TypeDrawer::DrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags, const char* id ) const
{
	ImGui::PushID( id );
	OnDrawValue( address, type, typeFlags );
	ImGui::PopID();
}

void editor::TypeDrawer::Draw( void* address, const rtti::Type& type, const char* id ) const
{
	const auto startPos = ImGui::GetCursorPos();
	DrawValue( address, type, rtti::InstanceFlags::None, id );
	DrawChildrenInternal( address, type, id, { startPos.x, startPos.y }, Math::Min( 16.0f, ImGui::GetCursorPos().y - startPos.y ) );
}

static std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomTypeDrawer > > CollectCustomTypeDrawers()
{
	std::unordered_map< const rtti::Type*, std::unique_ptr< editor::CustomTypeDrawer > > drawers;
	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.InheritsFrom< editor::CustomTypeDrawer >() )
			{
				std::unique_ptr< editor::CustomTypeDrawer > drawer( static_cast< const editor::CustomTypeDrawer::Type& >( type ).ConstructTyped() );
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
				func( *found->second );
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
		func( editor::TypeDrawer() );
		break;

	case rtti::Type::Kind::RawPointer:
		func( editor::TypeDrawer_RawPointer() );
		break;

	case rtti::Type::Kind::UniquePointer:
		func( editor::TypeDrawer_UniquePointer() );
		break;

	case rtti::Type::Kind::SharedPointer:
		func( editor::TypeDrawer_SharedPointer() );
		break;

	case rtti::Type::Kind::Array:
		func( editor::TypeDrawer_Array() );
		break;

	case rtti::Type::Kind::Vector:
		func( editor::TypeDrawer_Vector() );
		break;

	case rtti::Type::Kind::Set:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Map:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Enum:
		func( editor::TypeDrawer_Enum() );
		break;

	case rtti::Type::Kind::String:
		func( editor::TypeDrawer_String() );
		break;

	default:
		FORGE_ASSERT( false );
	}
}

void editor::TypeDrawer::DrawProperty( void* owner, const rtti::Property& property )
{
	VisitTypeDrawer( property.GetType(), [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.Draw( owner, property ); } );
}

void editor::TypeDrawer::DrawType( void* address, const rtti::Type& type, const char* id )
{
	VisitTypeDrawer( type, [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.Draw( address, type, id ); } );
}

void editor::TypeDrawer::DrawTypeChildren( void* address, const rtti::Type& type, const char* id )
{
	VisitTypeDrawer( type, [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.DrawChildren( address, type, id ); } );
}

void editor::TypeDrawer::DrawTypeChildren( void* address, const rtti::Type& type )
{
	VisitTypeDrawer( type, [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.DrawChildren( address, type ); } );
}

void editor::TypeDrawer::OnDrawName( void* owner, const rtti::Property& property ) const
{
	ImGui::Text( property.GetName() );
}

Bool editor::TypeDrawer::HasChildren( void* owner, const rtti::Type& type ) const
{
	return type.GetPropertiesAmount() > 0;
}

void editor::TypeDrawer::OnDrawChildren( void* address, const rtti::Type& type ) const
{
	for ( Uint32 i = 0u; i < type.GetPropertiesAmount(); ++i )
	{
		const auto* prop = type.GetProperty( i );
		DrawProperty( address, *prop );
	}
}

void editor::TypeDrawer::DrawChildrenInternal( void* address, const rtti::Type& type, const char* id, const Vector2& startPos, Float height ) const
{
	if ( HasChildren( address, type ) )
	{
		ImGui::SetCursorPos( { startPos.X, startPos.Y } );
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.f, ( height ) * 0.5f - 6.5f ) );

		auto prev = ImGui::GetCurrentWindow()->WorkRect.Max.x;
		ImGui::GetCurrentWindow()->WorkRect.Max.x = ImGui::GetCurrentWindow()->WorkRect.Min.x + ImGui::GetStyle().IndentSpacing;
		if ( ImGui::TreeNodeEx( forge::String::Printf( "##Children%s", id ).c_str(), ImGuiTreeNodeFlags_FramePadding, "") )
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;

			DrawChildren( address, type );
			ImGui::TreePop();
		}
		else
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;
		}
	}
}
