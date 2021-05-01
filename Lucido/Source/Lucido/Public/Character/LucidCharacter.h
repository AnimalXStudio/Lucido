// Copyright AnimalXStudio 2021
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LucidCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULucidInteractionComponent;
class ALucidToolBase;

UCLASS()
class LUCIDO_API ALucidCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALucidCharacter(const FObjectInitializer& ObjectInitializer);

	FTransform GetPickUpAttachTransform() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Input functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	void LookUp(float AxisValue);
	void LookRight(float AxisValue);

	void Jump();

	//Character components
	UPROPERTY(EditDefaultsOnly, Category = "Core", DisplayName = "CameraBoom")
	USpringArmComponent* m_CameraBoom;
	UPROPERTY(EditDefaultsOnly, Category = "Core", DisplayName = "FirstPersonCamera")
	UCameraComponent* m_CameraComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Core|Sockets", DisplayName = "ToolsAttachmentComponent")
	USceneComponent* m_ToolsAttachment = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Interaction functionality
////////////////////////////////////////////////////////////////////////////////
protected:

	void Interact();

	//Interaction settings
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Settings", DisplayName = "InteractionRange")
	float m_InteractionRange = 500.0f;

	//Tracks which actor is currently targeted by the character
	UPROPERTY()
	ALucidToolBase* m_CurrentInteractionTarget = nullptr;
	UPROPERTY()
	ULucidInteractionComponent* m_CurrentTargetInteractionComponent = nullptr;
private:

	void UpdateInteraction();

	void UpdateHUDState();

	void ResetInteractionState();

////////////////////////////////////////////////////////////////////////////////
// Pickup functionality
////////////////////////////////////////////////////////////////////////////////

public:
	
	ALucidToolBase* GetCurrentPickUp() const { return m_CurrentCarriedObject.Get(); }
	bool CanPickupObject() const { return m_CurrentCarriedObject.Get() == nullptr; }
	bool PickupObject(ALucidToolBase* WeaponToPickUp);

	//Object input
	void Attack();
	void DropObject();
	void PickupTargetObject();

private:

	TWeakObjectPtr<ALucidToolBase> m_CurrentCarriedObject;

////////////////////////////////////////////////////////////////////////////////
// Energy system
////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintCallable)
	bool HasEnoughEnergy(float EnergyCost) const { return m_CurrentEnergy >= EnergyCost; };
	UFUNCTION(BlueprintCallable)
	float GetRemainingEnergy() const { return m_CurrentEnergy; };

	float ReduceEnergy(float DeltaEnergy);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Energy", DisplayName = "CurrentEnergy")
	float m_CurrentEnergy = 100.0f;

////////////////////////////////////////////////////////////////////////////////
// Movement Control
////////////////////////////////////////////////////////////////////////////////

public:
	void BlockCharacterMovement();
	void BlockCharacterLook();

	void UnBlockCharacterMovement();
	void UnBlockCharacterLook();

private:

	bool m_IsMovementBlocked = false;
	bool m_IsLookBlocked = false;
};
