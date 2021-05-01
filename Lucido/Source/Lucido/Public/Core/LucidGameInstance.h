// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LucidSaveGame.h"
#include "LucidWorld/LucidChest.h"
#include "Apartment/LucidComputer.h"
#include "LucidGameInstance.generated.h"

USTRUCT()
struct FLuciditySettings
{
	GENERATED_BODY()

	UPROPERTY()
	float MinLucidity = 0.0f;
	UPROPERTY()
	float MaxLucidity = 0.0f;
};

UCLASS()
class LUCIDO_API ULucidGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif
	
	UFUNCTION(BlueprintCallable)
	static ULucidGameInstance* GetInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	void ResetStateForNewGame();

	float GetLucidityLevel() const { return m_LucidityLevel; };

	void AddLucidityLevel(float DeltaLucidity);
	void SetLuciditiLevel(float NewLucidity);

	int32 GetDaysPassed() const { return m_DaysPassed; }
	void IncrementDays() { ++m_DaysPassed; }

	virtual void BeginDestroy() override;
private:

	//Tracks the player's current lucidity level. This changes which objects are visible/interactable in the lucid world
	UPROPERTY()
	float m_LucidityLevel = 0;
	//How many days have passed in the real world since the start of the game. This will be used as a score
	UPROPERTY()
	int32 m_DaysPassed = 0;

////////////////////////////////////////////////////////////////////////////////
// Save Game section
////////////////////////////////////////////////////////////////////////////////

public:
	
	ULucidSaveGame* GetCurrentSaveGameData();
	bool SaveActor(const AActor* ActorToSave);
	void ClearLevelActorData(FName LevelToClear);
	void LoadLevelActors(FName LevelToLoad);
	void SetCurrentLevelName(FName CurrentLevelName) {m_CurrentLevelName = CurrentLevelName;};

	static bool bIsInitialLoad;

protected:

	UPROPERTY()
	ULucidSaveGame* m_SaveGameData;
	UPROPERTY()
	FName m_CurrentLevelName;

////////////////////////////////////////////////////////////////////////////////
// Lucid chest and computer helpers
////////////////////////////////////////////////////////////////////////////////

public:

	bool HasItem(ALucidToolBase* ObjectToAdd);
	void AddChestContents(ALucidToolBase* ObjectToAdd);

	//Will be called from the Chest UI
	//Checks the chest has this object and then awards it to the player
	UFUNCTION(BlueprintCallable)
	bool GetItemFromChest(TSubclassOf<ALucidToolBase> ObjectClass);
	
	UFUNCTION(BlueprintCallable)
	void GetChestContents(TArray<ALucidToolBase*>& OutChestContents);
	UFUNCTION(BlueprintCallable)
	void GetComputerOrders(TArray<ALucidOrder*>& OutPossibleOrders);

	void SetComputerOrders(TArray<ALucidOrder*>& Orders);
private:

	UPROPERTY(Transient)
	TMap<TSubclassOf<ALucidToolBase>, FLuciditySettings> m_LucidChestContents;
	
	TArray<ALucidOrder*>* m_ComputerOrders;

////////////////////////////////////////////////////////////////////////////////
// Money System
////////////////////////////////////////////////////////////////////////////////

public:

	UFUNCTION(BlueprintCallable)
	void AddMoney(float DeltaMoney) { m_Money += DeltaMoney; };
	UFUNCTION(BlueprintCallable)
	float GetCurrentMoney() const { return m_Money; }

	UFUNCTION(BlueprintCallable)
	bool CanOrderItem(ALucidOrder* ItemToOrder);
	UFUNCTION(BlueprintCallable)
	bool TryOrderItem(ALucidOrder* ItemToOrder);

	TArray<TSubclassOf<ALucidToolBase>>& GetCurrentOrders() { return m_PendingOrders; }
protected:

	UPROPERTY()
	TArray<TSubclassOf<ALucidToolBase>> m_PendingOrders;
	float m_Money = 0.0f;
};