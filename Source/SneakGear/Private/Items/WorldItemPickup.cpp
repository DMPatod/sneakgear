#include "Items/WorldItemPickup.h"

#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Misc/DataValidation.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Player/Components/PlayerInventoryComponent.h"
#include "UI/PickupPromptNativeWidget.h"
#include "UI/PickupPromptWidget.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AWorldItemPickup::AWorldItemPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTrigger"));
	PickupTrigger->SetupAttachment(PickupMesh);
	PickupTrigger->SetSphereRadius(80.f);
	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);

	PickupItemComponent = CreateDefaultSubobject<UPlayerItemPickupComponent>(TEXT("PickupItemComponent"));

	PickupPromptComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupPromptComponent"));
	PickupPromptComponent->SetupAttachment(PickupMesh);
	PickupPromptComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PickupPromptComponent->SetDrawAtDesiredSize(true);
	PickupPromptComponent->SetRelativeLocation(PickupPromptOffset);
	PickupPromptComponent->SetVisibility(false);
	PickupPromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupPromptWidgetClass = UPickupPromptNativeWidget::StaticClass();
}

void AWorldItemPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	(void)DeltaSeconds;

	if (PendingPromptActor.IsValid())
	{
		UpdatePickupPrompt();
	}
}

void AWorldItemPickup::BeginPlay()
{
	Super::BeginPlay();

	if (PickupPromptComponent)
	{
		PickupPromptComponent->SetRelativeLocation(PickupPromptOffset);
		PickupPromptComponent->SetWidgetClass(PickupPromptWidgetClass
			                                      ? PickupPromptWidgetClass.Get()
			                                      : UPickupPromptNativeWidget::StaticClass());
		PickupPromptComponent->InitWidget();
		PickupPromptComponent->SetVisibility(false);
	}

	if (PickupTrigger)
	{
		PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWorldItemPickup::HandlePickupTriggerBeginOverlap);
		PickupTrigger->OnComponentEndOverlap.AddDynamic(this, &AWorldItemPickup::HandlePickupTriggerEndOverlap);
	}
}

EDataValidationResult AWorldItemPickup::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!PickupMesh)
	{
		Context.AddError(FText::FromString(TEXT("PickupMesh component is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!PickupTrigger)
	{
		Context.AddError(FText::FromString(TEXT("PickupTrigger component is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!PickupItemComponent)
	{
		Context.AddError(FText::FromString(TEXT("PickupItemComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}
	else if (!PickupItemComponent->GetItemDefinition())
	{
		Context.AddError(FText::FromString(TEXT("PickupItemComponent must have an ItemDefinition assigned.")));
		Result = EDataValidationResult::Invalid;
	}

	if (!PickupPromptComponent)
	{
		Context.AddError(FText::FromString(TEXT("PickupPromptComponent is missing.")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

FPlayerInventoryItem AWorldItemPickup::GetPickupItem() const
{
	return PickupItemComponent ? PickupItemComponent->GetPickupItem() : FPlayerInventoryItem();
}

TSubclassOf<AWeaponBase> AWorldItemPickup::GetPickupWeaponClass() const
{
	return PickupItemComponent ? PickupItemComponent->GetPickupWeaponClass() : nullptr;
}

UPlayerItemDefinition* AWorldItemPickup::GetItemDefinition() const
{
	return PickupItemComponent ? PickupItemComponent->GetItemDefinition() : nullptr;
}

void AWorldItemPickup::ConsumePickup()
{
	if (PickupItemComponent)
	{
		PickupItemComponent->ConsumePickup();
	}
	else
	{
		Destroy();
	}
}

void AWorldItemPickup::HandlePickupTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                       bool bFromSweep,
                                                       const FHitResult& SweepResult)
{
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;
	(void)bFromSweep;
	(void)SweepResult;

	if (!IsLocallyControlledPlayerActor(OtherActor))
	{
		return;
	}

	const bool bPromptReady = UpdatePickupPrompt();

	if (PickupPromptComponent)
	{
		PickupPromptComponent->SetVisibility(true);
	}

	if (!bPromptReady && GetWorld())
	{
		PendingPromptActor = OtherActor;
		GetWorld()->GetTimerManager().SetTimer(PromptRetryTimer, this, &AWorldItemPickup::RetryShowPickupPrompt, 0.01f, false);
	}
}

void AWorldItemPickup::HandlePickupTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	(void)OverlappedComponent;
	(void)OtherComp;
	(void)OtherBodyIndex;

	if (!IsLocallyControlledPlayerActor(OtherActor))
	{
		return;
	}

	if (PickupPromptComponent)
	{
		PickupPromptComponent->SetVisibility(false);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PromptRetryTimer);
	}
	PendingPromptActor.Reset();
}

void AWorldItemPickup::RetryShowPickupPrompt()
{
	if (!PendingPromptActor.IsValid() || !IsLocallyControlledPlayerActor(PendingPromptActor.Get()))
	{
		return;
	}

	if (!UpdatePickupPrompt())
	{
		return;
	}

	if (PickupPromptComponent)
	{
		PickupPromptComponent->SetVisibility(true);
	}
}

bool AWorldItemPickup::UpdatePickupPrompt()
{
	if (!PickupPromptComponent)
	{
		return false;
	}

	if (!PickupPromptComponent->GetWidget())
	{
		PickupPromptComponent->SetWidgetClass(PickupPromptWidgetClass
			                                      ? PickupPromptWidgetClass.Get()
			                                      : UPickupPromptNativeWidget::StaticClass());
		PickupPromptComponent->InitWidget();
	}

	UPickupPromptWidget* PromptWidget = Cast<UPickupPromptWidget>(PickupPromptComponent->GetWidget());
	if (!PromptWidget)
	{
		return false;
	}

	const FPlayerInventoryItem PickupItem = GetPickupItem();
	const FText ItemName = !PickupItem.DisplayName.IsEmpty()
		                       ? PickupItem.DisplayName
		                       : FText::FromName(PickupItem.ItemId);
	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	const FText SlotLabel = SlotEnum
		                        ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(PickupItem.SlotType))
		                        : FText::GetEmpty();

	bool bRequiresHoldToSwap = false;
	float HoldProgress = 0.f;
	if (const ASneakGearPlayerCharacter* PlayerCharacter = Cast<ASneakGearPlayerCharacter>(PendingPromptActor.Get()))
	{
		if (const UPlayerInventoryComponent* InventoryComponent = PlayerCharacter->GetItemComponent())
		{
			bRequiresHoldToSwap = InventoryComponent->RequiresHoldToSwapItem(PickupItem);
		}

		HoldProgress = bRequiresHoldToSwap ? PlayerCharacter->GetPickupSwapHoldProgress() : 0.f;
	}

	PromptWidget->SetPickupInfo(ItemName, SlotLabel);
	PromptWidget->SetSwapPrompt(bRequiresHoldToSwap);
	PromptWidget->SetHoldProgress(HoldProgress);
	PromptWidget->SetPromptVisible(true);
	return true;
}

bool AWorldItemPickup::IsLocallyControlledPlayerActor(const AActor* OtherActor) const
{
	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return false;
	}

	if (Pawn->IsLocallyControlled())
	{
		return true;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	return PlayerController && PlayerController->IsLocalController();
}
