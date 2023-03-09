#include "Fpch.h"

#ifdef FORGE_IMGUI_ENABLED
#include "ECSDebug.h"
#include "../../External/imgui/imgui.h"

IMPLEMENT_TYPE( ecs::ECSDebug );

ecs::ECSDebug::ECSDebug( forge::EngineInstance& engineInstance )
	: m_engineInstance( &engineInstance )
{
	InitializeDebuggable< ECSDebug >( engineInstance );
}

ecs::ECSDebug::ECSDebug( ECSDebug&& ) = default;
ecs::ECSDebug::ECSDebug() = default;
ecs::ECSDebug::~ECSDebug() = default;

static void DrawArchetype( const ecs::Archetype& archetype, Uint32 index )
{
	ImGui::Text( "Archetype%u", index );
	ImGui::Text( "\tEntities amount : %u", archetype.GetEntitiesAmount() );
	{
		const auto& fragments = archetype.GetArchetypeID().m_fragmentsFlags;
		std::string buffer;
		for ( Uint32 i = 0u; i < fragments.size(); ++i )
		{
			if ( fragments.test( i ) )
			{
				const ecs::Fragment::Type* type = ecs::Fragment::GetDebugFragmentTypeFromIndex( i );
				if ( i + 1u != fragments.size() )
				{
					buffer += type->GetName();
					buffer += ", ";
				}
			}
		}
		ImGui::Text( "\tFragments : [%s]", buffer.size() > 0u ? buffer.substr( 0u, buffer.size() - 2 ).c_str() : "None" );
	}

	{
		const auto& tags = archetype.GetArchetypeID().m_tagsFlags;;
		std::string buffer;
		for ( Uint32 i = 0u; i < tags.size(); ++i )
		{
			if ( tags.test( i ) )
			{
				const ecs::Tag::Type* type = ecs::Tag::GetDebugTagTypeFromIndex( i );
				if ( i + 1u != tags.size() )
				{
					buffer += type->GetName();
					buffer += ", ";
				}
			}
		}
		ImGui::Text( "\tTags : [%s]", buffer.size() > 0u ? buffer.substr( 0u, buffer.size() - 2 ).c_str() : "None" );
	}
}

void ecs::ECSDebug::OnRenderDebug()
{
	if ( ImGui::Begin( "ECS" ) )
	{
		const auto archetypes = m_engineInstance->GetECSManager().GetArchetypes();
		for ( Uint32 i = 0u; i < archetypes.GetSize(); ++i )
		{
			DrawArchetype( *archetypes[ i ], i );
		}
	}
	ImGui::End();
}
#endif