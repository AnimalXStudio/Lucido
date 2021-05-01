// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Apartment/LucidComputer.h"
#include "LucidHUD.generated.h"

class ALucidCharacter;

UENUM(BlueprintType)
enum class EPickupState : uint8 
{
	PickUp_No,
	PickUp_Invalid,
	PickUp_Yes
};

UENUM(BlueprintType)
enum class EInteractState : uint8
{
	Interact_No,
	Interact_Invalid,
	Interact_Yes
};

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	static ALucidHUD* GetInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetPlayerEnergyPercentLeft() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetLucidTimePercentLeft() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsDreaming() const { return m_IsDreaming; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EInteractState GetInteractionState() const { return m_InteractionState; };
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPickupState GetPickupState() const {return m_PickUpState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetLucidStrengthPercent() const;

	//HUD Events
	void OnDreamingStateChanged(bool bIsDreaming) { m_IsDreaming = bIsDreaming; };
	void OnInteractionStateChanged(EInteractState NewInteractionState) { m_InteractionState = NewInteractionState; }
	void OnPickUpStateChanged(EPickupState NewPickupState) { m_PickUpState = NewPickupState; }

	//Shows a text message to the player
	//Text is styled based on message priority, higher priority takes well, priority...
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowHUDMessage(const FString& MessageString, float MessageDuration, int32 MessagePriority);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowChestContents();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseChestIfShowing();
	UFUNCTION(BlueprintCallable)
	void OnChestUIClosed();

	UFUNCTION(BlueprintImplementableEvent)
	void SetHUDVisibility(bool bNewVisibility, float Delay);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowComputerUI();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseComputerUI();
	UFUNCTION(BlueprintCallable)
	void OnComputerUIClosed();

private:
	
	ALucidCharacter* GetCachedLocalPlayer() const;
	mutable TWeakObjectPtr<ALucidCharacter> m_CachedCharacter = nullptr;

	//State variables
	bool m_IsDreaming;
	EInteractState m_InteractionState;
	EPickupState m_PickUpState;
};
