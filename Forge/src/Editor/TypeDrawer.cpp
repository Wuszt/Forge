#include "Fpch.h"
#include "TypeDrawer.h"
#include "../../External/imgui/imgui.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui_internal.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer );
RTTI_IMPLEMENT_TYPE( editor::CustomTypeDrawer );

static std::unordered_map< const rtti::Type*, const editor::CustomTypeDrawer::Type* > CollectCustomTypeDrawers()
{
	std::unordered_map< const rtti::Type*, const editor::CustomTypeDrawer::Type* > drawers;
	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.InheritsFrom< editor::CustomTypeDrawer >() )
			{
				std::unique_ptr< editor::CustomTypeDrawer > drawer( static_cast< const editor::CustomTypeDrawer::Type& >( type ).ConstructTyped() );
				const auto& supportedType = drawer->GetSupportedType();
				drawers.emplace( &supportedType, static_cast< const editor::CustomTypeDrawer::Type* >( &type ) );
			}

			return rtti::VisitOutcome::Continue;
		} );

	return drawers;
}

template< class TFunc >
static void VisitTypeDrawer( forge::EngineInstance& engineInstance, const rtti::Type& type, const TFunc& func )
{
	static const auto drawers = CollectCustomTypeDrawers();

	{
		const rtti::Type* tmpTypePtr = &type;
		while ( tmpTypePtr )
		{
			auto found = drawers.find( tmpTypePtr );
			if ( found != drawers.end() )
			{
				std::unique_ptr< editor::CustomTypeDrawer > drawer( found->second->ConstructTyped() );
				drawer->SetEngineInstance( engineInstance );
				func( *drawer );
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
		func( editor::TypeDrawer( engineInstance ) );
		break;

	case rtti::Type::Kind::RawPointer:
		func( editor::TypeDrawer_RawPointer( engineInstance ) );
		break;

	case rtti::Type::Kind::UniquePointer:
		func( editor::TypeDrawer_UniquePointer( engineInstance ) );
		break;

	case rtti::Type::Kind::SharedPointer:
		func( editor::TypeDrawer_SharedPointer( engineInstance ) );
		break;

	case rtti::Type::Kind::Array:
		func( editor::TypeDrawer_Array( engineInstance ) );
		break;

	case rtti::Type::Kind::Vector:
		func( editor::TypeDrawer_Vector( engineInstance ) );
		break;

	case rtti::Type::Kind::Set:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Map:
		FORGE_ASSERT( false ); // TODO
		break;

	case rtti::Type::Kind::Enum:
		func( editor::TypeDrawer_Enum( engineInstance ) );
		break;

	case rtti::Type::Kind::String:
		func( editor::TypeDrawer_String( engineInstance ) );
		break;

	default:
		FORGE_ASSERT( false );
	}
}

void editor::TypeDrawer::Draw( const Drawable& drawable ) const
{
	if ( drawable.GetName() == nullptr )
	{
		const auto startPos = ImGui::GetCursorPos();
		DrawValue( drawable );
		DrawChildrenInternal( drawable, { startPos.x, startPos.y }, Math::Min( 16.0f, ImGui::GetCursorPos().y - startPos.y ) );
		return;
	}

	const auto startPos = ImGui::GetCursorPos();

	ImGui::Indent();
	if ( ImGui::BeginTable( drawable.GetName(), 2 ) )
	{
		ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthFixed );

		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		OnDrawName( drawable );
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::PushItemWidth( -1.0f );
		DrawValue( drawable );
		ImGui::EndTable();
	}
	ImGui::Unindent();

	
	DrawChildrenInternal( drawable, { startPos.x, startPos.y }, Math::Min(16.0f, ImGui::GetCursorPos().y - startPos.y));
}

void editor::TypeDrawer::Draw( forge::EngineInstance& engineInstance, const Drawable& drawable )
{
	VisitTypeDrawer( engineInstance, drawable.GetType(), [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.Draw( drawable ); } );
}

void editor::TypeDrawer::DrawChildren( const Drawable& drawable ) const
{
	ImGui::PushID( drawable.GetID() );
	OnDrawChildren( drawable );
	ImGui::PopID();
}

void editor::TypeDrawer::DrawValue( const Drawable& drawable ) const
{
	if ( drawable.GetID() )
	{
		ImGui::PushID( drawable.GetID() );
	}

	OnDrawValue( drawable );

	if ( drawable.GetID() )
	{
		ImGui::PopID();
	}
}

void editor::TypeDrawer::DrawChildren( forge::EngineInstance& engineInstance, const Drawable& drawable )
{
	VisitTypeDrawer( engineInstance, drawable.GetType(), [ & ]( const editor::TypeDrawer& typeDrawer ) { typeDrawer.DrawChildren( drawable ); } );
}

Bool editor::TypeDrawer::HasChildren( const Drawable& drawable ) const
{
	return drawable.GetType().GetPropertiesAmount() > 0;
}

void editor::TypeDrawer::OnDrawName( const Drawable& drawable ) const
{
	ImGui::Text( drawable.GetName() );
}

void editor::TypeDrawer::OnDrawChildren( const Drawable& drawable ) const
{
	for ( Uint32 i = 0u; i < drawable.GetType().GetPropertiesAmount(); ++i )
	{
		const auto* prop = drawable.GetType().GetProperty( i );
		Draw( GetEngineInstance(), DrawableProperty( prop->GetAddress( drawable.GetAddress() ), *prop ) );
	}
}

void editor::TypeDrawer::DrawChildrenInternal( const Drawable& drawable, const Vector2& startPos, Float height ) const
{
	if ( HasChildren( drawable ) )
	{
		ImGui::SetCursorPos( { startPos.X, startPos.Y } );
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0.f, ( height ) * 0.5f - 6.5f ) );

		auto prev = ImGui::GetCurrentWindow()->WorkRect.Max.x;
		ImGui::GetCurrentWindow()->WorkRect.Max.x = ImGui::GetCurrentWindow()->WorkRect.Min.x + ImGui::GetStyle().IndentSpacing;
		if ( ImGui::TreeNodeEx( forge::String::Printf( "##Children%s", drawable.GetID() ).c_str(), ImGuiTreeNodeFlags_FramePadding, "") )
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;

			DrawChildren( drawable );
			ImGui::TreePop();
		}
		else
		{
			ImGui::PopStyleVar();
			ImGui::GetCurrentWindow()->WorkRect.Max.x = prev;
		}
	}
}
