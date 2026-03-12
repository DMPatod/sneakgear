#include "Player/Components/PlayerHUDComponent.h"

#include "Guards/GuardCharacter.h"
#include "Player/StealthPlayerCharacter.h"
#include "Player/StealthPlayerController.h"
#include "Radar/RadarRegistrySubsystem.h"
#include "UI/CrosshairWidget.h"
#include "UI/MainHUDWidget.h"
#include "UI/RadarWidget.h"

UPlayerHUDComponent::UPlayerHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerHUDComponent::Initialize(TSubclassOf<UMainHUDWidget> InMainHUDWidgetClass,
                                     TSubclassOf<UCrosshairWidget> InCrosshairWidgetClass,
                                     const FCrosshairSpreadConfig& InCrosshairSpread)
{
	MainHUDWidgetClass = InMainHUDWidgetClass;
	CrosshairWidgetClass = InCrosshairWidgetClass;
	CrosshairSpread = InCrosshairSpread;
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

void UPlayerHUDComponent::CreateHUDWidgets()
{
	AStealthPlayerController* Controller = GetOwningStealthPlayerController();
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
			CrosshairWidget->SetVisible(false);
			CrosshairWidget->SetSpread(0.f);
		}
	}

	if (!MainHUDWidget && MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UMainHUDWidget>(Controller, MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}

void UPlayerHUDComponent::UpdateRadarWidget()
{
	URadarWidget* RadarWidget = GetRadarWidget();
	AStealthPlayerController* Controller = GetOwningStealthPlayerController();
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
	AStealthPlayerCharacter* Player = GetOwningStealthPlayerCharacter();
	if (!CrosshairWidget || !Player)
	{
		return;
	}

	const float Speed2D = Player->GetVelocity().Size2D();
	const float MaxSpeed = Player->GetMaxSpeed();
	const float MoveAlpha = MaxSpeed > KINDA_SMALL_NUMBER ? FMath::Clamp(Speed2D / MaxSpeed, 0.f, 1.f) : 0.f;

	SpreadTarget = CrosshairSpread.Min + MoveAlpha * CrosshairSpread.FromMove;

	const bool bAiming = Player->IsAiming();
	SetCrosshairVisible(bAiming);

	if (bAiming)
	{
		SpreadTarget *= CrosshairSpread.FromAimMultiplier;
	}

	SpreadTarget = FMath::Clamp(SpreadTarget, CrosshairSpread.Min, CrosshairSpread.Max);
	SpreadCurrent = FMath::FInterpTo(SpreadCurrent, SpreadTarget, DeltaSeconds, CrosshairSpread.InterpolationSpeed);
	CrosshairWidget->SetSpread(SpreadCurrent);
}

AStealthPlayerController* UPlayerHUDComponent::GetOwningStealthPlayerController() const
{
	return Cast<AStealthPlayerController>(GetOwner());
}

AStealthPlayerCharacter* UPlayerHUDComponent::GetOwningStealthPlayerCharacter() const
{
	if (const AStealthPlayerController* Controller = GetOwningStealthPlayerController())
	{
		return Cast<AStealthPlayerCharacter>(Controller->GetPawn());
	}

	return nullptr;
}

URadarWidget* UPlayerHUDComponent::GetRadarWidget() const
{
	return MainHUDWidget ? MainHUDWidget->GetRadarWidget() : nullptr;
}
