// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "../../LucidoGameModeBase.h"
#include "LucidApartmentGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidApartmentGameMode : public ALucidoGameModeBase
{
	GENERATED_BODY()
public:
	
	ALucidApartmentGameMode(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void Tick( float DeltaSeconds ) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Energy", DisplayName = "EnergyReductionPerSecond")
	float m_EnergyReductionSecond = .1f;
};
