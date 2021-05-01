// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidShrine.generated.h"

UCLASS()
class LUCIDO_API ALucidShrine : public ALucidToolBase
{
	GENERATED_BODY()
public:
	ALucidShrine(const FObjectInitializer& ObjectInitializer);

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bSHowHUDMessage = false) override;

protected:

	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;
	bool m_StartedInteraction = false;
};
