// Copyright AnimalXStudio 2021


#include "Core/LucidApartmentGameMode.h"
#include <Core/LucidGameInstance.h>
#include "UI/LucidHUD.h"

ALucidApartmentGameMode::ALucidApartmentGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 1/30.f;
}

void ALucidApartmentGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!m_Traveling)
	{
		//This should probably accumulate energy to reduce and reduce in larger chunks?
		if (ALucidCharacter* CachedCharacter = GetCachedLocalPlayer())
		{
			CachedCharacter->ReduceEnergy(DeltaSeconds * m_EnergyReductionSecond);
		}
	}
}

void ALucidApartmentGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Load the level if this is not the initial load
	if (!ULucidGameInstance::bIsInitialLoad)
	{
		ULucidGameInstance::GetInstance(this)->LoadLevelActors(m_ApartmentLevelName);
	}

	if (ALucidHUD* HUD = ALucidHUD::GetInstance(this))
	{
		HUD->OnDreamingStateChanged(false);
	}
}
