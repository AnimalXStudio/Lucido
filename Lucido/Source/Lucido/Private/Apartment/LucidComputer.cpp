// Copyright AnimalXStudio 2021


#include "Apartment/LucidComputer.h"
#include "Components/BoxComponent.h"
#include "Character/LucidCharacter.h"
#include "UI/LucidHUD.h"
#include "Core/LucidGameInstance.h"
#include "../LucidoGameModeBase.h"

ALucidComputer::ALucidComputer(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBoxComponent>(ALucidToolBase::RootComponentClassName))
{
	//Set up interactable settings
	m_CanBePickedUp = false;
	m_ShouldSimulatePhysics = false;
	m_WantsSave = false;
	m_ShouldDestroyAfterInteraction = false;
	m_ShouldDestroyIfNotInitialLoad = false;
}

void ALucidComputer::BeginPlay()
{
	Super::BeginPlay();

	if (m_OrdersDataTable)
	{
		if (UWorld* World = GetWorld())
		{
			m_OrderObjects.Empty();

			TArray<FLucidOrderType*> AllOrders;
			m_OrdersDataTable->GetAllRows<FLucidOrderType>(TEXT("Get All Orders"), AllOrders);
			for (const FLucidOrderType* CurrentOrder : AllOrders)
			{
				if (CurrentOrder)
				{
					ALucidOrder* SpawnedActor = World->SpawnActor<ALucidOrder>();
					if (SpawnedActor)
					{
						SpawnedActor->OrderDetails = *CurrentOrder;
					}
					m_OrderObjects.Add(SpawnedActor);
				}
			}
		}
	}
}

bool ALucidComputer::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */)
{
	//No need to call super, we don't care about the cool down or energy
	if (TargetingActor)
	{
		if (ALucidToolBase* CarriedTool = TargetingActor->GetCurrentPickUp())
		{
			return false;
		}
		else
		{
			//If we're not carrying anything, we can open the interface
			if (ALucidoGameModeBase* GM = ALucidoGameModeBase::GetLucidGameMode(this))
			{
				return !GM->GetIsTraveling();
			}
		}
	}

	return false;
}

void ALucidComputer::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	Super::OnInteractionStarted(TargetingCharacter);

	if (TargetingCharacter)
	{
		if (ULucidGameInstance* GameInstance = ULucidGameInstance::GetInstance(this))
		{
			GameInstance->SetComputerOrders(m_OrderObjects);
		}

		if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
		{
			LucidHUD->ShowComputerUI();

			//These are unblocked again when the UI closes (assuming I remember to call the function from blueprint)
			TargetingCharacter->BlockCharacterMovement();
			TargetingCharacter->BlockCharacterLook();
		}
	}
}

ALucidOrder::ALucidOrder()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}
