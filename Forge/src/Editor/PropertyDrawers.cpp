#include "Fpch.h"
#include "PropertyDrawers.h"
#include "../../External/imgui/imgui.h"

RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::PropertyDrawer_Vector4 );

void editor::PropertyDrawer_Int32::DrawValue( void* owner, const rtti::Property& property ) const
{
	Int32& value = property.GetValue< Int32 >( owner );
	ImGui::InputInt( forge::String::Printf( "##%s", property.GetName() ).c_str(), &value );
}

void editor::PropertyDrawer_Float::DrawValue( void* owner, const rtti::Property& property ) const
{
	Float& value = property.GetValue< Float >( owner );
	ImGui::InputFloat( forge::String::Printf( "##%s", property.GetName() ).c_str(), &value );
}

void editor::PropertyDrawer_Class::Draw( void* owner, const rtti::Property& property ) const
{
	if ( ImGui::TreeNode( property.GetName() ) )
	{
		const auto& type = property.GetType();
		for ( Uint32 i = 0u; i < type.GetPropertiesAmount(); ++i )
		{
			void* add = property.GetAddress( owner );
			auto* prop = type.GetProperty( i );
			DrawProperty( property.GetAddress( owner ), *type.GetProperty( i ) );
		}

		ImGui::TreePop();
	}
}

void editor::PropertyDrawer_Vector2::DrawValue( void* owner, const rtti::Property& property ) const
{
	Vector2& value = property.GetValue< Vector2 >( owner );
	ImGui::InputFloat2( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::PropertyDrawer_Vector3::DrawValue( void* owner, const rtti::Property& property ) const
{
	Vector3& value = property.GetValue< Vector3 >( owner );
	ImGui::InputFloat3( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::PropertyDrawer_Vector4::DrawValue( void* owner, const rtti::Property& property ) const
{
	Vector4& value = property.GetValue< Vector4 >( owner );
	ImGui::InputFloat4( forge::String::Printf( "##%s", property.GetName() ).c_str(), value.AsArray() );
}

void editor::PropertyDrawer_Array::Draw( void* owner, const rtti::Property& property ) const
{
	const rtti::ContainerType& containerType = static_cast< const rtti::ContainerType& >( property.GetType() );
	void* containerAddress = property.GetAddress( owner );
	if ( ImGui::TreeNode( property.GetName() ) )
	{
		containerType.VisitElementsAsProperties( containerAddress, [ & ]( const rtti::Property& property )
			{
				DrawProperty( containerAddress, property );
			} );

		ImGui::TreePop();
	}
}
