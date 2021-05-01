// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidWeapon.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidWeapon : public ALucidToolBase
{
	GENERATED_BODY()
	
public:

	ALucidWeapon(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage = false) override;
protected:
	
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;

};
