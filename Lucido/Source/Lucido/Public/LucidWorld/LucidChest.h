// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidChest.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidChest : public ALucidToolBase
{
	GENERATED_BODY()

public:
	
	ALucidChest(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */) override;
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;
};
