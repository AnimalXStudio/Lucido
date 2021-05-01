#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Character/LucidCharacter.h"
#include "LucidoGameModeBase.generated.h"

UCLASS()
class LUCIDO_API ALucidoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ALucidoGameModeBase(const FObjectInitializer& ObjectInitializer);
	
	static ALucidoGameModeBase* GetLucidGameMode(const UObject* WorldContextObject);

	void StartLucidDreaming();
	void ReturnToApartment(float TimeSpentInDream, bool bWasFullTimer);
	bool GetIsTraveling() const  { return m_Traveling; };

	ALucidCharacter* GetCachedLocalPlayer() const;
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Worlds", DisplayName = "LucidDreamingLevel")
	FName m_LucidDreamingLevelName;

	UPROPERTY(EditDefaultsOnly, Category = "Worlds", DisplayName = "ApartmentLevel")
	FName m_ApartmentLevelName;

	bool m_Traveling = false;
private:
	
	UFUNCTION()
	void Delayed_StartLucidDreaming();

	UFUNCTION()
	void Delayed_ReturnToApartment();
	
	float m_MoneyToGrant = 0.0f;

	mutable TWeakObjectPtr<ALucidCharacter> m_CachedPlayerPawn;
};
