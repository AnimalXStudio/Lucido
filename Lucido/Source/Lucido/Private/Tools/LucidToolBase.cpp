// Copyright AnimalXStudio 2021

#include "Tools/LucidToolBase.h"
#include "Interaction/LucidInteractionComponent.h"
#include "Character/LucidCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Core/LucidVisibilityComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <Core/LucidGameInstance.h>
#include "Components/BoxComponent.h"
#include "UI/LucidHUD.h"

FName ALucidToolBase::RootComponentClassName(TEXT("TollCollision"));

ALucidToolBase::ALucidToolBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Default the collision component to a box, can be overridden in sub classes if required
	m_CollisionComponent = CreateDefaultSubobject<UPrimitiveComponent>(ALucidToolBase::RootComponentClassName);
	if (!m_CollisionComponent)
	{
		m_CollisionComponent = CreateDefaultSubobject<UBoxComponent>(ALucidToolBase::RootComponentClassName);
	}

	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		m_CollisionComponent->SetCollisionProfileName(FName{ "Tool" });
		SetRootComponent(m_CollisionComponent);
	}

	//#todo: Could make this optional so subclasses can choose to have no mesh (or the mesh is the root component etc)
	m_ToolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ToolMeshs"));
	if (m_ToolMesh)
	{
		m_ToolMesh->SetupAttachment(m_CollisionComponent);
		m_ToolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	m_InteractionComponent = CreateDefaultSubobject<ULucidInteractionComponent>(TEXT("InteractionComponent"));
	if (m_InteractionComponent)
	{
		m_InteractionComponent->GetOnBecomeActiveTargetDelegate().AddUObject(this, &ThisClass::OnBecomeActiveTarget);
		m_InteractionComponent->GetOnExitActiveTargetDelegate().AddUObject(this, &ThisClass::OnExitActiveTarget);
		m_InteractionComponent->GetInteractionStartedDelegate().AddUObject(this, &ThisClass::OnInteractionStarted);
	}

	m_LucidVisibilityComponent = CreateDefaultSubobject<ULucidVisibilityComponent>(TEXT("LucidVisibilityComponent"));
}

void ALucidToolBase::DropTool(bool bPlaceOnFloor)
{
	SetActorEnableCollision(true);
	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetSimulatePhysics(m_ShouldSimulatePhysics);
	}

	
	if (!m_ShouldSimulatePhysics && bPlaceOnFloor)
	{
		if (UWorld* World = GetWorld())
		{
			FVector ActorForwardVector = GetActorForwardVector();
			FVector RayStart = GetActorLocation() + ActorForwardVector * 30.0f;
			FVector RayEnd = RayStart - FVector::UpVector * 250.0f;
			
			FCollisionObjectQueryParams QueryParams{FCollisionObjectQueryParams::InitType::AllStaticObjects};
			FHitResult HitResult;
			if (World->LineTraceSingleByObjectType(HitResult, RayStart, RayEnd, QueryParams))
			{
				SetActorLocationAndRotation(HitResult.Location + HitResult.Normal * m_DropFloorOffset, UKismetMathLibrary::MakeRotFromZX(HitResult.Normal, ActorForwardVector));
			}
		}
	}
}

void ALucidToolBase::OnToolPickedUp()
{
	SetActorEnableCollision(false);
	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetSimulatePhysics(false);
	}
}

bool ALucidToolBase::CanInteract(ALucidCharacter* TargetingActor, bool bShowHUDMessage)
{
	if (!TargetingActor || m_IsInteractionFinished)
	{
		return false;
	}

	const float CurrentTimeStamp = GetWorldTimeSeconds();
	const bool bTimeConstraintMet = CurrentTimeStamp > (m_LastUseTimeStamp + m_UsageCooldownTime) || (m_AllowInstantActiation && m_LastUseTimeStamp <= 0.0f);
	const bool bEnergyConstraintMet = TargetingActor->HasEnoughEnergy(m_EnergyCost);
	return bTimeConstraintMet && bEnergyConstraintMet;
}

void ALucidToolBase::SetSimulatesPhysics(bool bSimulates)
{
	if (m_CollisionComponent)
	{
		m_CollisionComponent->SetSimulatePhysics(bSimulates);
	}
}

void ALucidToolBase::ConditionalSetSimulatesPhysics(bool bSimulates)
{
	if (m_ShouldSimulatePhysics)
	{
		SetSimulatesPhysics(bSimulates);
	}
}

void ALucidToolBase::SendOnInteractionFailedMessage(float Duration)
{
	if (ALucidHUD* LucidHUD = ALucidHUD::GetInstance(this))
	{
		LucidHUD->ShowHUDMessage(m_OnInteractionFailed, Duration, 5);
	}
}

void ALucidToolBase::BeginPlay()
{
	Super::BeginPlay();
}

void ALucidToolBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//If we are traveling to another world, make sure we save our actor
	if (EndPlayReason == EEndPlayReason::LevelTransition && m_WantsSave)
	{
		if (ULucidGameInstance* LucidGI = ULucidGameInstance::GetInstance(this))
		{
			LucidGI->SaveActor(this);
		}
	}

	Super::EndPlay(EndPlayReason);

	if (m_InteractionComponent)
	{
		//Clean up interaction callbacks on destruction
		m_InteractionComponent->GetOnBecomeActiveTargetDelegate().RemoveAll(this);
		m_InteractionComponent->GetOnExitActiveTargetDelegate().RemoveAll(this);
		m_InteractionComponent->GetInteractionStartedDelegate().RemoveAll(this);
	}
}

void ALucidToolBase::PostLoad()
{
	Super::PostLoad();

	//Destroy level placed actors that should not be loaded if not initial load
	if (m_ShouldDestroyIfNotInitialLoad &&!ULucidGameInstance::bIsInitialLoad)
	{
		Destroy();
	}
}

void ALucidToolBase::OnBecomeActiveTarget(ALucidCharacter* TargetingCharacter)
{
	ReceiveBecomeActiveTarget(TargetingCharacter);
}

void ALucidToolBase::OnExitActiveTarget(ALucidCharacter* TargetingCharacter)
{
	ReceiveExitActiveTarget(TargetingCharacter);
}

void ALucidToolBase::OnInteractionStarted(ALucidCharacter* TargetingCharacter)
{
	if (!TargetingCharacter)
	{
		return;
	}

	if (CanInteract(TargetingCharacter))
	{
		ReceiveInteractionStarted(TargetingCharacter);

		//Take the energy cost after interacting
		TargetingCharacter->ReduceEnergy(m_EnergyCost);

		//Auto destroy if required
		if (m_ShouldDestroyAfterInteraction)
		{
			Destroy();
		}
		else
		{
			m_LastUseTimeStamp = GetWorldTimeSeconds();
		}
	}
}

float ALucidToolBase::GetWorldTimeSeconds() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimeSeconds();
	}
	return 0.0f;
}
