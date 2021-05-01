// Copyright AnimalXStudio 2021

#include "LucidWorld/LucidShrine.h"
#include "../../LucidoGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "Core/LucidDreamGameMode.h"

//The shrine will be closer to a cylinder than a box, so override the root collision component to be a capsule
//This can change to just have the mesh as the base and use UCX collision perhaps?
ALucidShrine::ALucidShrine(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ALucidToolBase::RootComponentClassName))
{
	m_ShouldSimulatePhysics = false;
	m_ShouldDestroyIfNotInitialLoad = false;
	m_WantsSave = false;
}

bool ALucidShrine::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage)
{
	return !m_StartedInteraction;
}

void ALucidShrine::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	//Double check if interaction is possible before continuing
	if (CanInteract(TargetingCharacter))
	{
		TargetingCharacter->BlockCharacterMovement();
		if (ALucidDreamGameMode* LucidGM = ALucidDreamGameMode::GetInstance(this))
		{
			LucidGM->ReturnToApartment(LucidGM->GetMaxLucidTime() - LucidGM->GetLucidTimeLeft(), false);
		}

		m_StartedInteraction = true;
	}
}