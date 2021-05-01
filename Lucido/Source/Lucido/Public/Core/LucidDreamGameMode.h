// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "../../LucidoGameModeBase.h"
#include "LucidDreamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidDreamGameMode : public ALucidoGameModeBase
{
	GENERATED_BODY()

public:

	ALucidDreamGameMode(const FObjectInitializer& ObjectInitializer);

	static ALucidDreamGameMode* GetInstance(const UObject* WorldContextObject);

	float GetLucidTimeLeft() const { return m_LucidTimeLeft; }
	float GetMaxLucidTime() const { return m_MaxLucidTime; }

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//Curve used to map lucidity level to the time you can spend in the lucid dream state before waking up automatically
	UPROPERTY(EditDefaultsOnly, Category = "Energy", DisplayName = "EnergyReductionPerSecond")
	UCurveFloat* m_LucidTimeCurve = nullptr;

	//Default this to 100 second, this is overridden in beginplay using the provided curve
	float m_LucidTimeLeft = 100.0f;
	float m_MaxLucidTime = 100.0f;
	bool m_ReturningToApartment = false;
};
