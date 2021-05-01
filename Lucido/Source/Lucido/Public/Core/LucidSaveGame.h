// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LucidSaveGame.generated.h"

USTRUCT()
struct FActorData
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* ActorClass;
	UPROPERTY()
	FTransform ActorTransform;
	UPROPERTY()
	float RequireLucidityLevel;
	UPROPERTY()
	float MaxLucidityLevel;
	UPROPERTY()
	bool ObjectFinished;
};

USTRUCT()
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FActorData> LevelActors;
};

/**
 * 
 */
UCLASS()
class LUCIDO_API ULucidSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	//Player related data
	UPROPERTY()
	float m_LucidityLevel = 0;
	UPROPERTY()
	int32 m_DaysPassed = 0;

	//Current Level
	UPROPERTY()
	FName CurrentMapName = NAME_None;

	//Data for statically placed level actors
	UPROPERTY()
	TMap<FName, FLevelData> m_LevelDataMap;
};
