#include "Fpch.h"
#include "TypeDrawers.h"
#include "../../External/imgui/imgui.h"
#include "../GameEngine/IComponent.h"
#include "../Renderer/Material.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "../Core/Path.h"
#include "../GameEngine/EngineInstance.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Core/DepotsContainer.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector4 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_DataComponent );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Path );

static ImVec2 GetButtonSize()
{
	return { 0.0f, ImGui::GetFontSize() + 6.0f };
}

void editor::TypeDrawer_Int32::OnDrawValue( const Drawable& drawable ) const
{
	Int32& value = GetValue< Int32 >( drawable.GetAddress() );
	ImGui::InputInt( "##Value", &value );
}

void editor::TypeDrawer_Float::OnDrawValue( const Drawable& drawable ) const
{
	Float& value = GetValue< Float >( drawable.GetAddress() );
	ImGui::InputFloat( "##Value", &value);
}

void editor::TypeDrawer_Vector2::OnDrawValue( const Drawable& drawable ) const
{
	Vector2& value = GetValue< Vector2 >( drawable.GetAddress() );
	ImGui::InputFloat2( "##Value", value.AsArray());
}

void editor::TypeDrawer_Vector3::OnDrawValue( const Drawable& drawable ) const
{
	Vector3& value = GetValue< Vector3 >( drawable.GetAddress() );
	ImGui::InputFloat3( "##Value", value.AsArray());
}

void editor::TypeDrawer_Vector4::OnDrawValue( const Drawable& drawable ) const
{
	Vector4& value = GetValue< Vector4 >( drawable.GetAddress() );
	ImGui::InputFloat4( "##Value", value.AsArray());
}

void editor::TypeDrawer_Array::OnDrawChildren( const Drawable& drawable ) const
{
	const rtti::ContainerType& containerType = static_cast< const rtti::ContainerType& >( drawable.GetType() );
	containerType.VisitElementsAsProperties( drawable.GetAddress(), [ & ]( const rtti::Property& property )
		{
			Draw( GetEngineInstance(), editor::DrawableProperty( property.GetAddress( drawable.GetAddress() ), property ) );
			return rtti::VisitOutcome::Continue;
		} );
}

void editor::TypeDrawer_Vector::OnDrawValue( const Drawable& drawable ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( drawable.GetType() );
	if ( ImGui::Button( "Add" ) )
	{
		containerType.AddDefaultElement( drawable.GetAddress() );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) )
	{
		containerType.Clear( drawable.GetAddress() );
	}
}

void editor::TypeDrawer_Vector::OnDrawChildren( const Drawable& drawable ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( drawable.GetType() );

	Uint32 i = 0u;
	containerType.VisitElementsAsProperties( drawable.GetAddress(), [ & ]( const rtti::Property& property )
		{
			if ( ImGui::BeginTable( "VectorTable", 2) )
			{
				ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
				ImGui::TableSetupColumn( "Property" );

				ImGui::TableNextColumn();
				const Int32 index = i++;
				if ( ImGui::Button( forge::String::Printf( "X##%d", index ).c_str(), { 0, 25.0f } ) )
				{
					containerType.RemoveElementAtIndex( drawable.GetAddress(), index);
				}

				ImGui::TableNextColumn();
				Draw( GetEngineInstance(), editor::DrawableProperty( property.GetAddress( drawable.GetAddress() ), property ) );
				ImGui::EndTable();
			}

			return rtti::VisitOutcome::Continue;
		} );
}

const rtti::Type& editor::TypeDrawer_DataComponent::GetSupportedType() const
{
	return forge::IDataComponent::GetTypeStatic();
}

void editor::TypeDrawer_DataComponent::OnDrawChildren( const Drawable& drawable ) const
{
	forge::IDataComponent* dataComponent = static_cast< forge::IDataComponent* >( drawable.GetAddress() );
	const ecs::Fragment::Type* fragmentType = nullptr;
	if ( void* fragmentAddress = dataComponent->GetMutableRawData( fragmentType ) )
	{
		editor::TypeDrawer::DrawChildren( GetEngineInstance(), editor::DrawableType( fragmentAddress, *fragmentType ) );
	}

	editor::TypeDrawer::OnDrawChildren( drawable );
}

void editor::TypeDrawer_Enum::OnDrawValue( const Drawable& drawable ) const
{
	const rtti::EnumTypeBase& enumType = static_cast< const rtti::EnumTypeBase& >( drawable.GetType() );

	const auto* currentMember = enumType.GetCurrentMember( drawable.GetAddress() );
	FORGE_ASSERT( currentMember );

	if ( ImGui::BeginCombo( "Enum", currentMember->m_name ) )
	{
		Uint32 i = 0u;
		enumType.VisitMembers( [ & ]( const rtti::EnumTypeBase::MemberDesc& member )
			{
				const Bool isSelected = currentMember == &member;
				if ( ImGui::Selectable( member.m_name, isSelected ) )
				{
					enumType.SetCurrentMember( drawable.GetAddress(), member);
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

void editor::TypeDrawer_RawPointer::OnDrawValue( const Drawable& drawable ) const
{
	const rtti::PointerType< void >& pointerType = static_cast< const rtti::PointerType< void >& >( drawable.GetType() );
	if ( void* pointedAddress = GetValue< void* >( drawable.GetAddress() ) )
	{
		if ( ImGui::BeginTable( "PointerTable", 2) )
		{
			ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Property" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "X", GetButtonSize() ) )
			{
				GetValue< void* >( drawable.GetAddress() ) = nullptr;
			}
			else
			{
				ImGui::TableNextColumn();
				Draw( GetEngineInstance(), editor::DrawableType( pointedAddress, pointerType.GetInternalTypeDesc().GetType(), "PointedValue" ) );
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
void DrawSmartPtr( forge::EngineInstance& engineInstance, void* address, const T& pointerType )
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
				editor::TypeDrawer::Draw( engineInstance, editor::DrawableType( pointedAddress, pointerType.GetInternalTypeDesc().GetType(), "PointedValue" ) );
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
				forge::UniqueRawPtr buffer( pointerType.GetInternalTypeDesc().GetType().GetSize() );
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

void editor::TypeDrawer_UniquePointer::OnDrawValue( const Drawable& drawable ) const
{
	DrawSmartPtr( GetEngineInstance(), drawable.GetAddress(), static_cast< const rtti::UniquePtrBaseType& >( drawable.GetType() ) );
}

void editor::TypeDrawer_SharedPointer::OnDrawValue( const Drawable& drawable ) const
{
	DrawSmartPtr( GetEngineInstance(), drawable.GetAddress(), static_cast< const rtti::SharedPtrBaseType& >( drawable.GetType() ));
}

void editor::TypeDrawer_String::OnDrawValue( const Drawable& drawable ) const
{
	std::string* str = static_cast< std::string* >( drawable.GetAddress() );
	forge::imgui::InputText( "##Value", *str );
}

const rtti::Type& editor::TypeDrawer_Path::GetSupportedType() const
{
	return rtti::GetTypeInstanceOf< forge::Path >();
}

void editor::TypeDrawer_Path::OnDrawValue( const Drawable& drawable ) const
{
	forge::Path* m_path = static_cast< forge::Path* >( drawable.GetAddress() );
	ImGui::TextDisabled( m_path->Get() );
	ImGui::SameLine();
	if ( ImGui::Button( "Browse..." ) )
	{
		std::string startPath;
		if ( drawable.HasMetadata( "StartFromDepot" ) )
		{
			startPath = GetEngineInstance().GetDepotsContainer().GetDepotsPath();
		}

		std::vector< std::string > extensions;
		if ( const std::string* extensionsAsString = drawable.GetMetadataValue( "Extensions" ) )
		{
			Uint32 start = 0u;
			for ( Uint32 i = 0u; i < extensionsAsString->size(); ++i )
			{
				if ( ( *extensionsAsString )[ i ] == ',' )
				{
					extensions.emplace_back( extensionsAsString->data() + start, i - start );
					start = i + 1;
				}
			}
			extensions.emplace_back( extensionsAsString->data() + start );
		}

		forge::IWindow::FileDialogType dialogType = forge::IWindow::FileDialogType::Open;
		if ( const std::string* value = drawable.GetMetadataValue("FileDialogType") )
		{
			if ( *value == "Save" )
			{
				dialogType = forge::IWindow::FileDialogType::Save;
			}
		}

		const std::string newPath = GetEngineInstance().GetRenderingManager().GetWindow().CreateFileDialog( dialogType, extensions, startPath );
		if ( !newPath.empty() )
		{
			*m_path = newPath;
		}
	}
}
