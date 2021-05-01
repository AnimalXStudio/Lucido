// Copyright AnimalXStudio 2021


#include "Interaction/LucidInteractionComponent.h"

ULucidInteractionComponent::ULucidInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	GetInteractionStartedDelegate().AddUObject(this, &ThisClass::Internal_OnInteractionStarted);
	GetOnBecomeActiveTargetDelegate().AddUObject(this, &ThisClass::Internal_OnBecomeActiveTarget);
	GetOnExitActiveTargetDelegate().AddUObject(this, &ThisClass::Internal_OnExittActiveTarget);
}

void ULucidInteractionComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void ULucidInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetIsTargeted())
	{
		//Notify the targeting actor we are no longer a valid target
		GetOnExitActiveTargetDelegate().Broadcast(GetTargetingCharacter());
	}

	GetInteractionStartedDelegate().RemoveAll(this);
	GetOnBecomeActiveTargetDelegate().RemoveAll(this);
	GetOnExitActiveTargetDelegate().RemoveAll(this);
}

void ULucidInteractionComponent::Internal_OnBecomeActiveTarget(ALucidCharacter* InteractingActor)
{
	bIsBeingTargeted = true;
	m_TargetingActor = InteractingActor;
}

void ULucidInteractionComponent::Internal_OnExittActiveTarget(ALucidCharacter* InteractingActor)
{
	bIsBeingTargeted = false;
	m_TargetingActor = InteractingActor;
}

void ULucidInteractionComponent::Internal_OnInteractionStarted(ALucidCharacter* InteractingActor)
{
	
}
