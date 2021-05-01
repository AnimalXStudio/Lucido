// Copyright AnimalXStudio 2021


#include "UI/LucidHUD.h"
#include "../LucidoGameModeBase.h"
#include "Core/LucidDreamGameMode.h"
#include <Core/LucidGameInstance.h>

ALucidHUD* ALucidHUD::GetInstance(const UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr)
	{
		if (APlayerController* PC = World->GetFirstPlayerController<APlayerController>())
		{
			return Cast<ALucidHUD>(PC->GetHUD());
		}
	}
	return nullptr;
}

float ALucidHUD::GetPlayerEnergyPercentLeft() const
{
	if (ALucidCharacter* CachedCharacter = GetCachedLocalPlayer())
	{
		//Hard coded as I won't change it from 100 for the jam.
		return CachedCharacter->GetRemainingEnergy() / 100.0f;
	}
	return 0.0f;
}

float ALucidHUD::GetLucidTimePercentLeft() const
{
	if (ALucidDreamGameMode* LucidDreamGM = ALucidDreamGameMode::GetInstance(this))
	{
		return LucidDreamGM->GetLucidTimeLeft() / LucidDreamGM->GetMaxLucidTime();
	}

	return 0.0f;
}

float ALucidHUD::GetLucidStrengthPercent() const
{
	if (ULucidGameInstance* LucidInstance = ULucidGameInstance::GetInstance(this))
	{
		return LucidInstance->GetLucidityLevel() / 100.f;
	}
	return 0.0f;
}

void ALucidHUD::OnChestUIClosed()
{
	if (ALucidCharacter* CachedCharacter = GetCachedLocalPlayer())
	{
		CachedCharacter->UnBlockCharacterLook();
		CachedCharacter->UnBlockCharacterMovement();
	}
}

void ALucidHUD::OnComputerUIClosed()
{
	if (ALucidCharacter* CachedCharacter = GetCachedLocalPlayer())
	{
		CachedCharacter->UnBlockCharacterLook();
		CachedCharacter->UnBlockCharacterMovement();
	}
}

ALucidCharacter* ALucidHUD::GetCachedLocalPlayer() const
{
	if (ALucidCharacter* CachedCharacter = m_CachedCharacter.Get())
	{
		return CachedCharacter;
	}

	if (ALucidoGameModeBase* GM = ALucidoGameModeBase::GetLucidGameMode(this))
	{
		m_CachedCharacter = GM->GetCachedLocalPlayer();
		return m_CachedCharacter.Get();
	}
	return nullptr;
}
