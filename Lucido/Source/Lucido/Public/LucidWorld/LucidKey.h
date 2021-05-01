// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidKey.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidKey : public ALucidToolBase
{
	GENERATED_BODY()
	
public:
	ALucidKey(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */) override;

	//The name of the key, this is used to check which door the key opens
	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "Key Name")
	FName m_KeyName = NAME_None;
};
