#include "Fpch.h"
#include "NavmeshSystem.h"
#include "../ECS/Query.h"
#include "NavmeshVolume.h"
#include "TransformComponent.h"
#include "../GameEngine/EngineInstance.h"
#include "DebugSystem.h"
#include "PhysicsSystem.h"
#include "../Core/Utils.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#endif

RTTI_IMPLEMENT_TYPE( forge::ai::NavmeshSystem );

#pragma optimize("", off)

void forge::ai::NavmeshSystem::OnInitialize()
{
	InitializeDebuggable< NavmeshSystem >( GetEngineInstance() );
}

#ifdef FORGE_IMGUI_ENABLED
void forge::ai::NavmeshSystem::OnRenderDebug()
{
	ImGui::Begin( "NavmeshSystem" );

	ImGui::SliderFloat( "Max Cell Size", &m_debugSettings.m_maxCellSize, 0.01f, 0.5f );
	ImGui::SliderFloat( "Agent Radius", &m_debugSettings.m_agentRadius, 0.01f, m_debugSettings.m_agentHeight * 0.5f );
	ImGui::SliderFloat( "Agent Height", &m_debugSettings.m_agentHeight, m_debugSettings.m_agentRadius * 2.0f, 2.0f );
	ImGui::SliderFloat( "Max Step", &m_debugSettings.m_maxStep, 0.0f, 1.0f );

	m_debugSettings.m_agentRadius = Math::Min( m_debugSettings.m_agentRadius, m_debugSettings.m_agentHeight * 0.5f );
	m_debugSettings.m_agentHeight = Math::Max( m_debugSettings.m_agentHeight, m_debugSettings.m_agentRadius * 2.0f );


	if ( ImGui::Button( "Generate" ) )
	{
		Generate( m_debugSettings.m_maxCellSize, m_debugSettings.m_agentRadius, m_debugSettings.m_agentHeight, m_debugSettings.m_maxStep );
	}

	ImGui::End();
}

void forge::ai::NavmeshSystem::Generate( Float maxCellSize, Float agentRadius, Float agentHeight, Float maxStep )
{
	auto& objectManager = GetEngineInstance().GetObjectsManager();
	auto& debugSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >();
	auto& physicsSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::PhysicsSystem >();

	ecs::Query query( GetEngineInstance().GetECSManager() );
	query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	query.AddTagRequirement< forge::ai::NavmeshVolume::NavmeshVolumeTag >( ecs::Query::RequirementType::Included );

	query.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				const auto& transformFragment = transformFragments[ i ];
				const Transform& transform = transformFragment.m_transform;
				const Vector3 halfExtents = transform.GetOrientation() * transformFragment.m_scale * 0.5f;
				const Vector3 minCorner = transform.GetPosition3() - halfExtents;
				const Vector3 maxCorner = transform.GetPosition3() + halfExtents;

				const Float xAmount = std::ceilf( transformFragment.m_scale.X / maxCellSize );
				const Float yAmount = std::ceilf( transformFragment.m_scale.Y / maxCellSize );
				const Float zAmount = std::ceilf( transformFragment.m_scale.Z / maxCellSize );

				enum class VoxelType : Uint8
				{
					NonOverlapping,
					Overlapping,
					Walkable,
					Border,
				};

				std::vector< VoxelType > voxels;
				voxels.resize( static_cast< Uint32 >( zAmount ) * static_cast< Uint32 >( yAmount ) * static_cast< Uint32 >( xAmount ), VoxelType::NonOverlapping );

				auto GetVoxel = [ & ]( Uint32 x, Uint32 y, Uint32 z ) -> VoxelType&
					{
						return voxels[ ( y + z * static_cast< Uint32 >( yAmount ) ) * static_cast< Uint32 >( xAmount ) + x ];
					};

				const Float xSize = transformFragment.m_scale.X / xAmount;
				const Float ySize = transformFragment.m_scale.Y / yAmount;
				const Float zSize = transformFragment.m_scale.Z / zAmount;
				const Vector3 voxelExtents( xSize, ySize, zSize );
				
				struct Voxel
				{
					Uint32 m_x = 0;
					Uint32 m_y = 0;
					Uint32 m_z = 0;
				};

				auto hash = []( const Voxel& voxel ) { return Math::CombineHashes( Math::CalculateHash( voxel.m_x ), Math::CalculateHash( voxel.m_y ), Math::CalculateHash( voxel.m_z ) ); };
				auto equal = []( const Voxel& left, const Voxel& right ) { return left.m_x == right.m_x && left.m_y == right.m_y && left.m_z == right.m_z; };

				using Walkables = std::unordered_set< Voxel, decltype( hash ), decltype( equal ) >;
				Walkables walkables{ voxels.size(), hash, equal };

				// Visiting each voxel and checking if it collides. If it's not, but neighbour below does, check if agent's capsule would fit.
				// If it does, mark voxel as walkable
				for ( Uint32 z = 0u; z < static_cast< Uint32 >( zAmount ); ++z )
				{
					for ( Uint32 y = 0u; y < static_cast< Uint32 >( yAmount ); ++y )
					{
						for ( Uint32 x = 0u; x < static_cast< Uint32 >( xAmount ); ++x )
						{
							Vector3 offset( ( static_cast< Float >( x ) + 0.5f ) * xSize, ( static_cast< Float >( y ) + 0.5f ) * ySize, ( static_cast< Float >( z ) + 0.5f ) * zSize );
							offset = transform.GetOrientation() * offset;

							const Vector3 voxelPos = minCorner + offset;
							const Transform voxelTransform( voxelPos, transform.GetOrientation() );

							const bool isOverlapping = physicsSystem.PerformCubeOverlap( voxelTransform, voxelExtents * 0.5f );
							GetVoxel( x, y, z ) = isOverlapping ? VoxelType::Overlapping : VoxelType::NonOverlapping;
							if ( z > 0 && !isOverlapping && GetVoxel( x, y, z - 1 ) == VoxelType::Overlapping )
							{
								const Transform belowVoxelTransform( voxelTransform.GetPosition3() - Vector3::EZ() * zSize, voxelTransform.GetOrientation() );
								const float marginFromVoxel = zSize * 0.1f;
								const Transform agentTransform( belowVoxelTransform.GetPosition3() + Vector3::EZ() * ( zSize * 0.5f + agentHeight * 0.5f + marginFromVoxel ), transform.GetOrientation() );

								if ( !physicsSystem.PerformCapsuleOverlap( agentTransform, agentRadius, agentHeight ) )
								{
									GetVoxel( x, y, z - 1 ) = VoxelType::Walkable;
									walkables.insert( { x, y, z - 1 } );
								}
							}
						}
					}
				}

				struct Island
				{
					std::vector< Voxel > m_border;
					std::vector< Uint8 > m_borderWalkableNeighbourAmounts;
				};

				std::vector< Island > islands;

				struct Vector2D
				{
					Int32 m_x = 0;
					Int32 m_y = 0;
				};

				constexpr Vector2D c_offsets[] =
				{
					{ -1, 1 },  { 0, 1 },   { 1, 1 },
					{ -1, 0 },             { 1, 0 },
					{ -1, -1 }, { 0, -1 }, { 1, -1 },
				};

				while ( !walkables.empty() )
				{
					islands.emplace_back();
					Island& island = islands.back();
					std::vector< Voxel > queue;
					queue.push_back( *walkables.begin() );
					walkables.erase( walkables.begin() );

					while ( !queue.empty() )
					{
						const Voxel walkable = queue.back();
						queue.pop_back();

						const Int32 maxZ = Math::Max( 0, static_cast< Int32 >( std::ceil( maxStep / zSize ) ) );
						bool isBorder = false;
						Uint32 walkableNeighbours = 0u;
						for ( const Vector2D& offset : c_offsets )
						{
							const Int32 neighbourX = static_cast< Int32 >( walkable.m_x ) + offset.m_x;
							const Int32 neighbourY = static_cast< Int32 >( walkable.m_y ) + offset.m_y;

							bool walkableNeighbour = false;
							if ( neighbourX >= 0 && neighbourY >= 0 && neighbourX < xAmount && neighbourY < yAmount )
							{
								const Int32 walkableZ = static_cast< Int32 >( walkable.m_z );
								for ( Int32 z = Math::Max( 0, walkableZ - maxZ ); z <= Math::Min( walkableZ + maxZ, static_cast< Int32 >( zAmount ) - 1 ); ++z )
								{
									const Voxel neighbour{ walkable.m_x + offset.m_x, walkable.m_y + offset.m_y, static_cast< Uint32 >( z ) };
									const VoxelType neighbourType = GetVoxel( neighbour.m_x, neighbour.m_y, neighbour.m_z );
									if ( neighbourType == VoxelType::Walkable || neighbourType == VoxelType::Border )
									{
										walkableNeighbour = true;
										if ( walkables.contains( neighbour ) )
										{
											queue.push_back( neighbour );
											walkables.erase( neighbour );
										}

										break;
									}
								}
							}

							isBorder |= !walkableNeighbour;
							if ( walkableNeighbour )
							{
								++walkableNeighbours;
							}
						}

						if ( isBorder )
						{
							island.m_border.emplace_back( walkable );
							island.m_borderWalkableNeighbourAmounts.push_back( walkableNeighbours );
						}
					}
				}

				// Removing too small islands
				for ( Island& island : islands )
				{
					std::unordered_set<Uint64> border;
					for ( const Voxel& voxel : island.m_border )
					{
						border.insert( static_cast< Uint64 >( voxel.m_x ) << 32 | voxel.m_y );
					}

					FORGE_ASSERT( island.m_border.size() == island.m_borderWalkableNeighbourAmounts.size() );
					for ( Int32 voxelIndex = static_cast< Int32 >( island.m_border.size() ) - 1; voxelIndex >= 0; --voxelIndex )
					{
						const Voxel& voxel = island.m_border[ voxelIndex ];

						Int32 nonBorderNeighboursAmount = -island.m_borderWalkableNeighbourAmounts[ voxelIndex ];
						for ( const Vector2D& offset : c_offsets )
						{
							const Int32 neighbourX = static_cast< Int32 >( voxel.m_x ) + offset.m_x;
							const Int32 neighbourY = static_cast< Int32 >( voxel.m_y ) + offset.m_y;

							const Uint64 neighbourKey = static_cast< Uint64 >( neighbourX ) << 32 | static_cast< Uint64 >( neighbourY );
							if ( border.contains( neighbourKey ) )
							{
								++nonBorderNeighboursAmount;
							}
						}

						if ( nonBorderNeighboursAmount >= 0 )
						{
							forge::utils::RemoveReorder( island.m_border, voxelIndex );
						}
					}
				}

				// Removing too small islands
				for ( Int32 islandIndex = static_cast< Int32 >( islands.size() ) - 1; islandIndex >= 0; --islandIndex )
				{
					Island& island = islands[ islandIndex ];
					constexpr Float minSize = 1.0f;
					if ( static_cast< Float >( island.m_border.size() ) * xSize * ySize < minSize )
					{
						forge::utils::RemoveReorder( islands, islandIndex );
					}
				}

				for ( Int32 islandIndex = 0; islandIndex < islands.size(); ++islandIndex )
				{
					systems::DebugSystem::DebugDrawParams params;
					params.m_lifetime = 15.0f;
					Math::Random rng{ static_cast< Uint32 >( islandIndex ) };
					params.m_color = LinearColor{ rng.GetFloat(), rng.GetFloat(), rng.GetFloat() };

					Island& island = islands[ islandIndex ];

					for ( const Voxel& voxel : island.m_border )
					{
						GetVoxel( voxel.m_x, voxel.m_y, voxel.m_z ) = VoxelType::Border;

						Vector3 offset( ( static_cast< Float >( voxel.m_x ) + 0.5f )* xSize, ( static_cast< Float >( voxel.m_y ) + 0.5f )* ySize, ( static_cast< Float >( voxel.m_z ) + 0.5f )* zSize );
						offset = transform.GetOrientation() * offset;

						const Vector3 voxelPos = minCorner + offset;
						const Transform voxelTransform( voxelPos, transform.GetOrientation() );
						debugSystem.DrawCube( voxelTransform, voxelExtents, params );
					}
				}
			}
		} );

	// I think system could add an object with custom serialization to scene
}
#endif

#pragma optimize("", on)