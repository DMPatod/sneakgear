#include "Game/StealthPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Characters/AI/GuardCharacter.h"
#include "Radar/RadarRegistrySubsystem.h"
#include "UI/MainHUDWidget.h"
#include "UI/RadarWidget.h"
#include "Characters/Player/StealthPlayerCharacter.h"
#include "UI/CrosshairWidget.h"
#include "UI/WeaponQuickIndicatorWidget.h"
#include "UI/WeaponSelectionMenuWidget.h"
#include "TimerManager.h"

static FVector2D WorldToRadar(const FVector& PlayerLocation, const float RefYawDeg, const FVector& TargetLocation,
                              const float RadarRadiusPx, const float RadarRangeWorld)
{
	auto Delta = TargetLocation - PlayerLocation;

	FVector2D D2(Delta.X, Delta.Y);

	auto YawRad = FMath::DegreesToRadians(RefYawDeg);
	auto Cos = FMath::Cos(-YawRad);
	auto Sin = FMath::Sin(-YawRad);
	FVector2D Rotator(D2.X * Cos - D2.Y * Sin, D2.X * Sin + D2.Y * Cos);

	auto Dist = Rotator.Size();
	auto ClampedDist = FMath::Min(Dist, RadarRangeWorld);
	auto Dir = Dist > 1.f ? Rotator / Dist : FVector2D::ZeroVector;

	auto Px = ClampedDist / RadarRangeWorld * RadarRadiusPx;
	return Px * Dir;
}

AStealthPlayerController::AStealthPlayerController()
{
}

void AStealthPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupInputMappings();
	CreateHUDWidgets();
}

void AStealthPlayerController::SetupInputMappings()
{
	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is not set on %s"), *GetName());
	}
	if (!DebugMappingContext && bDebug)
	{
		UE_LOG(LogTemp, Error, TEXT("DebugMappingContext is not set on %s"), *GetName());
	}

	auto* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer && DefaultMappingContext)
	{
		auto* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

			if (bDebug)
			{
				Subsystem->AddMappingContext(DebugMappingContext, 0);
			}
		}
	}
}

void AStealthPlayerController::CreateHUDWidgets()
{
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UCrosshairWidget>(this, CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
			CrosshairWidget->SetVisible(false);
			CrosshairWidget->SetSpread(0.f);
		}
	}

	if (MainHUDWidgetClass)
	{
		MainHUDWidget = CreateWidget<UMainHUDWidget>(this, MainHUDWidgetClass);
		if (MainHUDWidget)
		{
			MainHUDWidget->AddToViewport();
		}
	}
}

void AStealthPlayerController::OnWeaponFired()
{
	SpreadCurrent = FMath::Clamp(SpreadCurrent + CrosshairSpread.ShootKick, CrosshairSpread.Min, CrosshairSpread.Max);
	if (CrosshairWidget)
	{
		CrosshairWidget->SetSpread(SpreadCurrent);
	}
}

void AStealthPlayerController::SetCrosshairVisible(bool bVisible)
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetVisible(bVisible);
	}
}

void AStealthPlayerController::UpdateCrosshairWidget(float DeltaSeconds)
{
	if (!CrosshairWidget)
	{
		return;
	}

	auto Player = Cast<AStealthPlayerCharacter>(GetPawn());
	if (!Player)
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

void AStealthPlayerController::UpdateRadarWidget()
{
	if (!MainHUDWidget)
	{
		return;
	}

	auto* RadarWidget = MainHUDWidget->GetRadarWidget();
	if (!RadarWidget)
	{
		return;
	}

	auto Pawn = GetPawn();
	if (!Pawn)
	{
		return;
	}

	auto PlayerLocation = Pawn->GetActorLocation();
	FVector ViewLoc;
	FRotator ViewRot;
	GetPlayerViewPoint(ViewLoc, ViewRot);
	auto RefYaw = ViewRot.Yaw;

	auto RadarSys = GetWorld() ? GetWorld()->GetSubsystem<URadarRegistrySubsystem>() : nullptr;
	if (!RadarSys)
	{
		return;
	}

	auto Actors = RadarSys->GetActors();

	TArray<FRadarContact> Contacts;
	Contacts.Reserve(Actors.Num());

	for (auto WeakA : Actors)
	{
		auto A = WeakA.Get();
		if (!A)
		{
			continue;
		}

		auto G = Cast<AGuardCharacter>(A);
		if (!G)
		{
			continue;
		}

		FRadarContact C;
		C.Awareness = G->GetAwareness();
		C.VisionRange = G->GetVisionRange();
		C.HearingRange = G->GetHearingRange();
		C.bHasLOS = G->HasLineOfSight();
		C.LookYawOnRadarDeg = FMath::FindDeltaAngleDegrees(RefYaw, G->GetActorRotation().Yaw);

		C.RadarPos = WorldToRadar(PlayerLocation, RefYaw, G->GetActorLocation(), RadarWidget->RadarRadiusPx,
		                          RadarWidget->RadarRangeWorld);

		Contacts.Add(C);
	}

	RadarWidget->WorldNorthYawDeg = RefYaw;
	RadarWidget->SetContacts(Contacts);
	RadarWidget->InvalidateLayoutAndVolatility();
}

void AStealthPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateRadarWidget();
	UpdateCrosshairWidget(DeltaSeconds);
}

void AStealthPlayerController::NotifyHitMarker()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->ShowHitMarker();
	}
}

void AStealthPlayerController::ShowWeaponQuickSelectIndicator(EPlayerItemSlot Slot)
{
	BP_OnWeaponQuickSelectIndicator(Slot);

	if (!WeaponQuickIndicatorWidget && WeaponQuickIndicatorWidgetClass)
	{
		WeaponQuickIndicatorWidget = CreateWidget<UWeaponQuickIndicatorWidget>(this, WeaponQuickIndicatorWidgetClass);
	}

	if (WeaponQuickIndicatorWidget)
	{
		WeaponQuickIndicatorWidget->UpdateForSlot(Slot);
		WeaponQuickIndicatorWidget->AddToViewport();
	}

	GetWorldTimerManager().ClearTimer(WeaponQuickIndicatorTimer);
	GetWorldTimerManager().SetTimer(
		WeaponQuickIndicatorTimer,
		[this]()
		{
			if (WeaponQuickIndicatorWidget)
			{
				WeaponQuickIndicatorWidget->RemoveFromParent();
			}
		},
		FMath::Max(WeaponQuickIndicatorDuration, 0.05f),
		false
	);
}

void AStealthPlayerController::OpenWeaponSelectionWidget(EPlayerItemSlot InitialSlot)
{
	if (bWeaponSelectionOpen)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(WeaponQuickIndicatorTimer);
	if (WeaponQuickIndicatorWidget)
	{
		WeaponQuickIndicatorWidget->RemoveFromParent();
	}

	bWeaponSelectionOpen = true;
	bWasGamePausedBeforeWeaponSelection = IsPaused();
	SetPause(true);

	if (!WeaponSelectionWidget && WeaponSelectionWidgetClass)
	{
		WeaponSelectionWidget = CreateWidget<UWeaponSelectionMenuWidget>(this, WeaponSelectionWidgetClass);
	}

	if (WeaponSelectionWidget)
	{
		WeaponSelectionWidget->AddToViewport(100);
		WeaponSelectionWidget->InitializeMenu(InitialSlot);
	}

	FInputModeGameAndUI InputMode;
	if (WeaponSelectionWidget)
	{
		InputMode.SetWidgetToFocus(WeaponSelectionWidget->TakeWidget());
	}
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	BP_OnWeaponSelectionOpened(InitialSlot);
}

void AStealthPlayerController::CloseWeaponSelectionWidget()
{
	if (!bWeaponSelectionOpen)
	{
		return;
	}

	bWeaponSelectionOpen = false;

	if (WeaponSelectionWidget)
	{
		WeaponSelectionWidget->RemoveFromParent();
	}

	if (!bWasGamePausedBeforeWeaponSelection)
	{
		SetPause(false);
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	BP_OnWeaponSelectionClosed();
}

void AStealthPlayerController::SelectWeaponFromSelectionMenu(EPlayerItemSlot Slot)
{
	if (Slot != EPlayerItemSlot::PrimaryWeapon && Slot != EPlayerItemSlot::SecondaryWeapon)
	{
		return;
	}

	if (AStealthPlayerCharacter* PlayerCharacter = Cast<AStealthPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->SetActiveWeaponSlot(Slot, true);
	}

	ShowWeaponQuickSelectIndicator(Slot);
	CloseWeaponSelectionWidget();
}

void AStealthPlayerController::CancelWeaponSelectionMenu()
{
	CloseWeaponSelectionWidget();
}
