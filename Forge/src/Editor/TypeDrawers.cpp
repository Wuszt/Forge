#include "Fpch.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui.h"
#include "../GameEngine/IComponent.h"
#include "../Renderer/Material.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector4 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_DataComponent );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Material );

static ImVec2 GetButtonSize()
{
	return { 0.0f, ImGui::GetFontSize() + 6.0f };
}

void editor::TypeDrawer_Int32::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	Int32& value = GetValue< Int32 >( address );
	ImGui::InputInt( "##Value", &value );
}

void editor::TypeDrawer_Float::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	Float& value = GetValue< Float >( address );
	ImGui::InputFloat( "##Value", &value);
}

void editor::TypeDrawer_Vector2::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	Vector2& value = GetValue< Vector2 >( address );
	ImGui::InputFloat2( "##Value", value.AsArray());
}

void editor::TypeDrawer_Vector3::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	Vector3& value = GetValue< Vector3 >( address );
	ImGui::InputFloat3( "##Value", value.AsArray());
}

void editor::TypeDrawer_Vector4::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	Vector4& value = GetValue< Vector4 >( address );
	ImGui::InputFloat4( "##Value", value.AsArray());
}

void editor::TypeDrawer_Array::OnDrawChildren( void* address, const rtti::Type& type ) const
{
	const rtti::ContainerType& containerType = static_cast< const rtti::ContainerType& >( type );
	containerType.VisitElementsAsProperties( address, [ & ]( const rtti::Property& property )
		{
			DrawProperty( address, property );
			return rtti::VisitOutcome::Continue;
		} );
}

void editor::TypeDrawer_Vector::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( type );
	if ( ImGui::Button( "Add" ) )
	{
		containerType.AddDefaultElement( address );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) )
	{
		containerType.Clear( address );
	}
}

void editor::TypeDrawer_Vector::OnDrawChildren( void* address, const rtti::Type& type ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( type );

	Uint32 i = 0u;
	containerType.VisitElementsAsProperties( address, [ & ]( const rtti::Property& property )
		{
			if ( ImGui::BeginTable( "VectorTable", 2) )
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

void editor::TypeDrawer_DataComponent::OnDrawChildren( void* address, const rtti::Type& type ) const
{
	forge::IDataComponent* dataComponent = static_cast< forge::IDataComponent* >( address );
	const ecs::Fragment::Type* fragmentType = nullptr;
	if ( void* fragmentAddress = dataComponent->GetMutableRawData( fragmentType ) )
	{
		editor::TypeDrawer::DrawTypeChildren( fragmentAddress, *fragmentType );
	}

	editor::TypeDrawer::OnDrawChildren( address, type );
}

void editor::TypeDrawer_Enum::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	const rtti::EnumTypeBase& enumType = static_cast< const rtti::EnumTypeBase& >( type );

	const auto* currentMember = enumType.GetCurrentMember( address );
	FORGE_ASSERT( currentMember );

	if ( ImGui::BeginCombo( "Enum", currentMember->m_name ) )
	{
		Uint32 i = 0u;
		enumType.VisitMembers( [ & ]( const rtti::EnumTypeBase::MemberDesc& member )
			{
				const Bool isSelected = currentMember == &member;
				if ( ImGui::Selectable( member.m_name, isSelected ) )
				{
					enumType.SetCurrentMember( address, member );
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

void editor::TypeDrawer_RawPointer::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	const rtti::PointerType< void >& pointerType = static_cast< const rtti::PointerType< void >& >( type );
	if ( void* pointedAddress = GetValue< void* >( address ) )
	{
		if ( ImGui::BeginTable( "PointerTable", 2) )
		{
			ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Property" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "X", GetButtonSize() ) )
			{
				GetValue< void* >( address ) = nullptr;
			}
			else
			{
				ImGui::TableNextColumn();
				DrawType( pointedAddress, pointerType.GetInternalTypeDesc().GetType(), "PointedValue" );
			}
			ImGui::EndTable();
		}
	}
	else
	{
		ImGui::Text( "NULL" );
	}
}

template< class T >
void DrawSmartPtr( void* address, const T& pointerType )
{
	if ( void* pointedAddress = pointerType.GetPointedAddress( address ) )
	{
		if ( ImGui::BeginTable( "SmartPtr", 2) )
		{
			ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Property" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "X", GetButtonSize() ) )
			{
				pointerType.SetPointedAddress( address, nullptr );
			}
			else
			{
				ImGui::TableNextColumn();
				editor::TypeDrawer::DrawType( pointedAddress, pointerType.GetInternalTypeDesc().GetType(), "PointedValue" );
			}

			ImGui::EndTable();
		}
	}
	else
	{
		if ( ImGui::BeginTable( "SmartPtr", 2) )
		{
			ImGui::TableSetupColumn( "NewButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Value" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "New", GetButtonSize() ) )
			{
				pointerType.ConstructInPlace( address );
				forge::RawSmartPtr buffer( pointerType.GetInternalTypeDesc().GetType().GetSize() );
				pointerType.GetInternalTypeDesc().GetType().ConstructInPlace( buffer.GetData() );
				pointerType.SetPointedAddress( address, buffer.GetData() );
				buffer.Release();
			}

			ImGui::TableNextColumn();
			ImGui::Text( "NULL" );
			ImGui::EndTable();
		}
	}
}

void editor::TypeDrawer_UniquePointer::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags) const
{
	DrawSmartPtr( address, static_cast< const rtti::UniquePtrBaseType& >( type ) );
}

void editor::TypeDrawer_SharedPointer::OnDrawValue( void* address, const rtti::Type& type, rtti::InstanceFlags typeFlags ) const
{
	DrawSmartPtr( address, static_cast< const rtti::SharedPtrBaseType& >( type ) );
}
