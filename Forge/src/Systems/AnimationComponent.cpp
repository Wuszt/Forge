#include "Fpch.h"
#include "AnimationComponent.h"
#include "../Renderer/SkeletonAsset.h"
#include "RenderingComponent.h"
#include "../Renderer/IRenderer.h"

IMPLEMENT_TYPE( forge::AnimationFragment );
IMPLEMENT_TYPE( forge::AnimationComponent );

forge::AnimationFragment::AnimationFragment()
	: m_animation( 0.0f, 0u )
{}

void forge::AnimationComponent::OnAttach( EngineInstance& engineInstance )
{
	DataComponent< AnimationFragment >::OnAttach( engineInstance );
	GetMutableData().m_cb.SetImpl( engineInstance.GetRenderer().CreateConstantBufferImpl() );
	GetOwner().GetComponent< forge::RenderingComponent >()->GetDirtyRenderable().AddConstantBuffer( { renderer::VSConstantBufferType::SkeletalMesh, renderer::PSConstantBufferType::Invalid, GetMutableData().m_cb.GetImpl() } );
}

void forge::AnimationComponent::OnDetach( EngineInstance& engineInstance )
{
	DataComponent< AnimationFragment >::OnDetach( engineInstance );
	GetOwner().GetComponent< forge::RenderingComponent >()->GetDirtyRenderable().RemoveConstantBuffer( { renderer::VSConstantBufferType::SkeletalMesh, renderer::PSConstantBufferType::Invalid, GetMutableData().m_cb.GetImpl() } );
}

void forge::AnimationComponent::SetAnimation( renderer::Animation animation )
{
	GetMutableData().m_animation = std::move( animation );
}

void forge::AnimationComponent::SetSkeleton( const renderer::SkeletonAsset& skeleton )
{
	FORGE_ASSERT( skeleton.GetBonesOffsets().GetSize() <= c_maxBonesAmount );
	std::copy( skeleton.GetBonesOffsets().begin(), skeleton.GetBonesOffsets().end(), std::begin( GetMutableData().m_bonesOffsets ) );
}
