// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidChangingBase.generated.h"

UCLASS()
class LUCIDO_API ALucidChangingBase : public ALucidToolBase
{
	GENERATED_BODY()

public:
	
	ALucidChangingBase(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage) override;

protected:
	
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;

	UPROPERTY(EditDefaultsOnly, Category = "Lucidity", DisplayName = "MinLucidityChange")
	float m_MinLucidityChange = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Lucidity", DisplayName = "MaxLucidityChange")
	float m_MaxLucidityChange = 0.0f;
};
