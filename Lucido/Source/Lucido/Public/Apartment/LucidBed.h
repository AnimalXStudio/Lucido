// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidBed.generated.h"

UCLASS()
class LUCIDO_API ALucidBed : public ALucidToolBase
{
	GENERATED_BODY()
	
public:
	ALucidBed(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage = false) override;

protected:

	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;

	bool m_StartedInteraction = false;
};
