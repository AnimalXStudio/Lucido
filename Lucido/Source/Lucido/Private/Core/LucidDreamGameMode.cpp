// Copyright AnimalXStudio 2021

#include "Core/LucidDreamGameMode.h"
#include "Core/LucidGameInstance.h"
#include "UI/LucidHUD.h"

ALucidDreamGameMode::ALucidDreamGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 1 / 30.f;
}

ALucidDreamGameMode* ALucidDreamGameMode::GetInstance(const UObject* WorldContextObject)
{
	return Cast<ALucidDreamGameMode>(ALucidoGameModeBase::GetLucidGameMode(WorldContextObject));
}

void ALucidDreamGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (m_LucidTimeCurve)
	{
		if (ULucidGameInstance* LucidInstance = ULucidGameInstance::GetInstance(this))
		{
			m_MaxLucidTime = m_LucidTimeCurve->GetFloatValue(LucidInstance->GetLucidityLevel());
			m_LucidTimeLeft = m_MaxLucidTime;
		}
	}

	//Load the level if this is not the initial load
	if (!ULucidGameInstance::bIsInitialLoad)
	{
		ULucidGameInstance::GetInstance(this)->LoadLevelActors(m_LucidDreamingLevelName);
	}

	if (ALucidHUD* HUD = ALucidHUD::GetInstance(this))
	{
		HUD->OnDreamingStateChanged(true);
	}
}

void ALucidDreamGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!m_Traveling)
	{
		m_LucidTimeLeft -= DeltaSeconds;

		//Automatically return to the apartment world once the timer runs out
		if (m_LucidTimeLeft <= 0.0f && !m_ReturningToApartment)
		{
			ReturnToApartment(m_MaxLucidTime, true);
			m_ReturningToApartment = true;
		}
	}
}
