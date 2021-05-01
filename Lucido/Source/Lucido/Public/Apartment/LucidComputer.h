// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "Tools/LucidToolBase.h"
#include "Engine/DataTable.h"
#include "LucidComputer.generated.h"

USTRUCT(BlueprintType)
struct FLucidOrderType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ALucidToolBase> ObjectClass;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ObjectPrice;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString OrderName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Description;
};

UCLASS()
class LUCIDO_API ALucidOrder : public AActor
{
	GENERATED_BODY()

public:
	
	ALucidOrder();

	UPROPERTY(BlueprintReadOnly)
	FLucidOrderType OrderDetails;
};

UCLASS()
class LUCIDO_API ALucidComputer : public ALucidToolBase
{
	GENERATED_BODY()
	
public:
	
	ALucidComputer(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual bool CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage /* = false */) override;
	virtual void OnInteractionStarted(ALucidCharacter* TargetingCharacter) override;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	UDataTable* m_OrdersDataTable;
	UPROPERTY()
	TArray<ALucidOrder*> m_OrderObjects;
};
