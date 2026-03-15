#include "Items/WorldItemPickup.h"

#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/PlayerItemPickupComponent.h"
#include "Misc/DataValidation.h"
#include "UI/PickupPromptNativeWidget.h"
#include "UI/PickupPromptWidget.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

AWorldItemPickup::AWorldItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

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

	UpdatePickupPrompt();

	if (PickupPromptComponent)
	{
		PickupPromptComponent->SetVisibility(true);
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
}

void AWorldItemPickup::UpdatePickupPrompt()
{
	if (!PickupPromptComponent)
	{
		return;
	}

	UPickupPromptWidget* PromptWidget = Cast<UPickupPromptWidget>(PickupPromptComponent->GetWidget());
	if (!PromptWidget)
	{
		return;
	}

	const FPlayerInventoryItem PickupItem = GetPickupItem();
	const FText ItemName = !PickupItem.DisplayName.IsEmpty()
		                       ? PickupItem.DisplayName
		                       : FText::FromName(PickupItem.ItemId);
	const UEnum* SlotEnum = StaticEnum<EPlayerItemSlot>();
	const FText SlotLabel = SlotEnum
		                        ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(PickupItem.SlotType))
		                        : FText::GetEmpty();

	PromptWidget->SetPickupInfo(ItemName, SlotLabel);
	PromptWidget->SetPromptVisible(true);
}

bool AWorldItemPickup::IsLocallyControlledPlayerActor(const AActor* OtherActor) const
{
	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return false;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	return PlayerController && PlayerController->IsLocalController();
}
