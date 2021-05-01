// Copyright AnimalXStudio 2021

#include "Core/LucidGameInstance.h"
#include "Core/LucidSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include <Core/LucidVisibilityComponent.h>
#include "../LucidoGameModeBase.h"

bool ULucidGameInstance::bIsInitialLoad = true;

#if WITH_EDITOR
FGameInstancePIEResult ULucidGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	FGameInstancePIEResult SuperResult = Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
	bIsInitialLoad = true;
	return SuperResult;
}
#endif

ULucidGameInstance* ULucidGameInstance::GetInstance(const UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		return Cast<ULucidGameInstance>(World->GetGameInstance());
	}
	return nullptr;
}

void ULucidGameInstance::ResetStateForNewGame()
{
	m_SaveGameData = nullptr;
	m_LucidityLevel = 0;
	m_DaysPassed = 0;
	bIsInitialLoad = true;
	m_LucidChestContents.Empty();
	m_ComputerOrders = nullptr;
	m_Money = 0.0f;
	m_PendingOrders.Empty();
}

void ULucidGameInstance::AddLucidityLevel(float DeltaLucidity)
{
	m_LucidityLevel += DeltaLucidity;
	if (m_LucidityLevel < 0)
	{
		m_LucidityLevel = 0;
	}
	else if (m_LucidityLevel > 100.0f)
	{
		m_LucidityLevel = 100.0f;
	}
}

void ULucidGameInstance::SetLuciditiLevel(float NewLucidity)
{
	if (NewLucidity > 0)
	{
		m_LucidityLevel = NewLucidity;
	}
}

void ULucidGameInstance::BeginDestroy()
{
	Super::BeginDestroy();

#if WITH_EDITOR
	bIsInitialLoad = true;
#endif
}

ULucidSaveGame* ULucidGameInstance::GetCurrentSaveGameData()
{
	//Create save game data if not yet present
	if (!m_SaveGameData)
	{
		m_SaveGameData = Cast<ULucidSaveGame>(UGameplayStatics::CreateSaveGameObject(ULucidSaveGame::StaticClass()));
	}
	return m_SaveGameData;
}

bool ULucidGameInstance::SaveActor(const AActor* ActorToSave)
{
	FLevelData& LevelData = GetCurrentSaveGameData()->m_LevelDataMap.FindOrAdd(m_CurrentLevelName);

	FActorData ActorData{};
	ActorData.ActorClass = ActorToSave->GetClass();
	ActorData.ActorTransform = ActorToSave->GetActorTransform();

	if (const ALucidToolBase* AsTool = Cast<ALucidToolBase>(ActorToSave))
	{
		ActorData.ObjectFinished = AsTool->m_IsInteractionFinished;
	}
	else
	{
		ActorData.ObjectFinished = false;
	}

	if (ULucidVisibilityComponent* VisibilityComponent = ActorToSave->FindComponentByClass<ULucidVisibilityComponent>())
	{
		ActorData.RequireLucidityLevel = VisibilityComponent->GetRequiredLucidityLevel();
		ActorData.MaxLucidityLevel = VisibilityComponent->GetMaxLucidityLevel();
	}

	LevelData.LevelActors.Add(ActorData);
	
	return true;
}

void ULucidGameInstance::ClearLevelActorData(FName LevelToClear)
{
	FLevelData& LevelData = GetCurrentSaveGameData()->m_LevelDataMap.FindOrAdd(LevelToClear);
	LevelData.LevelActors.Empty(LevelData.LevelActors.Num());
}


void ULucidGameInstance::LoadLevelActors(FName LevelToLoad)
{
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FLevelData& LevelData = GetCurrentSaveGameData()->m_LevelDataMap.FindOrAdd(LevelToLoad);
		for (const FActorData& ActorData : LevelData.LevelActors)
		{
			AActor* SpawnedActor = World->SpawnActorDeferred<AActor>(ActorData.ActorClass, ActorData.ActorTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (SpawnedActor)
			{
				if (ULucidVisibilityComponent* VisibilityComponent = SpawnedActor->FindComponentByClass<ULucidVisibilityComponent>())
				{
					VisibilityComponent->m_RequiredLucidityLevel = ActorData.RequireLucidityLevel;
					VisibilityComponent->m_MaxLucidityLevel = ActorData.MaxLucidityLevel;

					if (ALucidToolBase* AsTool = Cast<ALucidToolBase>(SpawnedActor))
					{
						AsTool->m_IsInteractionFinished = ActorData.ObjectFinished;
					}
				}
				UGameplayStatics::FinishSpawningActor(SpawnedActor, ActorData.ActorTransform);
			}
		}
	}
}

bool ULucidGameInstance::HasItem(ALucidToolBase* ObjectToAdd)
{
	if (ObjectToAdd)
	{
		UClass* ObjectClass = ObjectToAdd->GetClass();
		if (m_LucidChestContents.Find(ObjectClass))
		{
			return true;
		}
	}
	return false;
}

void ULucidGameInstance::AddChestContents(ALucidToolBase* ObjectToAdd)
{
	if (ObjectToAdd)
	{
		if (ULucidVisibilityComponent* LucidComponent = ObjectToAdd->GetLucidVisibilityComponent())
		{
			FLuciditySettings& LuciditySettings = m_LucidChestContents.FindOrAdd(ObjectToAdd->GetClass());
			LuciditySettings.MinLucidity = LucidComponent->GetRequiredLucidityLevel();
			LuciditySettings.MaxLucidity = LucidComponent->GetMaxLucidityLevel();
		}
	}
}

bool ULucidGameInstance::GetItemFromChest(TSubclassOf<ALucidToolBase> ObjectClass)
{
	if (ALucidoGameModeBase* LucidGM = ALucidoGameModeBase::GetLucidGameMode(this))
	{
		if (ALucidCharacter* Character = LucidGM->GetCachedLocalPlayer())
		{
			if (Character->GetCurrentPickUp() == nullptr)
			{
				if (FLuciditySettings* LucidSettings = m_LucidChestContents.Find(ObjectClass))
				{
					UWorld* World = GetWorld();
					if (!World)
					{
						return false;
					}

					FTransform SpawnTransform = Character->GetPickUpAttachTransform();
					ALucidToolBase* SpawnedTool = World->SpawnActorDeferred<ALucidToolBase>(ObjectClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
					if(SpawnedTool)
					{
						//If you take out an item from the chest, their lucidity requirement will be set to your current lucidity level
						if (ULucidVisibilityComponent* VisibilityComponent = SpawnedTool->FindComponentByClass<ULucidVisibilityComponent>())
						{
							VisibilityComponent->m_RequiredLucidityLevel = LucidSettings->MinLucidity;
							VisibilityComponent->m_MaxLucidityLevel = LucidSettings->MaxLucidity;
							
							//Skip checking for lucidity this dream
							VisibilityComponent->m_ShouldCheckVisibility = false;
						}
						UGameplayStatics::FinishSpawningActor(SpawnedTool, SpawnTransform);
						SpawnedTool->m_GrabbedFromChest = true;

						SpawnedTool->OnToolPickedUp();
						if (Character->PickupObject(SpawnedTool))
						{
							m_LucidChestContents.Remove(ObjectClass);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void ULucidGameInstance::GetChestContents(TArray<ALucidToolBase*>& OutChestContents)
{
	OutChestContents.Empty();

	for (auto& MapPair : m_LucidChestContents)
	{
		//We return the default objects as we can use these to show them in the list entry menu
		OutChestContents.Add(MapPair.Key->GetDefaultObject<ALucidToolBase>());
	}
}

void ULucidGameInstance::GetComputerOrders(TArray<ALucidOrder*>& OutPossibleOrders)
{
	if (m_ComputerOrders)
	{
		OutPossibleOrders.Empty(m_ComputerOrders->Num());
		OutPossibleOrders = *m_ComputerOrders;
	}
}

void ULucidGameInstance::SetComputerOrders(TArray<ALucidOrder*>& Orders)
{
	m_ComputerOrders = &Orders;
}

bool ULucidGameInstance::CanOrderItem(ALucidOrder* ItemToOrder)
{
	if (ItemToOrder)
	{
		float OrderPrice = ItemToOrder->OrderDetails.ObjectPrice;
		int32 IsOrderPending = m_PendingOrders.Find(ItemToOrder->OrderDetails.ObjectClass);
		return OrderPrice <= m_Money && IsOrderPending == INDEX_NONE;
	}
	return false;
}

bool ULucidGameInstance::TryOrderItem(ALucidOrder* ItemToOrder)
{
	if (CanOrderItem(ItemToOrder))
	{
		m_PendingOrders.Add(ItemToOrder->OrderDetails.ObjectClass);
		AddMoney(-ItemToOrder->OrderDetails.ObjectPrice);
		return true;
	}
	return false;
}
