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
#include "../Core/PropertiesChain.h"

RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Int32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Uint32 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Float );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector2 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector3 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Vector4 );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_DataComponent );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_Path );
RTTI_IMPLEMENT_TYPE( editor::TypeDrawer_LinearColor );

static ImVec2 GetButtonSize()
{
	return { 0.0f, ImGui::GetFontSize() + 6.0f };
}

static void BroadcastPropertyChangedCallback( const editor::TypeDrawer::Drawable& drawable )
{
	forge::PropertiesChain chain;
	const editor::TypeDrawer::Drawable* firstDrawable = &drawable;

	std::function< void( const editor::TypeDrawer::Drawable& ) > visitDrawable;
	visitDrawable = [ & ]( const editor::TypeDrawer::Drawable& drawable )
	{
		if ( const auto* parentDrawable = drawable.GetParent() )
		{
			firstDrawable = parentDrawable;
			if ( parentDrawable->GetName() != nullptr )
			{
				visitDrawable( *parentDrawable );
			}

			const Char* propertyName = drawable.GetName();
			FORGE_ASSERT( propertyName );

			const rtti::Property* property = parentDrawable->GetType().FindProperty( propertyName );
			FORGE_ASSERT( property );

			chain.Add( *property );
		}
	};

	visitDrawable( drawable );

	auto tryToBroadcastCallbackOnType = [ & ]( const rtti::Type& type, void* address )
	{
		if ( const rtti::Function* func = type.FindMethod( "OnPropertyChanged" ) )
		{
			if ( func->GetParametersAmount() == 1 && func->GetReturnTypeDesc() == nullptr )
			{
				const rtti::ParameterTypeDesc* paramDesc = func->GetParameterTypeDesc( 0 );

				if ( paramDesc->GetType() == forge::PropertiesChain::GetTypeStatic() && paramDesc->HasFlags( rtti::InstanceFlags::Const | rtti::InstanceFlags::Ref ) )
				{
					func->Call( address, &chain, nullptr );
				}
			}
		}
	};

	tryToBroadcastCallbackOnType( firstDrawable->GetType(), firstDrawable->GetAddress() );

	void* currentAddress = firstDrawable->GetAddress();
	for ( const rtti::Property* property : chain.Get() )
	{
		currentAddress = property->GetAddress( currentAddress );
		tryToBroadcastCallbackOnType( property->GetType(), currentAddress );
	}
}

void editor::TypeDrawer_Int32::OnDrawValue( const Drawable& drawable ) const
{
	Int32& value = GetValue< Int32 >( drawable.GetAddress() );
	if ( ImGui::InputInt( "##Value", &value ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Float::OnDrawValue( const Drawable& drawable ) const
{
	Float& value = GetValue< Float >( drawable.GetAddress() );
	Float min = -std::numeric_limits< Float >::infinity();
	Float max = std::numeric_limits< Float >::infinity();
	if ( const std::string* minValue = drawable.GetMetadataValue( "RangeMin" ) )
	{
		min = std::stof( *minValue );
	}

	if ( const std::string* maxValue = drawable.GetMetadataValue( "RangeMax" ) )
	{
		max = std::stof( *maxValue );
	}

	bool bModified = false;
	if ( drawable.HasMetadata( "IsRadiansAngle" ) )
	{
		min = Math::Max( RAD2DEG * min, -180.0f );
		max = Math::Min( RAD2DEG * max, 180.0f );
		bModified = ImGui::SliderAngle( "##Value", &value, min, max );
	}
	else
	{
		bModified = ImGui::InputFloat( "##Value", &value );
	}

	if ( bModified )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Vector2::OnDrawValue( const Drawable& drawable ) const
{
	Vector2& value = GetValue< Vector2 >( drawable.GetAddress() );
	if ( ImGui::InputFloat2( "##Value", value.AsArray() ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Vector3::OnDrawValue( const Drawable& drawable ) const
{
	Vector3& value = GetValue< Vector3 >( drawable.GetAddress() );
	if ( ImGui::InputFloat3( "##Value", value.AsArray() ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Vector4::OnDrawValue( const Drawable& drawable ) const
{
	Vector4& value = GetValue< Vector4 >( drawable.GetAddress() );
	if ( ImGui::InputFloat4( "##Value", value.AsArray() ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Array::OnDrawChildren( const Drawable& drawable ) const
{
	const rtti::ContainerType& containerType = static_cast< const rtti::ContainerType& >( drawable.GetType() );
	containerType.VisitElementsAsProperties( drawable.GetAddress(), [ & ]( const rtti::Property& property )
		{
			Draw( GetEngineInstance(), editor::DrawableProperty( property.GetAddress( drawable.GetAddress() ), property, drawable ) );
			return rtti::VisitOutcome::Continue;
		} );
}

void editor::TypeDrawer_Vector::OnDrawValue( const Drawable& drawable ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( drawable.GetType() );
	if ( ImGui::Button( "Add" ) )
	{
		containerType.AddDefaultElement( drawable.GetAddress() );
		BroadcastPropertyChangedCallback( drawable );
	}
	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) )
	{
		containerType.Clear( drawable.GetAddress() );
		BroadcastPropertyChangedCallback( drawable );
	}
}

void editor::TypeDrawer_Vector::OnDrawChildren( const Drawable& drawable ) const
{
	const rtti::DynamicContainerType& containerType = static_cast< const rtti::DynamicContainerType& >( drawable.GetType() );

	Uint32 i = 0u;
	containerType.VisitElementsAsProperties( drawable.GetAddress(), [ & ]( const rtti::Property& property )
		{
			if ( ImGui::BeginTable( "VectorTable", 2 ) )
			{
				ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
				ImGui::TableSetupColumn( "Property" );

				ImGui::TableNextColumn();
				const Int32 index = i++;
				const Bool clickedRemove = ImGui::Button( forge::String::Printf( "X##%d", index ).c_str(), { 0, 25.0f } );

				ImGui::TableNextColumn();
				Draw( GetEngineInstance(), editor::DrawableProperty( property.GetAddress( drawable.GetAddress() ), property, drawable, index ) );
				ImGui::EndTable();

				if ( clickedRemove )
				{
					containerType.RemoveElementAtIndex( drawable.GetAddress(), index );
					BroadcastPropertyChangedCallback( drawable );
				}
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
	if ( void* fragmentAddress = dataComponent->GetMutableRawData() )
	{
		editor::TypeDrawer::DrawChildren( GetEngineInstance(), editor::DrawableType( fragmentAddress, dataComponent->GetDataType() ) );
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
					enumType.SetCurrentMember( drawable.GetAddress(), member );
					BroadcastPropertyChangedCallback( drawable );
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
		if ( ImGui::BeginTable( "PointerTable", 2 ) )
		{
			ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Property" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "X", GetButtonSize() ) )
			{
				GetValue< void* >( drawable.GetAddress() ) = nullptr;
				BroadcastPropertyChangedCallback( drawable );
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
void DrawSmartPtr( forge::EngineInstance& engineInstance, const editor::TypeDrawer::Drawable& drawable )
{
	void* address = drawable.GetAddress();
	const T& pointerType = static_cast< const T& >( drawable.GetType() );

	if ( void* pointedAddress = pointerType.GetPointedAddress( address ) )
	{
		if ( ImGui::BeginTable( "SmartPtr", 2 ) )
		{
			ImGui::TableSetupColumn( "RemoveButton", ImGuiTableColumnFlags_WidthFixed );
			ImGui::TableSetupColumn( "Property" );

			ImGui::TableNextColumn();
			if ( ImGui::Button( "X", GetButtonSize() ) )
			{
				pointerType.SetPointedAddress( address, nullptr );
				BroadcastPropertyChangedCallback( drawable );
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
		if ( ImGui::BeginTable( "SmartPtr", 2 ) )
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
				BroadcastPropertyChangedCallback( drawable );
			}

			ImGui::TableNextColumn();
			ImGui::Text( "NULL" );
			ImGui::EndTable();
		}
	}
}

void editor::TypeDrawer_UniquePointer::OnDrawValue( const Drawable& drawable ) const
{
	DrawSmartPtr< const rtti::UniquePtrBaseType >( GetEngineInstance(), drawable );
}

void editor::TypeDrawer_SharedPointer::OnDrawValue( const Drawable& drawable ) const
{
	DrawSmartPtr< const rtti::SharedPtrBaseType >( GetEngineInstance(), drawable );
}

void editor::TypeDrawer_String::OnDrawValue( const Drawable& drawable ) const
{
	std::string* str = static_cast< std::string* >( drawable.GetAddress() );
	if ( forge::imgui::InputText( "##Value", *str ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}
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
		forge::Path startPath;
		if ( drawable.HasMetadata( "InDepot" ) )
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
		if ( const std::string* value = drawable.GetMetadataValue( "FileDialogType" ) )
		{
			if ( *value == "Save" )
			{
				dialogType = forge::IWindow::FileDialogType::Save;
			}
		}

		forge::Path newPath = GetEngineInstance().GetRenderingManager().GetWindow().CreateFileDialog( dialogType, extensions, startPath );
		
		if ( drawable.HasMetadata( "InDepot" ) )
		{
			if ( newPath.AsString().find( startPath.Get() ) != std::string::npos )
			{
				newPath = newPath = forge::Path( newPath.Get() + strlen( startPath.Get() ) );
			}
			else
			{
				newPath = {};
			}
		}
		
		if ( !newPath.IsEmpty() )
		{
			if ( dialogType == forge::IWindow::FileDialogType::Save && extensions.size() == 1u )
			{
				newPath.Append( forge::Path( forge::String::Printf( ".%s", newPath.Get(), extensions[ 0 ].c_str() ) ) );
			}

			*m_path = newPath;
			BroadcastPropertyChangedCallback( drawable );
		}
	}
}

void editor::TypeDrawer_Uint32::OnDrawValue( const Drawable& drawable ) const
{
	Uint32& value = GetValue< Uint32 >( drawable.GetAddress() );

	Int32 valueAsInt = static_cast< Int32 >( value );
	if ( ImGui::InputInt( "##Value", &valueAsInt ) )
	{
		value = Math::Max( 0, valueAsInt );
		BroadcastPropertyChangedCallback( drawable );
	}	
}

const rtti::Type& editor::TypeDrawer_LinearColor::GetSupportedType() const
{
	return LinearColor::GetTypeStatic();
}

void editor::TypeDrawer_LinearColor::OnDrawValue( const Drawable& drawable ) const
{
	LinearColor& value = GetValue< LinearColor >( drawable.GetAddress() );
	if ( ImGui::ColorEdit4( "##Value", &value.R ) )
	{
		BroadcastPropertyChangedCallback( drawable );
	}

}
