// Copyright AnimalXStudio 2021

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LucidVisibilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisibilityChanged, bool, NewVisibility);

/*
* Component that checks lucidity level requirements for actor visibility.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUCIDO_API ULucidVisibilityComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ULucidGameInstance;

public:	
	ULucidVisibilityComponent();

	float GetRequiredLucidityLevel() const {return m_RequiredLucidityLevel;}
	float GetMaxLucidityLevel() const {return m_MaxLucidityLevel;}

	UPROPERTY(BlueprintAssignable)
	FOnVisibilityChanged OnVisibilityChangedDelegate;
	UPROPERTY(BlueprintReadOnly)
	bool bIsVisible = false;

	bool m_ShouldCheckVisibility = true;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

////////////////////////////////////////////////////////////////////////////////
// Visibility settings
////////////////////////////////////////////////////////////////////////////////
	
	//The minimum require lucidity level required for this actor to be shown in the lucid world
	UPROPERTY(EditAnywhere, Category = "Lucidity", DisplayName = "RequiredLuciditylevel")
	float m_RequiredLucidityLevel = 0.f;
	//If you are more lucid that this level, this actor will no longer show up!
	UPROPERTY(EditAnywhere, Category = "Lucidity", DisplayName = "MaxLucidityLevel")
	float m_MaxLucidityLevel = 100.f;
};
