// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/LucidCharacter.h"
#include "LucidInteractionComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInteractionDelegate, ALucidCharacter*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUCIDO_API ULucidInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULucidInteractionComponent();

	float GetInteractionRange() const { return m_InteractionRange; };

	FInteractionDelegate& GetInteractionStartedDelegate() { return m_InteractionStartedDelegate; };
	FInteractionDelegate& GetOnBecomeActiveTargetDelegate() { return m_OnBecomeActiveTarget; };
	FInteractionDelegate& GetOnExitActiveTargetDelegate() { return m_OnExitActiveTarget; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

////////////////////////////////////////////////////////////////////////////////
// Interactions settings
////////////////////////////////////////////////////////////////////////////////

	//From how far away can the character interact with the owner
	UPROPERTY(EditDefaultsOnly)
	float m_InteractionRange = 250.0f;

private:
		
	void Internal_OnBecomeActiveTarget(ALucidCharacter* InteractingActor);
	void Internal_OnExittActiveTarget(ALucidCharacter* InteractingActor);
	void Internal_OnInteractionStarted(ALucidCharacter* InteractingActor);

	FInteractionDelegate m_InteractionStartedDelegate;
	FInteractionDelegate m_OnBecomeActiveTarget;
	FInteractionDelegate m_OnExitActiveTarget;

////////////////////////////////////////////////////////////////////////////////
// Interaction State
////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintCallable)
	bool GetIsTargeted() const { return bIsBeingTargeted; }

	UFUNCTION(BlueprintCallable)
	ALucidCharacter* GetTargetingCharacter() const { return m_TargetingActor.Get(); }

protected:
	bool bIsBeingTargeted = false;
	TWeakObjectPtr<ALucidCharacter> m_TargetingActor = nullptr;
};
