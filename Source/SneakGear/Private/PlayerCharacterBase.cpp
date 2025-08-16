#include "PlayerCharacterBase.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ItemData.h"
#include "EquipableToolBase.h"
#include "EquipableToolDefinition.h"
#include "InventoryComponent.h"
#include "PlayerControllerBase.h"


APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void APlayerCharacterBase::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (bIsSprinting && CurrentStamina <= 0.f)
	{
		SetSprinting(false);
	}

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);

	if (bIsSprinting)
	{
		bHasSprinted = true;
	}
}

void APlayerCharacterBase::Crouch(bool bClientSimulation)
{
	SetSprinting(false);
	Super::Crouch(bClientSimulation);
}

bool APlayerCharacterBase::IsToolAlreadyOwned(UEquipableToolDefinition* ToolDefinition)
{
	for (auto IventoryItem : InventoryComponent->ToolInventory)
	{
		if (IventoryItem->ID == ToolDefinition->ID)
		{
			return true;
		}
	}
	return false;
}

void APlayerCharacterBase::AttachTool(UEquipableToolDefinition* ToolDefinition)
{
	if (!IsToolAlreadyOwned(ToolDefinition))
	{
		auto ToolToEquip = GetWorld()->SpawnActor<AEquipableToolBase>(ToolDefinition->ToolAsset,
		                                                              this->GetActorTransform());

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules);

		ToolToEquip->OwningCharacter = this;

		InventoryComponent->ToolInventory.Add(ToolDefinition);

		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);

		EquippedTool = ToolToEquip;

		if (auto PlayerController = Cast<APlayerControllerBase>(Controller))
		{
			if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ToolToEquip->ToolMappingContext, 1);
			}
			ToolToEquip->BindInputAction(UseAction);
		}
	}
}

void APlayerCharacterBase::GiveItem(UItemDefinition* ItemDefinition)
{
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
		auto* ToolDefinition = Cast<UEquipableToolDefinition>(ItemDefinition);
		if (ToolDefinition)
		{
			AttachTool(ToolDefinition);
		}
		break;
	case EItemType::Consumable:
		break;
	case EItemType::None:
	default:
		break;
	}
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().IsNearlyZero())
	{
		SetSprinting(false);
	}

	float CurrentStaminaRegenRate = StaminaRegenRate;
	if (bHasSprinted)
	{
		CurrentStaminaRegenRate = -StaminaRegenRate;
	}
	else if (bIsCrouched)
	{
		CurrentStaminaRegenRate = RestStaminaRegenRate;
	}

	const float OldStamina = CurrentStamina;

	CurrentStamina = FMath::Clamp(CurrentStamina + CurrentStaminaRegenRate, 0.f, MaxStamina);

	if (CurrentStamina != OldStamina)
	{
		OnStaminaChanged.Broadcast(OldStamina, CurrentStamina, MaxStamina);
	}

	bHasSprinted = false;
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int APlayerCharacterBase::GetHealth()
{
	return CurrentHealth;
}

void APlayerCharacterBase::UpdateHealth(int DeltaHealth)
{
	if (CurrentHealth <= 0.f)
	{
		return;
	}

	int OldValue = CurrentHealth;
	CurrentHealth += DeltaHealth;

	CurrentHealth = FMath::Clamp(CurrentHealth, -1.f, MaxHealth);

	if (OldValue != CurrentHealth)
	{
		OnHealthChanged.Broadcast(OldValue, CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0.f)
	{
		OnPlayerDied.Broadcast();
	}
}

void APlayerCharacterBase::SetMaxHealth(int NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

float APlayerCharacterBase::GetStamina()
{
	return CurrentStamina;
}

float APlayerCharacterBase::GetStaminaRegenRate()
{
	return StaminaRegenRate;
}

void APlayerCharacterBase::SetStaminaRegenRate(float NewStaminaRegenRate)
{
	StaminaRegenRate = NewStaminaRegenRate;
}

void APlayerCharacterBase::SetSprinting(bool IsSprinting)
{
	bIsSprinting = IsSprinting;

	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintingMaxWalkSpeed : NormalMaxWalkSpeed;
	if (bIsSprinting && bIsCrouched)
	{
		UnCrouch();
	}
}

void APlayerCharacterBase::ToggleSprint()
{
	SetSprinting(!bIsSprinting);
}

void APlayerCharacterBase::BroadcastCurrentStats()
{
	OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth, MaxHealth);
	OnStaminaChanged.Broadcast(CurrentStamina, CurrentStamina, MaxStamina);
}
