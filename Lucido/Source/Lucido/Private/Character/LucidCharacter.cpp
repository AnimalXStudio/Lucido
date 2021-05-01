#include "Character/LucidCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/LucidMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/LucidInteractionComponent.h"
#include "Tools/LucidWeapon.h"
#include "UI/LucidHUD.h"
#include "Kismet/GameplayStatics.h"
#include "../LucidoGameModeBase.h"

ALucidCharacter::ALucidCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULucidMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* CharacterCapsuleComponent = GetCapsuleComponent();

	//Create the camera boom component and attach it to the capsule
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(CharacterCapsuleComponent);
	m_CameraBoom->SetRelativeLocation(FVector{ -40.0f, 0.0f , 60.0f });
	m_CameraBoom->TargetArmLength = 0.0f;
	m_CameraBoom->bUsePawnControlRotation = true;

	//Create the camera component and attach it to the capsule
	m_CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	m_CameraComponent->SetupAttachment(m_CameraBoom);

	//Create the scene component used to attach tools to you find in the world
	m_ToolsAttachment = CreateDefaultSubobject<USceneComponent>(TEXT("ToolAttachment"));
	m_ToolsAttachment->SetupAttachment(CharacterCapsuleComponent);
	m_ToolsAttachment->SetRelativeLocation(FVector{ 30.0f, 30.0f , 30.0f });

	AutoReceiveInput = EAutoReceiveInput::Player0;
}

FTransform ALucidCharacter::GetPickUpAttachTransform() const
{
	if (m_ToolsAttachment)
	{
		m_ToolsAttachment->GetComponentTransform();
	}

	//Return identity if the attachment component is not valid
	return FTransform{};
}

void ALucidCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALucidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update the interaction settings based on current state
	UpdateInteraction();
}

void ALucidCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
		//Bind axis callbacks

		//Movement
		PlayerInputComponent->BindAxis(
			FName{ "MoveForward" }, this, &ThisClass::MoveForward
		);
		PlayerInputComponent->BindAxis(
			FName{ "MoveRight" }, this, &ThisClass::MoveRight
		);

		//Look
		PlayerInputComponent->BindAxis(
			FName{ "LookUp" }, this, &ThisClass::LookUp
		);
		PlayerInputComponent->BindAxis(
			FName{ "LookRight" }, this, &ThisClass::LookRight
		);

		//Bind action events

		//Movement
		PlayerInputComponent->BindAction(
			FName{ "Jump" }, EInputEvent::IE_Pressed, this, &ThisClass::Jump
		);

		//Interaction
		PlayerInputComponent->BindAction(
			FName{ "Interact" }, EInputEvent::IE_Pressed, this, &ThisClass::Interact
		);

		//Objects
		PlayerInputComponent->BindAction(
			FName{ "Attack" }, EInputEvent::IE_Pressed, this, &ThisClass::Attack
		);
		PlayerInputComponent->BindAction(
			FName{ "DropObject" }, EInputEvent::IE_Pressed, this, &ThisClass::DropObject
		);
		PlayerInputComponent->BindAction(
			FName{ "PickUp" }, EInputEvent::IE_Pressed, this, &ThisClass::PickupTargetObject
		);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Input functions
////////////////////////////////////////////////////////////////////////////////

void ALucidCharacter::MoveForward(float AxisValue)
{
	if (!m_IsMovementBlocked)
	{
		AddMovementInput(GetActorForwardVector(), AxisValue);
	}
}

void ALucidCharacter::MoveRight(float AxisValue)
{
	if (!m_IsMovementBlocked)
	{
		AddMovementInput(GetActorRightVector(), AxisValue);
	}
}

void ALucidCharacter::LookUp(float AxisValue)
{
	if (!m_IsLookBlocked)
	{
		AddControllerPitchInput(AxisValue);
	}
}

void ALucidCharacter::LookRight(float AxisValue)
{
	if (!m_IsLookBlocked)
	{
		AddControllerYawInput(AxisValue);
	}
}

void ALucidCharacter::Jump()
{
	if (!m_IsMovementBlocked)
	{
		bPressedJump = true;
	}
}

void ALucidCharacter::Interact()
{
	//Don't interact with anything while traveling as this can cause issues with saving
	if (ALucidoGameModeBase* GM = ALucidoGameModeBase::GetLucidGameMode(this))
	{
		if (GM->GetIsTraveling())
		{
			return;
		}
	}
	
	if (m_CurrentInteractionTarget && m_CurrentTargetInteractionComponent)
	{
		if (m_CurrentInteractionTarget->CanInteract(this, true))
		{
			m_CurrentTargetInteractionComponent->GetInteractionStartedDelegate().Broadcast(this);
		}
	}
}

void ALucidCharacter::UpdateInteraction()
{
	if (UWorld* World = GetWorld())
	{
		FVector TraceStart = m_CameraComponent->GetComponentLocation();
		FVector TraceEnd = TraceStart + m_CameraComponent->GetForwardVector() * m_InteractionRange;

		FHitResult HitResult;
		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GameTraceChannel1))
		{
			if (ALucidToolBase* HitActor = Cast<ALucidToolBase>(HitResult.Actor.Get()))
			{
				if (m_CurrentInteractionTarget)
				{
					//If we hit a different actor compared to last frame, reset the state
					if (m_CurrentInteractionTarget != HitActor)
					{
						ResetInteractionState();
					}
					else
					{
						//Update the HUD in case the interaction state on the object changed
						UpdateHUDState();
						//We hit the same actor as last frame, all required delegates have been fired already
						return;
					}
				}

				if (ULucidInteractionComponent* InteractionComponent = HitActor->FindComponentByClass<ULucidInteractionComponent>())
				{
					if (InteractionComponent->GetInteractionRange() >= HitResult.Distance)
					{
						m_CurrentInteractionTarget = HitActor;
						m_CurrentTargetInteractionComponent = InteractionComponent;
						m_CurrentTargetInteractionComponent->GetOnBecomeActiveTargetDelegate().Broadcast(this);
					}
				}
			}
			else
			{
				ResetInteractionState();
			}
		}
		else
		{
			ResetInteractionState();
		}

		UpdateHUDState();
	}
}

void ALucidCharacter::UpdateHUDState()
{
	if (ALucidHUD* HUD = ALucidHUD::GetInstance(this))
	{
		if (m_CurrentInteractionTarget)
		{
			if (m_CurrentInteractionTarget->CanInteract(this))
			{
				HUD->OnInteractionStateChanged(EInteractState::Interact_Yes);
			}
			else
			{
				HUD->OnInteractionStateChanged(EInteractState::Interact_Invalid);
			}

			if (m_CurrentInteractionTarget->GetCanBePickedUp())
			{
				if (CanPickupObject())
				{
					HUD->OnPickUpStateChanged(EPickupState::PickUp_Yes);
				}
				else
				{
					HUD->OnPickUpStateChanged(EPickupState::PickUp_Invalid);
				}
			}
			else
			{
				HUD->OnPickUpStateChanged(EPickupState::PickUp_No);
			}
		}
		else
		{
			//No target actors so nothing to show here
			HUD->OnPickUpStateChanged(EPickupState::PickUp_No);
			HUD->OnInteractionStateChanged(EInteractState::Interact_No);
		}
	}
}

void ALucidCharacter::ResetInteractionState()
{
	if (m_CurrentTargetInteractionComponent)
	{
		//Reset the current target interaction data
		m_CurrentTargetInteractionComponent->GetOnExitActiveTargetDelegate().Broadcast(this);
		m_CurrentTargetInteractionComponent = nullptr;
		m_CurrentInteractionTarget = nullptr;
	}
}

bool ALucidCharacter::PickupObject(ALucidToolBase* WeaponToPickUp)
{
	//Validate data
	if (! WeaponToPickUp || GetCurrentPickUp() != nullptr)
	{
		return false;
	}

	//Save pointer to our current weapon
	m_CurrentCarriedObject = WeaponToPickUp;

	//Attach the weapon to our socket
	m_CurrentCarriedObject->AttachToComponent(m_ToolsAttachment, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	return true;
}

void ALucidCharacter::Attack()
{
	//This should be a timeline driven by curves to "animate" the weapon we are holding in thin air
	if (ALucidWeapon* CurrentWeapon = Cast<ALucidWeapon>(GetCurrentPickUp()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, TEXT("KAPOW!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("You're not wielding a weapon, silly!"));
	}
}

void ALucidCharacter::DropObject()
{
	// Temp just destroy the weapon and clear the settings
	if (ALucidToolBase* CurrentPickup = GetCurrentPickUp())
	{
		m_CurrentCarriedObject->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		m_CurrentCarriedObject->DropTool(true);

		m_CurrentCarriedObject = nullptr;
	}
}

void ALucidCharacter::PickupTargetObject()
{
	if (ALucidToolBase* TargetTool = m_CurrentInteractionTarget)
	{
		//If this object can be picked up, pick it up
		if (TargetTool->GetCanBePickedUp() && CanPickupObject())
		{
			TargetTool->OnToolPickedUp();
			PickupObject(TargetTool);
		}
	}
}

float ALucidCharacter::ReduceEnergy(float DeltaEnergy)
{
	m_CurrentEnergy -= DeltaEnergy;
	if (m_CurrentEnergy < 0.0f)
	{
		m_CurrentEnergy = 0.0f;
	}

	return m_CurrentEnergy;
}


//At this point I had been going for 15 hours with 3 hours sleep, so yeah :D
void ALucidCharacter::BlockCharacterMovement()
{
	m_IsMovementBlocked = true;
}

void ALucidCharacter::BlockCharacterLook()
{
	m_IsLookBlocked = true;
}

void ALucidCharacter::UnBlockCharacterMovement()
{
	m_IsMovementBlocked = false;
}

void ALucidCharacter::UnBlockCharacterLook()
{
	m_IsLookBlocked = false;
}
