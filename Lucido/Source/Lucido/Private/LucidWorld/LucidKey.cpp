// Copyright AnimalXStudio 2021


#include "LucidWorld/LucidKey.h"
#include "Tools/LucidToolBase.h"

ALucidKey::ALucidKey(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ALucidToolBase::RootComponentClassName))
{
	//Set up interactable settings
	m_CanBePickedUp = true;
	m_ShouldSimulatePhysics = true;
	m_WantsSave = true;
	m_ShouldDestroyAfterInteraction = false;
	m_ShouldDestroyIfNotInitialLoad = true;

	//Don't care about energy
	m_EnergyCost = 0.0f;
}

bool ALucidKey::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */)
{
	//Can't interact with the keys, only pick them up
	return false;
}
