// Copyright AnimalXStudio 2021

#include "Core/LucidVisibilityComponent.h"
#include "Core/LucidGameInstance.h"
#include "Tools/LucidToolBase.h"

ULucidVisibilityComponent::ULucidVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULucidVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!m_ShouldCheckVisibility)
	{
		return;
	}

	if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
	{
		float LucidLevel = LucidGI->GetLucidityLevel();
		bIsVisible = LucidLevel >= m_RequiredLucidityLevel && LucidLevel <= m_MaxLucidityLevel;

		OnVisibilityChangedDelegate.Broadcast(bIsVisible);


		if (AActor* Owner = GetOwner())
		{
			Owner->SetActorHiddenInGame(!bIsVisible);
			Owner->SetActorEnableCollision(bIsVisible);

			if (bIsVisible)
			{
				if (ALucidToolBase* AsTool = Cast<ALucidToolBase>(Owner))
				{
					AsTool->ConditionalSetSimulatesPhysics(true);
				}
			}
		}
	}
}
