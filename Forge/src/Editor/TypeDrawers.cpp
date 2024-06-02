#include "Fpch.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui.h"
#include "../GameEngine/IComponent.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector4 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_DataComponent );

void editor::TypeDrawer_Int32::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	Int32& value = property.GetValue< Int32 >( owner );
	ImGui::InputInt( forge::String::Printf( "##%s", property.GetName() ).c_str(), &value );
}

void editor::TypeDrawer_Float::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	Float& value = property.GetValue< Float >( owner );
	ImGui::InputFloat( forge::String::Printf( "##%s", property.GetName() ).c_str(), &value );
}

void editor::TypeDrawer_Vector2::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	Vector2& value = property.GetValue< Vector2 >( owner );
	ImGui::InputFloat2( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::TypeDrawer_Vector3::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	Vector3& value = property.GetValue< Vector3 >( owner );
	ImGui::InputFloat3( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::TypeDrawer_Vector4::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	Vector4& value = property.GetValue< Vector4 >( owner );
	ImGui::InputFloat4( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::TypeDrawer_Array::DrawChildren( void* address, const rtti::Type& type ) const
{
	const rtti::ContainerType& containerType = static_cast< const rtti::ContainerType& >( type );
	containerType.VisitElementsAsProperties( address, [ & ]( const rtti::Property& property )
		{
			DrawProperty( address, property );
			return rtti::VisitOutcome::Continue;
		} );
}

void editor::TypeDrawer_Vector::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	void* containerAddress = property.GetAddress( owner );
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( property.GetType() );
	if ( ImGui::Button( "Add" ) )
	{
		containerType.AddDefaultElement( containerAddress );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) )
	{
		containerType.Clear( containerAddress );
	}
}

void editor::TypeDrawer_Vector::DrawChildren( void* address, const rtti::Type& type ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( type );

	Uint32 i = 0u;
	containerType.VisitElementsAsProperties( address, [ & ]( const rtti::Property& property )
		{
			if ( ImGui::BeginTable( property.GetName(), 2 ) )
			{
				ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
				ImGui::TableSetupColumn( "Property" );

				ImGui::TableNextColumn();
				const Int32 index = i++;
				if ( ImGui::Button( forge::String::Printf( "X##%d", index ).c_str(), { 0, 25.0f } ) )
				{
					containerType.RemoveElementAtIndex( address, index );
				}

				ImGui::TableNextColumn();
				DrawProperty( address, property );
				ImGui::EndTable();
			}

			return rtti::VisitOutcome::Continue;
		} );
}

const rtti::Type& editor::TypeDrawer_DataComponent::GetSupportedType() const
{
	return forge::IDataComponent::GetTypeStatic();
}

void editor::TypeDrawer_DataComponent::DrawChildren( void* address, const rtti::Type& type ) const
{
	forge::IDataComponent* dataComponent = static_cast< forge::IDataComponent* >( address );
	const ecs::Fragment::Type* fragmentType = nullptr;
	if ( void* fragmentAddress = dataComponent->GetMutableRawData( fragmentType ) )
	{
		editor::TypeDrawer::DrawType( fragmentAddress, *fragmentType );
	}

	editor::TypeDrawer::DrawChildren( address, type );
}

void editor::TypeDrawer_Enum::DrawPropertyValue( void* owner, const rtti::Property& property ) const
{
	const rtti::EnumTypeBase& enumType = static_cast< const rtti::EnumTypeBase& >( property.GetType() );
	std::string id = std::string( "##enumCombo_" ) + property.GetName();

	const auto* currentMember = enumType.GetCurrentMember( property.GetAddress( owner ) );
	FORGE_ASSERT( currentMember );

	if ( ImGui::BeginCombo( id.c_str(), currentMember->m_name ) )
	{
		Uint32 i = 0u;
		enumType.VisitMembers( [ & ]( const rtti::EnumTypeBase::MemberDesc& member )
			{
				const Bool isSelected = currentMember == &member;
				if ( ImGui::Selectable( member.m_name, isSelected ) )
				{
					enumType.SetCurrentMember( property.GetAddress( owner ), member );
				}

				if ( isSelected )
				{
					ImGui::SetItemDefaultFocus();
				}

				return rtti::VisitOutcome::Continue;
			} );		
		ImGui::EndCombo();
	}
}
