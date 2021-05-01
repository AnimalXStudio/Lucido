// Copyright AnimalXStudio 2021

#include "Apartment/LucidDoor.h"
#include "Components/BoxComponent.h"
#include "Core/LucidGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/LucidHUD.h"
#include "LucidWorld/LucidKey.h"
#include "Character/LucidCharacter.h"

ALucidDoor::ALucidDoor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UStaticMeshComponent>(ALucidToolBase::RootComponentClassName))
{
	//Set up interactable settings
	m_CanBePickedUp = false;
	m_ShouldSimulatePhysics = false;
	m_WantsSave = false;
	m_ShouldDestroyAfterInteraction = false;
	m_ShouldDestroyIfNotInitialLoad = false;
	m_EnergyCost = 0.0f;
	m_UsageCooldownTime = 30.0f;
	m_AllowInstantActiation = true;

	m_SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectSpawnPoint"));
	m_SpawnPoint->SetupAttachment(GetRootComponent());
}

void ALucidDoor::BeginPlay()
{
	Super::BeginPlay();

	if (m_CanSpawnDeliveries)
	{
		//Check pending orders and spawn the ordered items at the door!
		if (UWorld* World = GetWorld())
		{
			if (ULucidGameInstance* GameInstance = ULucidGameInstance::GetInstance(this))
			{
				TArray<TSubclassOf<ALucidToolBase>>& PendingOrders = GameInstance->GetCurrentOrders();

				//Check if the player has found the door key in the dream world and put it in the chest before waking up
				TArray<ALucidToolBase*> LucidChestContents;
				GameInstance->GetChestContents(LucidChestContents);
				for (ALucidToolBase* ChestTool : LucidChestContents)
				{
					if (ALucidKey* AsKey = Cast<ALucidKey>(ChestTool))
					{
						if (AsKey->m_KeyName == FName{ "FrontDoor" })
						{
							//Only spawn the key if it was added to the chest
							PendingOrders.Empty();
							//Add this as an order
							PendingOrders.Add(AsKey->GetClass());

							//Let the player know they received a strange package
							m_StrangeDeliveryReceived = true;
						}
					}
				}

				FTransform SpawnTransform{ m_SpawnPoint->GetComponentTransform() };
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				m_NumSpawnedObjects = 0;
				for (TSubclassOf<ALucidToolBase> OrderClass : PendingOrders)
				{
					ALucidToolBase* SpawnedTool = World->SpawnActor<ALucidToolBase>(OrderClass, SpawnTransform, SpawnParams);
					if (SpawnedTool)
					{
						SpawnedTool->SetSimulatesPhysics(true);
						++m_NumSpawnedObjects;
					}
				}

				if (m_NumSpawnedObjects > 0)
				{
					FTimerHandle MessageTimer;
					FTimerManager& TimerManager = World->GetTimerManager();
					TimerManager.SetTimer(MessageTimer, this, &ThisClass::PostDeliveryMessage, 5.0f, false, false);
				}

				PendingOrders.Empty();
			}
		}
	}
}

bool ALucidDoor::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */)
{
	return Super::CanInteract(TargetingActor, bShowHUDMessage);
}

void ALucidDoor::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);
	
	//If we are carrying the right key, open the door.
	//Otherwise tell the player this isn't the right door
	if (ALucidKey* LucidKey = Cast<ALucidKey>(TargetingCharacter->GetCurrentPickUp()))
	{
		if (LucidKey->m_KeyName == m_DoorName)
		{
			TargetingCharacter->DropObject();
			LucidKey->Destroy();

			OnDoorOpened(TargetingCharacter);
		}
		else
		{
			if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
			{
				LucidHUD->ShowHUDMessage("This key doesn't seem to fit :(", m_ShowInteractionTime, 6);
			}
		}
	}
	else
	{
		SendOnInteractionFailedMessage(m_ShowInteractionTime);
	}
}

void ALucidDoor::PostDeliveryMessage()
{
	if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
	{
		if (m_StrangeDeliveryReceived)
		{
			LucidHUD->ShowHUDMessage("As you wake up you notice you received a strange package...", 10.0f, 99);
			m_StrangeDeliveryReceived = false;
		}
		else
		{
			FString Package = m_NumSpawnedObjects > 1 ? "packages" : "package";
			FString HUDMessage = "You have " + FString::FromInt(m_NumSpawnedObjects) + " new " + Package + " delivered at your door!";

			LucidHUD->ShowHUDMessage(HUDMessage, 3.0f, 2);
		}

	}
}
