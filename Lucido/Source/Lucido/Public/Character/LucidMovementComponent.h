// Copyright AnimalXStudio 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LucidMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class LUCIDO_API ULucidMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	ULucidMovementComponent(const FObjectInitializer& ObjectInitializer);
};
