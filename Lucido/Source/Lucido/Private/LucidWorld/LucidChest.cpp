// Copyright AnimalXStudio 2021


#include "LucidWorld/LucidChest.h"
#include "Components/BoxComponent.h"
#include "Character/LucidCharacter.h"
#include "Core/LucidGameInstance.h"
#include "UI/LucidHUD.h"
#include "../LucidoGameModeBase.h"

ALucidChest::ALucidChest(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ALucidToolBase::RootComponentClassName))
{
	m_CanBePickedUp = false;
	m_ShouldSimulatePhysics = false;
	m_WantsSave = false;
	m_ShouldDestroyAfterInteraction = false;
	m_ShouldDestroyIfNotInitialLoad = false;
}

bool ALucidChest::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */)
{
	//No need to call super, we don't care about the cool down or energy
	if (TargetingActor)
	{
		//Can't open the interface when we are falling asleep/waking up
		if (ALucidoGameModeBase* GM = ALucidoGameModeBase::GetLucidGameMode(this))
		{
			bool bIsTraveling = GM->GetIsTraveling();
			if (bIsTraveling)
			{
				return false;
			}
		}

		//If we're not carrying anything, we can open the interface
		if (ALucidToolBase* CarriedTool = TargetingActor->GetCurrentPickUp())
		{
			if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
			{
				//Only allow interacting if the chest doesn't already contain this item
				return !LucidGI->HasItem(CarriedTool);
			}
		}
		else
		{
			//If we're not carrying anything, we can open the interface
			return true;
		}
	}

	return false;
}

void ALucidChest::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	if (TargetingCharacter)
	{
		if (ALucidToolBase* CarriedTool = TargetingCharacter->GetCurrentPickUp())
		{
			if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
			{
				TargetingCharacter->DropObject();

				LucidGI->AddChestContents(CarriedTool);
				CarriedTool->Destroy();
			}
		}
		else if(ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
		{
			LucidHUD->ShowChestContents();

			//These are unblocked again when the UI closes (assuming I remember to call the function from blueprint)
			TargetingCharacter->BlockCharacterMovement();
			TargetingCharacter->BlockCharacterLook();
		}
	}
}
