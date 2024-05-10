#include "Fpch.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector4 );

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
		} );
}