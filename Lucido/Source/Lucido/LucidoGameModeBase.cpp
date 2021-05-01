#include "LucidoGameModeBase.h"
#include "Character/LucidCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Core/LucidGameInstance.h"
#include "UI/LucidHUD.h"


ALucidoGameModeBase::ALucidoGameModeBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	DefaultPawnClass = ALucidCharacter::StaticClass();
	HUDClass = ALucidHUD::StaticClass();
}

ALucidoGameModeBase* ALucidoGameModeBase::GetLucidGameMode(const UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		return Cast<ALucidoGameModeBase>(World->GetAuthGameMode());
	}
	return nullptr;
}

void ALucidoGameModeBase::StartLucidDreaming()
{
	if (UWorld* World = GetWorld())
	{
		m_Traveling = true;
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
		{
			CameraManager->StartCameraFade(0.0f, 1.0f, 2.0f, FLinearColor::Black, true, true);
		}

		if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
		{
			LucidHUD->CloseComputerUI();

			LucidHUD->ShowHUDMessage("You start to feel light headed and start lucid dreaming", 3.f, 10);
			LucidHUD->SetHUDVisibility(false, 3.0f);
		}

		FTimerHandle Dummy;
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(Dummy, this, &ThisClass::Delayed_StartLucidDreaming, 5.0f, false);
	}
}

void ALucidoGameModeBase::ReturnToApartment(float TimeSpentInDream, bool bWasFullTimer)
{
	m_MoneyToGrant = TimeSpentInDream * (bWasFullTimer? 1.5f : 1.0f);
	if (UWorld* World = GetWorld())
	{
		m_Traveling = true;
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
		{
			CameraManager->StartCameraFade(0.0f, 1.0f, 2.0f, FLinearColor::Black, true, true);
		}

		if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
		{
			LucidHUD->CloseChestIfShowing();

			LucidHUD->ShowHUDMessage("You feel your grasp on your dream fade and wake up.", 3.f, 10);
			LucidHUD->SetHUDVisibility(false, 3.0f);
		}

		FTimerHandle Dummy;
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(Dummy, this, &ThisClass::Delayed_ReturnToApartment, 5.0f, false);
	}
}

ALucidCharacter* ALucidoGameModeBase::GetCachedLocalPlayer() const
{
	if (ALucidCharacter* LocalCharacter = m_CachedPlayerPawn.Get())
	{
		return LocalCharacter;
	}

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ALucidCharacter* LucidCharacter = Cast<ALucidCharacter>(PC->GetPawn()))
			{
				m_CachedPlayerPawn = LucidCharacter;
				return LucidCharacter;
			}
		}
	}

	return nullptr;
}


void ALucidoGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
	{
		CameraManager->StartCameraFade(1.0f, 0.0f, 5.0f, FLinearColor::Black, true, true);
	}

	if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
	{
		LucidHUD->SetHUDVisibility(true, 3.0f);
	}

	if (ALucidCharacter* PlayerPawn = GetCachedLocalPlayer())
	{
		PlayerPawn->UnBlockCharacterMovement();
		PlayerPawn->UnBlockCharacterLook();
	}

	m_Traveling = false;
}

void ALucidoGameModeBase::Delayed_StartLucidDreaming()
{
	//Before traveling, we want to clear the current level actor details so the state can be updated
	if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
	{
		LucidGI->ClearLevelActorData(m_ApartmentLevelName);
		LucidGI->SetCurrentLevelName(m_ApartmentLevelName);
	}

	UGameplayStatics::OpenLevel(this, m_LucidDreamingLevelName);
}

void ALucidoGameModeBase::Delayed_ReturnToApartment()
{
	if (ALucidCharacter* PlayerPawn = GetCachedLocalPlayer())
	{
		if (ALucidToolBase* CarriedTool = PlayerPawn->GetCurrentPickUp())
		{
			if (CarriedTool->m_GrabbedFromChest)
			{
				if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
				{
					if (!LucidGI->HasItem(CarriedTool))
					{
						LucidGI->AddChestContents(CarriedTool);
					}
					else
					{
						PlayerPawn->DropObject();
					}
				}
				else
				{
					PlayerPawn->DropObject();
				}
			}
			else
			{
				PlayerPawn->DropObject();
			}
		}
	}
	
	//Before traveling, we want to clear the current level actor details so the state can be updated
	if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
	{
		LucidGI->ClearLevelActorData(m_LucidDreamingLevelName);
		LucidGI->SetCurrentLevelName(m_LucidDreamingLevelName);

		LucidGI->AddMoney(m_MoneyToGrant);
	}

	ULucidGameInstance::bIsInitialLoad = false;
	UGameplayStatics::OpenLevel(this, m_ApartmentLevelName);
}
