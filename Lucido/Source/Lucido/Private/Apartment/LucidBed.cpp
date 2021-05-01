// Copyright AnimalXStudio 2021


#include "Apartment/LucidBed.h"
#include "Character/LucidCharacter.h"
#include "../../LucidoGameModeBase.h"
#include "Components/BoxComponent.h"
#include "UI/LucidHUD.h"

ALucidBed::ALucidBed(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(ALucidToolBase::RootComponentClassName))
{
	m_ShouldSimulatePhysics = false;
	m_ShouldDestroyIfNotInitialLoad = false;
	m_WantsSave = false;

	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetMobility(EComponentMobility::Static);
		m_CollisionComponent->SetCollisionObjectType(ECC_WorldStatic);
	}
}

bool ALucidBed::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage)
{
	if (!TargetingActor || m_StartedInteraction)
	{
		return false;
	}

	bool bCanInteract = TargetingActor->GetCurrentPickUp() == nullptr;

	if (!bCanInteract && bShowHUDMessage)
	{
		if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
		{
			LucidHUD->ShowHUDMessage("You're not trying to sleep while carrying a " + GetToolDisplayName() + ", are you?", 2.f, 3);
		}
	}

	return bCanInteract;
}

void ALucidBed::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	//Double check if interaction is possible before continuing
	if (CanInteract(TargetingCharacter))
	{
		TargetingCharacter->BlockCharacterMovement();
		TargetingCharacter->BlockCharacterLook();

		if (ALucidoGameModeBase* LucidGM = ALucidoGameModeBase::GetLucidGameMode(this))
		{
			LucidGM->StartLucidDreaming();
		}

		m_StartedInteraction = true;
	}
}
