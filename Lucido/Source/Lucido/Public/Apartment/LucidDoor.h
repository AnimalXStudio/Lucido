// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "LucidDoor.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ALucidDoor : public ALucidToolBase
{
	GENERATED_BODY()

public:

	ALucidDoor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */) override;
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;

	UFUNCTION()
	void PostDeliveryMessage();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDoorOpened(ALucidCharacter* TargetingActor);

protected:

	UPROPERTY(EditAnywhere, DisplayName = "SpawnPoint", Category = "Spawn")
	USceneComponent* m_SpawnPoint = nullptr;

	UPROPERTY(EditAnywhere, DisplayName = "Settings", Category = "Door Name")
	FName m_DoorName = NAME_None;
	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "CanSpawnDeliveries")
	bool m_CanSpawnDeliveries = false;
	UPROPERTY(EditAnywhere, Category = "Settings", DisplayName = "Show Examin Message Time")
	float m_ShowInteractionTime = 10.0f;

	int32 m_NumSpawnedObjects = 0;
	bool m_StrangeDeliveryReceived = false;
};
