// Copyright AnimalXStudio 2021
#include "Core/LucidChangingBase.h"
#include "Character/LucidCharacter.h"
#include "Core/LucidGameInstance.h"
#include "Components/BoxComponent.h"

ALucidChangingBase::ALucidChangingBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ALucidToolBase::RootComponentClassName))
{
	m_ShouldSimulatePhysics = false;
}

bool ALucidChangingBase::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage)
{
	bool bSuperCanInteract = Super::CanInteract(TargetingActor, bShowHUDMessage);
	return bSuperCanInteract && TargetingActor ? TargetingActor->GetCurrentPickUp() == nullptr : false;
}

void ALucidChangingBase::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
	{
		float LucidityChange = FMath::RandRange(m_MinLucidityChange, m_MaxLucidityChange);
		LucidGI->AddLucidityLevel(LucidityChange);
	}

	Super::OnInteractionStarted(TargetingCharacter);
}
