#include "Player/PlayerCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Scene.h"
#include "Player/Components/PlayerAimComponent.h"
#include "Player/Components/PlayerLocomotionComponent.h"
#include "Player/Components/PlayerWeaponComponent.h"
#include "Player/SneakGearPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Game/GAS/AmmoAttributeSet.h"
#include "Game/GAS/HealthAttributeSet.h"
#include "Game/GAS/StaminaAttributeSet.h"
#include "Weapon/WeaponBase.h"

APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SetupViewComponents();
	SetupGameplayComponents();

	if (LocomotionComponent)
	{
		LocomotionComponent->SetupMovementDefaults();
	}
}

bool APlayerCharacterBase::IsAiming() const
{
	return AimComponent ? AimComponent->IsAiming() : false;
}

AWeaponBase* APlayerCharacterBase::GetCurrentWeapon() const
{
	return WeaponComponent ? WeaponComponent->GetCurrentWeapon() : nullptr;
}

bool APlayerCharacterBase::GetWeaponAimData(FVector& OutAimOrigin, FVector& OutAimDirection) const
{
	const UCameraComponent* AimCamera = nullptr;
	if (FirstPersonCamera && FirstPersonCamera->IsActive())
	{
		AimCamera = FirstPersonCamera;
	}
	else if (ThirdPersonCamera)
	{
		AimCamera = ThirdPersonCamera;
	}

	if (AimCamera)
	{
		OutAimOrigin = AimCamera->GetComponentLocation();
		OutAimDirection = AimCamera->GetForwardVector().GetSafeNormal();
		return !OutAimDirection.IsNearlyZero();
	}

	FRotator EyeRotation;
	GetActorEyesViewPoint(OutAimOrigin, EyeRotation);
	OutAimDirection = EyeRotation.Vector().GetSafeNormal();
	return !OutAimDirection.IsNearlyZero();
}

float APlayerCharacterBase::GetAmmo() const
{
	const UAmmoAttributeSet* AmmoSet = GetAmmoSet();
	return AmmoSet ? AmmoSet->GetAmmo() : 0.f;
}

float APlayerCharacterBase::ConsumeAmmo(float Amount)
{
	if (Amount <= 0.f)
	{
		return 0.f;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	const UAmmoAttributeSet* AmmoSet = GetAmmoSet();
	if (!AbilitySystemComponent || !AmmoSet)
	{
		return 0.f;
	}

	const float UsedAmmo = FMath::Clamp(Amount, 0.f, AmmoSet->GetAmmo());
	if (UsedAmmo <= 0.f)
	{
		return 0.f;
	}

	AbilitySystemComponent->ApplyModToAttribute(UAmmoAttributeSet::GetAmmoAttribute(), EGameplayModOp::Additive, -UsedAmmo);
	return UsedAmmo;
}

bool APlayerCharacterBase::ApplyHealthDelta(float DeltaHealth)
{
	if (DeltaHealth <= 0.f)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	const UHealthAttributeSet* HealthSet = GetHealthSet();
	if (!AbilitySystemComponent || !HealthSet)
	{
		return false;
	}

	const float CurrentHealth = HealthSet->GetHealth();
	const float MaxHealth = HealthSet->GetMaxHealth();
	if (CurrentHealth >= MaxHealth)
	{
		return false;
	}

	const float AppliedDelta = FMath::Min(DeltaHealth, MaxHealth - CurrentHealth);
	AbilitySystemComponent->ApplyModToAttribute(UHealthAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, AppliedDelta);
	return AppliedDelta > 0.f;
}

FActiveGameplayEffectHandle APlayerCharacterBase::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (!GameplayEffectClass)
	{
		return FActiveGameplayEffectHandle();
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return FActiveGameplayEffectHandle();
	}

	const FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContext);
	if (!EffectSpec.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}

	return AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
}

bool APlayerCharacterBase::RemoveGameplayEffectFromSelf(FActiveGameplayEffectHandle EffectHandle)
{
	if (!EffectHandle.IsValid())
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	return AbilitySystemComponent && AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle) > 0;
}

void APlayerCharacterBase::SetCameraPostProcessMaterialEnabled(UMaterialInterface* Material, bool bEnabled, float Weight)
{
	auto UpdateCameraBlendable = [Material, bEnabled, Weight](UCameraComponent* Camera)
	{
		if (!Camera || !Material)
		{
			return;
		}

		TArray<FWeightedBlendable>& Blendables = Camera->PostProcessSettings.WeightedBlendables.Array;
		for (int32 Index = 0; Index < Blendables.Num(); ++Index)
		{
			FWeightedBlendable& Blendable = Blendables[Index];
			if (Blendable.Object == Material)
			{
				if (bEnabled)
				{
					Blendable.Weight = Weight;
					Camera->PostProcessBlendWeight = 1.f;
				}
				else
				{
					Blendables.RemoveAt(Index);
				}
				return;
			}
		}

		if (bEnabled)
		{
			Blendables.Add(FWeightedBlendable(Weight, Material));
			Camera->PostProcessBlendWeight = 1.f;
		}
	};

	UpdateCameraBlendable(ThirdPersonCamera);
	UpdateCameraBlendable(FirstPersonCamera);
}

void APlayerCharacterBase::SetHUDOverlayWidgetVisible(TSubclassOf<UUserWidget> WidgetClass, bool bVisible)
{
	if (ASneakGearPlayerController* PlayerController = Cast<ASneakGearPlayerController>(GetController()))
	{
		PlayerController->SetOverlayWidgetVisible(WidgetClass, bVisible);
	}
}

float APlayerCharacterBase::GetMaxSpeed() const
{
	return LocomotionComponent ? LocomotionComponent->GetMaxSpeed() : 0.f;
}

bool APlayerCharacterBase::GetPlayerVitalsViewData(FPlayerVitalsViewData& OutData) const
{
	const UHealthAttributeSet* HealthSet = GetHealthSet();
	const UStaminaAttributeSet* StaminaSet = GetStaminaSet();
	if (!HealthSet || !StaminaSet)
	{
		return false;
	}

	OutData.Health = HealthSet->GetHealth();
	OutData.MaxHealth = HealthSet->GetMaxHealth();
	OutData.Stamina = StaminaSet->GetStamina();
	OutData.MaxStamina = StaminaSet->GetMaxStamina();
	return true;
}

bool APlayerCharacterBase::GetWeaponStatusViewData(FWeaponStatusViewData& OutData) const
{
	const AWeaponBase* Weapon = GetCurrentWeapon();
	if (!Weapon)
	{
		return false;
	}

	OutData.bHasWeapon = true;
	OutData.WeaponName = FText::FromString(Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName());
	OutData.FireRate = Weapon->GetFireRate();

	const UPlayerWeaponComponent* PlayerWeaponComponent = FindComponentByClass<UPlayerWeaponComponent>();
	OutData.InClip = PlayerWeaponComponent ? FMath::Max(PlayerWeaponComponent->GetInClip(), 0) : 0;
	OutData.ClipSize = PlayerWeaponComponent ? FMath::Max(PlayerWeaponComponent->GetClipSize(), 0) : 0;
	OutData.ReserveAmmo = FMath::Max(FMath::FloorToInt(GetAmmo()), 0);
	return true;
}

bool APlayerCharacterBase::GetWeaponQuickSlotViewData(EPlayerItemSlot Slot, FWeaponQuickSlotViewData& OutData) const
{
	(void)Slot;
	(void)OutData;
	return false;
}

bool APlayerCharacterBase::GetStealthDebugViewData(FStealthDebugViewData& OutData) const
{
	OutData.bHasPlayer = true;
	OutData.bAiming = IsAiming();
	OutData.Stance = Stance;
	OutData.Speed2D = GetVelocity().Size2D();

	if (const AWeaponBase* Weapon = GetCurrentWeapon())
	{
		OutData.WeaponName = FText::FromString(Weapon->GetClass() ? Weapon->GetClass()->GetName() : Weapon->GetName());
	}

	return true;
}

bool APlayerCharacterBase::GetCurrentStanceForUI(EStance& OutStance) const
{
	OutStance = Stance;
	return true;
}

FText APlayerCharacterBase::GetInventoryItemDisplayName(EPlayerItemSlot Slot) const
{
	(void)Slot;
	return FText::GetEmpty();
}

int32 APlayerCharacterBase::GetInventoryItemCount(EPlayerItemSlot Slot) const
{
	(void)Slot;
	return 0;
}

FText APlayerCharacterBase::GetInventoryItemDisplayNameAt(EPlayerItemSlot Slot, int32 Index) const
{
	(void)Slot;
	(void)Index;
	return FText::GetEmpty();
}

int32 APlayerCharacterBase::GetActiveInventoryItemIndex(EPlayerItemSlot Slot) const
{
	(void)Slot;
	return INDEX_NONE;
}

FOnPlayerUIVitalsChanged& APlayerCharacterBase::OnPlayerUIVitalsChangedEvent()
{
	return OnPlayerUIVitalsChanged;
}

FOnPlayerUIWeaponStateChanged& APlayerCharacterBase::OnPlayerUIWeaponStateChangedEvent()
{
	return OnPlayerUIWeaponStateChanged;
}

FOnPlayerUIStanceChanged& APlayerCharacterBase::OnPlayerUIStanceChangedEvent()
{
	return OnStanceChanged;
}

void APlayerCharacterBase::SetupViewComponents()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 60.f));

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("first_person_camera_attachment"));
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 8.f, 4.f));
	FirstPersonCamera->SetActive(false);
	ThirdPersonCamera->SetActive(true);
}

void APlayerCharacterBase::SetupGameplayComponents()
{
	WeaponComponent = CreateDefaultSubobject<UPlayerWeaponComponent>(TEXT("WeaponComponent"));
	AimComponent = CreateDefaultSubobject<UPlayerAimComponent>(TEXT("AimComponent"));
	LocomotionComponent = CreateDefaultSubobject<UPlayerLocomotionComponent>(TEXT("LocomotionComponent"));
}

void APlayerCharacterBase::InitializeGameplayState()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->Initialize(CameraBoom);
	}

	if (AimComponent)
	{
		AimComponent->Initialize(CameraBoom, ThirdPersonCamera, FirstPersonCamera);
	}

	if (WeaponComponent && WeaponComponent->GetCurrentWeapon())
	{
		WeaponComponent->ToggleEquip();
	}

	if (LocomotionComponent)
	{
		LocomotionComponent->UpdateRotationMode(IsAiming());
	}
}

void APlayerCharacterBase::SetStance(EStance NewStance)
{
	const EStance PreviousStance = Stance;

	if (LocomotionComponent)
	{
		LocomotionComponent->SetStance(NewStance);
	}

	if (Stance != PreviousStance)
	{
		OnStanceChanged.Broadcast(Stance);
	}
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetMaxAmmoAttribute(), FMath::Max(InitialMaxAmmo, 0.f));
		AbilitySystemComponent->SetNumericAttributeBase(
			UAmmoAttributeSet::GetAmmoAttribute(),
			FMath::Clamp(InitialAmmo, 0.f, FMath::Max(InitialMaxAmmo, 0.f))
		);
	}

	InitializeGameplayState();
	BindUIDataDelegates();
}

void APlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	BindUIDataDelegates();
}

void APlayerCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindUIDataDelegates();
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* Eic = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Eic)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not an enhanced input component"));
		return;
	}

	BindInputActions(Eic);
}

void APlayerCharacterBase::BindInputActions(UEnhancedInputComponent* Eic)
{
	if (MoveAction)
	{
		Eic->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
	}
	else
	{
		ensureMsgf(false, TEXT("MoveAction is not set on %s"), *GetName());
	}

	if (LookAction)
	{
		Eic->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);
	}
	else
	{
		ensureMsgf(false, TEXT("LookAction is not set on %s"), *GetName());
	}

	if (FireAction)
	{
		Eic->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacterBase::StartFire);
		Eic->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::StopFire);
	}

	if (AimAction)
	{
		Eic->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacterBase::StartAim);
		Eic->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::StopAim);
	}

	if (AimViewToggleAction)
	{
		Eic->BindAction(AimViewToggleAction, ETriggerEvent::Started, this, &APlayerCharacterBase::ToggleAimView);
	}

	if (SprintToggleAction)
	{
		Eic->BindAction(SprintToggleAction, ETriggerEvent::Started, this, &APlayerCharacterBase::ToggleSprint);
	}

	if (JumpAction)
	{
		Eic->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnJumpPressed);
		Eic->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::OnJumpReleased);
	}

	if (ReloadAction)
	{
		Eic->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacterBase::ReloadWeapon);
	}

	if (EquipAction)
	{
		Eic->BindAction(EquipAction, ETriggerEvent::Started, this, &APlayerCharacterBase::ToggleEquip);
	}

	if (StanceAction)
	{
		Eic->BindAction(StanceAction, ETriggerEvent::Started, this, &APlayerCharacterBase::OnStancePressed);
		Eic->BindAction(StanceAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::OnStanceReleased);
	}
}

void APlayerCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (LocomotionComponent)
	{
		LocomotionComponent->TickLocomotion(DeltaSeconds);
	}
}

void APlayerCharacterBase::Move(const FInputActionValue& Value)
{
	if (!LocomotionComponent)
	{
		return;
	}

	LocomotionComponent->Move(Value);
}

void APlayerCharacterBase::Look(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	const float TurnScalar = AimComponent ? AimComponent->GetCurrentTurnScalar() : 1.f;
	AddControllerYawInput(Axis.X * TurnScalar);
	AddControllerPitchInput(Axis.Y * TurnScalar);
}

void APlayerCharacterBase::StartAim()
{
	if (AimComponent)
	{
		AimComponent->StartAim();

		ApplyAimRotationMode(true);
	}
}

void APlayerCharacterBase::StopAim()
{
	if (AimComponent)
	{
		AimComponent->StopAim();

		ApplyAimRotationMode(false);
	}
}

void APlayerCharacterBase::ApplyAimRotationMode(bool bEnableAimRotation)
{
	bUseControllerRotationYaw = bEnableAimRotation;

	if (LocomotionComponent)
	{
		LocomotionComponent->UpdateRotationMode(IsAiming());
	}

	if (bEnableAimRotation && Controller)
	{
		const FRotator ControlYaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
		SetActorRotation(ControlYaw);
	}
}

void APlayerCharacterBase::ToggleAimView()
{
	if (AimComponent)
	{
		AimComponent->ToggleAimView();
	}
}

void APlayerCharacterBase::ToggleEquip()
{
	if (!WeaponComponent)
	{
		return;
	}

	WeaponComponent->ToggleEquip();
}

void APlayerCharacterBase::ToggleSprint()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->ToggleSprint();
	}
}

void APlayerCharacterBase::ReloadWeapon()
{
	if (WeaponComponent)
	{
		WeaponComponent->Reload();
	}
}

void APlayerCharacterBase::OnJumpPressed()
{
	Jump();
}

void APlayerCharacterBase::OnJumpReleased()
{
	StopJumping();
}

void APlayerCharacterBase::OnStancePressed()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->OnStancePressed();
	}
}

void APlayerCharacterBase::OnStanceReleased()
{
	if (LocomotionComponent)
	{
		LocomotionComponent->OnStanceReleased();
	}
}

void APlayerCharacterBase::BindUIDataDelegates()
{
	if (bHasBoundUIDataDelegates)
	{
		return;
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddUObject(
			this, &APlayerCharacterBase::HandleHealthAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaAttribute()).AddUObject(
			this, &APlayerCharacterBase::HandleStaminaAttributeChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetAmmoAttribute()).AddUObject(
			this, &APlayerCharacterBase::HandleAmmoAttributeChanged);
		bHasBoundUIDataDelegates = true;
	}
}

void APlayerCharacterBase::UnbindUIDataDelegates()
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaAttribute()).RemoveAll(this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetAmmoAttribute()).RemoveAll(this);
	}

	bHasBoundUIDataDelegates = false;
}

void APlayerCharacterBase::HandleHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	(void)Data;
	OnPlayerUIVitalsChanged.Broadcast();
}

void APlayerCharacterBase::HandleStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
	(void)Data;
	OnPlayerUIVitalsChanged.Broadcast();
}

void APlayerCharacterBase::HandleAmmoAttributeChanged(const FOnAttributeChangeData& Data)
{
	(void)Data;
	OnPlayerUIWeaponStateChanged.Broadcast();
}

void APlayerCharacterBase::StartFire()
{
	if (WeaponComponent && IsAiming())
	{
		WeaponComponent->StartFire();
	}
}

void APlayerCharacterBase::StopFire()
{
	if (WeaponComponent)
	{
		WeaponComponent->StopFire();
	}
}
