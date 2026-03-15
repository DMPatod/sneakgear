#include "Player/Components/PlayerHUDComponent.h"

#include "Blueprint/UserWidget.h"
#include "Guards/GuardCharacter.h"
#include "Player/SneakGearPlayerCharacter.h"
#include "Player/SneakGearPlayerController.h"
#include "Radar/RadarRegistrySubsystem.h"
#include "UI/CrosshairWidget.h"
#include "UI/PlayerHUDWidget.h"
#include "UI/RadarWidget.h"

UPlayerHUDComponent::UPlayerHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerHUDComponent::Initialize(TSubclassOf<UPlayerHUDWidget> InPlayerHUDWidgetClass,
                                     TSubclassOf<UCrosshairWidget> InCrosshairWidgetClass,
                                     const FCrosshairSpreadConfig& InCrosshairSpread,
                                     bool bInShowCrosshairOnlyWhenAiming)
{
	PlayerHUDWidgetClass = InPlayerHUDWidgetClass;
	CrosshairWidgetClass = InCrosshairWidgetClass;
	CrosshairSpread = InCrosshairSpread;
	bShowCrosshairOnlyWhenAiming = bInShowCrosshairOnlyWhenAiming;
	RadarRefreshCooldown = 0.f;
	CreateHUDWidgets();
}

void UPlayerHUDComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerHUDComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RadarRefreshCooldown -= DeltaTime;
	if (RadarRefreshCooldown <= 0.f)
	{
		UpdateRadarWidget();
		RadarRefreshCooldown = RadarRefreshInterval;
	}
	UpdateCrosshairWidget(DeltaTime);
}

void UPlayerHUDComponent::OnWeaponFired()
{
	SpreadCurrent = FMath::Clamp(SpreadCurrent + CrosshairSpread.ShootKick, CrosshairSpread.Min, CrosshairSpread.Max);
	if (CrosshairWidget)
	{
		CrosshairWidget->SetSpread(SpreadCurrent);
	}
}

void UPlayerHUDComponent::SetCrosshairVisible(bool bVisible) const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisible(bVisible);
	}
}

void UPlayerHUDComponent::NotifyHitMarker() const
{
	if (CrosshairWidget)
	{
		CrosshairWidget->ShowHitMarker();
	}
}

void UPlayerHUDComponent::SetOverlayWidgetVisible(TSubclassOf<UUserWidget> WidgetClass, bool bVisible)
{
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
	UClass* OverlayClass = WidgetClass.Get();
	if (!Controller || !OverlayClass)
	{
		return;
	}

	if (bVisible)
	{
		TObjectPtr<UUserWidget>& OverlayWidget = OverlayWidgets.FindOrAdd(OverlayClass);
		if (!OverlayWidget)
		{
			OverlayWidget = CreateWidget<UUserWidget>(Controller, WidgetClass);
		}

		if (OverlayWidget && !OverlayWidget->IsInViewport())
		{
			OverlayWidget->AddToViewport();
		}
		return;
	}

	if (TObjectPtr<UUserWidget>* ExistingWidget = OverlayWidgets.Find(OverlayClass))
	{
		if (*ExistingWidget)
		{
			(*ExistingWidget)->RemoveFromParent();
		}
	}
}

void UPlayerHUDComponent::CreateHUDWidgets()
{
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
	if (!Controller)
	{
		return;
	}

	if (!CrosshairWidget && CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UCrosshairWidget>(Controller, CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
			CrosshairWidget->SetVisible(!bShowCrosshairOnlyWhenAiming);
			CrosshairWidget->SetSpread(0.f);
		}
	}

	if (!PlayerHUDWidget && PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(Controller, PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
}

void UPlayerHUDComponent::UpdateRadarWidget()
{
	URadarWidget* RadarWidget = GetRadarWidget();
	ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	if (!RadarWidget || !Pawn)
	{
		return;
	}

	FVector ViewLoc;
	FRotator ViewRot;
	Controller->GetPlayerViewPoint(ViewLoc, ViewRot);
	const float RefYaw = ViewRot.Yaw;

	URadarRegistrySubsystem* RadarSys = GetWorld() ? GetWorld()->GetSubsystem<URadarRegistrySubsystem>() : nullptr;
	if (!RadarSys)
	{
		return;
	}

	RadarSys->BuildGuardContacts(Pawn, RefYaw, RadarWidget->RadarRadiusPx, RadarWidget->RadarRangeWorld, RadarContactsCache);

	RadarWidget->WorldNorthYawDeg = RefYaw;
	RadarWidget->SetContacts(RadarContactsCache);
	RadarWidget->InvalidateLayoutAndVolatility();
}

void UPlayerHUDComponent::UpdateCrosshairWidget(float DeltaSeconds)
{
	ASneakGearPlayerCharacter* Player = GetOwningSneakGearPlayerCharacter();
	if (!CrosshairWidget || !Player)
	{
		return;
	}

	const float Speed2D = Player->GetVelocity().Size2D();
	const float MaxSpeed = Player->GetMaxSpeed();
	const float MoveAlpha = MaxSpeed > KINDA_SMALL_NUMBER ? FMath::Clamp(Speed2D / MaxSpeed, 0.f, 1.f) : 0.f;

	SpreadTarget = CrosshairSpread.Min + MoveAlpha * CrosshairSpread.FromMove;

	const bool bAiming = Player->IsAiming();
	SetCrosshairVisible(!bShowCrosshairOnlyWhenAiming || bAiming);

	if (bAiming)
	{
		SpreadTarget *= CrosshairSpread.FromAimMultiplier;
	}

	SpreadTarget = FMath::Clamp(SpreadTarget, CrosshairSpread.Min, CrosshairSpread.Max);
	SpreadCurrent = FMath::FInterpTo(SpreadCurrent, SpreadTarget, DeltaSeconds, CrosshairSpread.InterpolationSpeed);
	CrosshairWidget->SetSpread(SpreadCurrent);
}

ASneakGearPlayerController* UPlayerHUDComponent::GetOwningSneakGearPlayerController() const
{
	return Cast<ASneakGearPlayerController>(GetOwner());
}

ASneakGearPlayerCharacter* UPlayerHUDComponent::GetOwningSneakGearPlayerCharacter() const
{
	if (const ASneakGearPlayerController* Controller = GetOwningSneakGearPlayerController())
	{
		return Cast<ASneakGearPlayerCharacter>(Controller->GetPawn());
	}

	return nullptr;
}

URadarWidget* UPlayerHUDComponent::GetRadarWidget() const
{
	return PlayerHUDWidget ? PlayerHUDWidget->GetRadarWidget() : nullptr;
}
