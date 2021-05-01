// Copyright AnimalXStudio 2021

#include "Tools/LucidWeapon.h"
#include "Character/LucidCharacter.h"
#include "Components/BoxComponent.h"

ALucidWeapon::ALucidWeapon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(ALucidToolBase::RootComponentClassName))
{
	m_CanBePickedUp = true;
}

bool ALucidWeapon::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage)
{
	return false;
}

void ALucidWeapon::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);
}
