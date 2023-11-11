#include "Fpch.h"

#ifdef FORGE_IMGUI_ENABLED
#include "ECSDebug.h"
#include "../../External/imgui/imgui.h"

RTTI_IMPLEMENT_TYPE( ecs::ECSDebug );

ecs::ECSDebug::ECSDebug( forge::EngineInstance& engineInstance )
	: m_engineInstance( &engineInstance )
{
	InitializeDebuggable< ECSDebug >( engineInstance );
}

ecs::ECSDebug::ECSDebug( ECSDebug&& ) = default;
ecs::ECSDebug::ECSDebug() = default;
ecs::ECSDebug::~ECSDebug() = default;

static void DrawArchetype( ecs::ArchetypeView archetype, Uint32 index )
{
	ImGui::Text( "Archetype%u", index );
	ImGui::Text( "\tEntities amount : %u", archetype.GetEntitiesAmount() );
	{
		const auto& fragments = archetype.GetArchetypeID().GetFragmentsFlags();
		std::string buffer;
		Uint32 fragmentsSize = 0u;
		fragments.VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
			{
				buffer += type.GetName();
				buffer += "[" + std::to_string( type.GetSize() ) + "B]";
				buffer += ", ";

				fragmentsSize += static_cast< Uint32 >( type.GetSize() );
			} );
		ImGui::Text( "\tFragments : [%s]", buffer.size() > 0u ? buffer.substr( 0u, buffer.size() - 2 ).c_str() : "None" );
		ImGui::Text( "\tArchetype Size: %u x %uB = %uB", archetype.GetEntitiesAmount(), fragmentsSize, archetype.GetEntitiesAmount() * fragmentsSize );
	}

	{
		const auto& tags = archetype.GetArchetypeID().GetTagsFlags();
		std::string buffer;
		tags.VisitSetTypes( [ & ]( const ecs::Tag::Type& type )
			{
				buffer += type.GetName();
				buffer += ", ";
			} );
		ImGui::Text( "\tTags : [%s]", buffer.size() > 0u ? buffer.substr( 0u, buffer.size() - 2 ).c_str() : "None" );
	}
}

void ecs::ECSDebug::OnRenderDebug()
{
	if ( ImGui::Begin( "ECS" ) )
	{
		Uint32 i = 0u;
		m_engineInstance->GetECSManager().VisitAllArchetypes( FragmentsFlags(), [ & ]( ecs::ArchetypeView archetype )
			{
				DrawArchetype( archetype, i++ );
			} );
	}
	ImGui::End();
}
#endif