// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LucidToolBase.generated.h"

class ULucidInteractionComponent;
class ALucidCharacter;
class ULucidVisibilityComponent;

UCLASS()
class LUCIDO_API ALucidToolBase : public AActor
{
	GENERATED_BODY()
	friend class ULucidGameInstance;
	friend class ULucidVisibilityComponent;

public:	
	
	ALucidToolBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	ULucidInteractionComponent* GetInteracationComponent() const { return m_InteractionComponent; }
	ULucidVisibilityComponent* GetLucidVisibilityComponent() const { return m_LucidVisibilityComponent; }
	//Use this name to override the class of the root primitive component in child classes
	static FName RootComponentClassName;

	//@ param bPlaceOnFloor: if set to true, places the object on the floor if it shouldn't simulate physics
	virtual void DropTool(bool bPlaceOnFloor = true);

	bool GetCanBePickedUp() const { return m_CanBePickedUp; }
	void OnToolPickedUp();

	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FString& GetToolDisplayName() const { return m_Displayname; };

	void SetSimulatesPhysics(bool bSimulates);
	void ConditionalSetSimulatesPhysics(bool bSimulates);

	bool m_GrabbedFromChest = false;
protected:
	
	UFUNCTION(BlueprintCallable)
	void SendOnInteractionFailedMessage(float Duration = 3.0f);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostLoad() override;

	//Interaction callbacks
	virtual void OnBecomeActiveTarget(ALucidCharacter* TargetingCharacter);
	virtual void OnExitActiveTarget(ALucidCharacter* TargetingCharacter);
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter);

	//Blueprint interface for tools, used for quick iteration
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveBecomeActiveTarget(ALucidCharacter* TargetingCharacter);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveExitActiveTarget(ALucidCharacter* TargetingCharacter);
	UFUNCTION(BlueprintImplementableEvent)
	void ReceiveInteractionStarted(ALucidCharacter* TargetingCharacter);

////////////////////////////////////////////////////////////////////////////////
// Default components
////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category = "Collision", DisplayName = "CollisionComponent")
	UPrimitiveComponent* m_CollisionComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Visuals", DisplayName = "ToolMesh", BlueprintReadOnly)
	UStaticMeshComponent* m_ToolMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", DisplayName = "InteractionComponent")
	ULucidInteractionComponent* m_InteractionComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Lucidity", DisplayName = "LucidVisibilityComponent", BlueprintReadOnly)
	ULucidVisibilityComponent* m_LucidVisibilityComponent = nullptr;

////////////////////////////////////////////////////////////////////////////////
// Pickup settings
////////////////////////////////////////////////////////////////////////////////

	//If this object can be picked up
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "CanBePickedUp")
	bool m_CanBePickedUp = false;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "ShouldSimulatePhysics")
	bool m_ShouldSimulatePhysics = true;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DestroyAfterInteraction")
	bool m_ShouldDestroyAfterInteraction = false;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "UsageCooldownTimeSeconds")
	float m_UsageCooldownTime = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "AllowInstantActiation")
	bool m_AllowInstantActiation = false;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DropFloorOffset")
	float m_DropFloorOffset = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "EnergyCost")
	float m_EnergyCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Saving", DisplayName = "WantsSave")
	bool m_WantsSave = true;
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Saving", DisplayName = "DestroyIfNotInitialLoad")
	bool m_ShouldDestroyIfNotInitialLoad = true;

	UPROPERTY(EditDefaultsOnly, Category = "Settings", DisplayName = "DisplayName")
	FString m_Displayname = "Lucid tool";
	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "On Interaction Failed Message")
	FString m_OnInteractionFailed = "Drop what you're doing before continuing!";

	UPROPERTY(EditDefaultsOnly, Category = "Runtime", DisplayName = "ObjectFinished", BlueprintReadWrite)
	bool m_IsInteractionFinished = false;

////////////////////////////////////////////////////////////////////////////////
// Runtime tracking variables
////////////////////////////////////////////////////////////////////////////////

private:
	
	float GetWorldTimeSeconds() const;
	float m_LastUseTimeStamp = 0.0f;
	
};