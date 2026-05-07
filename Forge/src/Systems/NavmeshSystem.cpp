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

					bool operator==(const Voxel&) const = default;
				};

				auto hash = []( const Voxel& voxel ) { return Math::CombineHashes( Math::CalculateHash( voxel.m_x ), Math::CalculateHash( voxel.m_y ), Math::CalculateHash( voxel.m_z ) ); };

				using Walkables = std::unordered_set< Voxel, decltype( hash ) >;
				Walkables walkables{ voxels.size(), hash };

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

				struct Vector2D
				{
					Int32 m_x = 0;
					Int32 m_y = 0;

					bool operator==( const Vector2D& ) const = default;
				};

				constexpr std::array c_offsets
				{
					Vector2D{ 0, 1 }, Vector2D{ 1, 0 }, Vector2D{ 0, -1 }, Vector2D{ -1, 0 }
				};

				struct VoxelIsland
				{
					std::vector< Voxel > m_border;
					std::vector< Uint8 > m_walkableNeighbourMasks;
				};

				std::vector< VoxelIsland > voxelIslands;

				while ( !walkables.empty() )
				{
					voxelIslands.emplace_back();
					VoxelIsland& island = voxelIslands.back();
					std::vector< Voxel > queue;
					queue.push_back( *walkables.begin() );
					walkables.erase( walkables.begin() );

					while ( !queue.empty() )
					{
						const Voxel walkable = queue.back();
						queue.pop_back();

						const Int32 maxZ = Math::Max( 0, static_cast< Int32 >( std::ceil( maxStep / zSize ) ) );
						bool isBorder = false;
						Uint8 walkableNeighboursMask = 0u;
						for ( Uint32 offsetIndex = 0u; offsetIndex < c_offsets.size(); ++offsetIndex )
						{
							const Vector2D& offset = c_offsets[ offsetIndex ];
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
								walkableNeighboursMask |= 1 << offsetIndex;
							}
						}

						if ( isBorder )
						{
							island.m_border.emplace_back( walkable );
							island.m_walkableNeighbourMasks.emplace_back( walkableNeighboursMask );
						}
					}
				}

				for ( VoxelIsland& island : voxelIslands )
				{
					std::unordered_set< Uint64 > border;
					for ( const Voxel& voxel : island.m_border )
					{
						border.insert( static_cast< Uint64 >( voxel.m_x ) << 32 | voxel.m_y );
					}

					FORGE_ASSERT( island.m_border.size() == island.m_walkableNeighbourMasks.size() );
					for ( Int32 voxelIndex = static_cast< Int32 >( island.m_border.size() ) - 1; voxelIndex >= 0; --voxelIndex )
					{
						const Voxel& voxel = island.m_border[ voxelIndex ];

						if ( voxel.m_x == 0 || voxel.m_y == 0 || voxel.m_x == xAmount - 1 || voxel.m_y == yAmount - 1 )
						{
							continue;
						}

						Uint8 nonBorderNeighboursMask = island.m_walkableNeighbourMasks[ voxelIndex ];
						for ( Uint32 offsetIndex = 0u; offsetIndex < c_offsets.size(); ++offsetIndex )
						{
							const Vector2D& offset = c_offsets[ offsetIndex ];
							const Int32 neighbourX = static_cast< Int32 >( voxel.m_x ) + offset.m_x;
							const Int32 neighbourY = static_cast< Int32 >( voxel.m_y ) + offset.m_y;

							const Uint64 neighbourKey = static_cast< Uint64 >( neighbourX ) << 32 | static_cast< Uint64 >( neighbourY );
							if ( border.contains( neighbourKey ) )
							{
								nonBorderNeighboursMask &= ~( 1 << offsetIndex );
							}
						}

						if ( nonBorderNeighboursMask == 0)
						{
							//forge::utils::RemoveReorder( island.m_border, voxelIndex );
							//forge::utils::RemoveReorder( island.m_walkableNeighbourMasks, voxelIndex );
						}
					}
				}

				// Removing too small islands
				for ( Int32 islandIndex = static_cast< Int32 >( voxelIslands.size() ) - 1; islandIndex >= 0; --islandIndex )
				{
					VoxelIsland& island = voxelIslands[ islandIndex ];
					constexpr Float minSize = 1.0f;
					if ( static_cast< Float >( island.m_border.size() ) * xSize * ySize < minSize )
					{
						forge::utils::RemoveReorder( voxelIslands, islandIndex );
					}
				}
				
				struct VoxelEdge
				{
					Voxel m_voxel;
					Uint8 m_offsetIndex = 0u;

					bool operator==( const VoxelEdge& ) const = default;
				};

				struct Shape
				{
					std::vector< VoxelEdge > m_edges;
				};

				struct Island
				{
					std::vector< Shape > m_shapes;
				};

				std::vector< Island > islands;

				for ( const VoxelIsland& island : voxelIslands )
				{
					auto hasher = []( const VoxelEdge& edge )
						{
							return Math::CombineHashes( 
								Math::CalculateHash( edge.m_voxel.m_x ), 
								Math::CalculateHash( edge.m_voxel.m_y ),
								Math::CalculateHash( edge.m_offsetIndex ) );
						};

					std::unordered_map< Uint64, VoxelEdge > queue;
					islands.emplace_back();

					FORGE_ASSERT( island.m_border.size() == island.m_walkableNeighbourMasks.size() );
					for ( Uint32 borderIndex = 0u; borderIndex < island.m_border.size(); ++borderIndex )
					{
						for ( Uint32 offsetIndex = 0u; offsetIndex < c_offsets.size(); ++offsetIndex )
						{
							if ( ( island.m_walkableNeighbourMasks[ borderIndex ] & ( 1 << offsetIndex ) ) == 0 )
							{
								VoxelEdge edge { island.m_border[ borderIndex ], static_cast< Uint8 >( offsetIndex ) };
								queue.emplace( hasher( edge ), edge );
							}
						}
					}

					while ( !queue.empty() )
					{
						auto& shapes = islands[ islands.size() - 1 ].m_shapes;
						shapes.emplace_back();
						Shape& shape = shapes[ shapes.size() - 1 ];
						std::unordered_set< Uint64 > visitedEdges;
						VoxelEdge currentEdge = queue.begin()->second;
						Uint64 currentHash = hasher( currentEdge );
						Uint8 currentDirIdx = ( currentEdge.m_offsetIndex + 1 ) % c_offsets.size();

						constexpr Uint8 c_upIdx = 0;
						constexpr Uint8 c_rightIdx = 1;
						constexpr Uint8 c_downIdx = 2;
						constexpr Uint8 c_leftIdx = 3;

						while ( !visitedEdges.contains( currentHash ) )
						{
							visitedEdges.emplace( currentHash );
							queue.erase( currentHash );
							shape.m_edges.emplace_back( currentEdge );

							// self
							{
								{
									VoxelEdge tmp = currentEdge;
									tmp.m_offsetIndex = ( currentEdge.m_offsetIndex + 1 ) % c_offsets.size();
									
									const Uint64 tmpHash = hasher( tmp );
									auto it = queue.find( tmpHash );
									if ( it != queue.end() )
									{
										currentDirIdx = ( currentDirIdx + 1 ) % c_offsets.size();
										currentEdge = it->second;
										currentHash = tmpHash;
										continue;
									}
								}

								{
									VoxelEdge tmp = currentEdge;
									tmp.m_offsetIndex = ( currentEdge.m_offsetIndex + c_offsets.size() - 1 ) % c_offsets.size();
									
									const Uint64 tmpHash = hasher( tmp );
									auto it = queue.find( tmpHash );
									if ( it != queue.end() )
									{
										currentDirIdx = ( currentDirIdx + c_offsets.size() - 1 ) % c_offsets.size();
										currentEdge = it->second;
										currentHash = tmpHash;
										continue;
									}
								}
							}

							// neighbour, same dir
							{
								VoxelEdge tmp = currentEdge;
								tmp.m_voxel.m_x += c_offsets[ currentDirIdx ].m_x;
								tmp.m_voxel.m_y += c_offsets[ currentDirIdx ].m_y;

								const Uint64 tmpHash = hasher( tmp );
								auto it = queue.find( tmpHash );
								if ( it != queue.end() )
								{
									currentEdge = it->second;
									currentHash = tmpHash;
									continue;
								}
							}

							// diagonal neighbour, dir change
							{
								/*
								[ ][ ][X]
								[X][X][ ]
								[ ][ ][ ]

								[ ][ ][ ]
								[X][X][ ]
								[ ][ ][X]

								[ ][X][ ]
								[ ][X][ ]
								[X][ ][ ]
								*/

								{
									VoxelEdge tmp = currentEdge;
									const Vector2D offset0 = c_offsets[ currentDirIdx ];
									const Vector2D offset1 = c_offsets[ currentEdge.m_offsetIndex ];
									tmp.m_voxel.m_x += offset0.m_x + offset1.m_x;
									tmp.m_voxel.m_y += offset0.m_y + offset1.m_y;
									tmp.m_offsetIndex = ( currentDirIdx + 2 ) % c_offsets.size();

									Uint64 tmpHash = hasher( tmp );
									if ( queue.find( tmpHash ) != queue.end() )
									{
										currentDirIdx = currentEdge.m_offsetIndex;
										currentEdge = tmp;
										currentHash = tmpHash;
										continue;
									}
								}
							}

							//FORGE_ASSERT( false );
						}
					}
				}

				for ( Int32 islandIndex = 0; islandIndex < islands.size(); ++islandIndex )
				{
					systems::DebugSystem::DebugDrawParams params;
					params.m_lifetime = 999999.0f; //15.0f;

					const Island& island = islands[ islandIndex ];

					for ( Int32 shapeIndex = 0; shapeIndex < island.m_shapes.size(); ++shapeIndex )
					{
						Math::Random rng{ static_cast< Uint32 >( Math::CombineHashes( Math::CalculateHash( islandIndex ), Math::CalculateHash( shapeIndex ) ) ) };
						params.m_color = LinearColor{ rng.GetFloat(), rng.GetFloat(), rng.GetFloat() };

						const Shape& shape = island.m_shapes[ shapeIndex ];
						const VoxelEdge* start = &shape.m_edges[ 0 ];
						const VoxelEdge* end = start;

						auto draw = [ & ]()
							{
								Vector3 startOffset( ( static_cast< Float >( start->m_voxel.m_x ) + 0.5f ) * xSize, ( static_cast< Float >( start->m_voxel.m_y ) + 0.5f ) * ySize, ( static_cast< Float >( start->m_voxel.m_z ) + 0.5f ) * zSize );
								startOffset = transform.GetOrientation() * startOffset;

								Vector3 endOffset( ( static_cast< Float >( end->m_voxel.m_x ) + 0.5f ) * xSize, ( static_cast< Float >( end->m_voxel.m_y ) + 0.5f ) * ySize, ( static_cast< Float >( end->m_voxel.m_z ) + 0.5f ) * zSize );
								endOffset = transform.GetOrientation() * endOffset;

								const Vector3 startVoxelPos = minCorner + startOffset;
								const Vector3 endVoxelPos = minCorner + endOffset;
								if ( startVoxelPos != endVoxelPos )
								{
									debugSystem.DrawLine( startVoxelPos, endVoxelPos, 0.05f, params );
								}	
							};

						for ( Uint32 edgeIndex = 0; edgeIndex < shape.m_edges.size(); ++edgeIndex )
						{
							const VoxelEdge& current = shape.m_edges[ ( edgeIndex + 1 ) % shape.m_edges.size() ];
							if ( start->m_offsetIndex == current.m_offsetIndex )
							{
								end = &current;
							}
							else
							{
								draw();
								start = &current;
								end = start;
							}
						}

						draw();
					}
				}

				//for ( Int32 islandIndex = 0; islandIndex < voxelIslands.size(); ++islandIndex )
				//{
				//	systems::DebugSystem::DebugDrawParams params;
				//	params.m_lifetime = 999999.0f; //15.0f;
				//	Math::Random rng{ static_cast< Uint32 >( islandIndex ) };
				//	params.m_color = LinearColor{ rng.GetFloat(), rng.GetFloat(), rng.GetFloat() };

				//	VoxelIsland& island = voxelIslands[ islandIndex ];

				//	for ( const Voxel& voxel : island.m_border )
				//	{
				//		GetVoxel( voxel.m_x, voxel.m_y, voxel.m_z ) = VoxelType::Border;

				//		Vector3 offset( ( static_cast< Float >( voxel.m_x ) + 0.5f )* xSize, ( static_cast< Float >( voxel.m_y ) + 0.5f )* ySize, ( static_cast< Float >( voxel.m_z ) + 0.5f )* zSize );
				//		offset = transform.GetOrientation() * offset;

				//		const Vector3 voxelPos = minCorner + offset;
				//		const Transform voxelTransform( voxelPos, transform.GetOrientation() );
				//		debugSystem.DrawCube( voxelTransform, voxelExtents, params );
				//	}
				//}
			}
		} );

	// I think system could add an object with custom serialization to scene
}
#endif

#pragma optimize("", on)