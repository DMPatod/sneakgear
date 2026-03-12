#include "UI/CrosshairWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCrosshairWidget::SetVisible(bool bVisible)
{
	SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::SetSpread(float InSpread)
{
	if (Slot_Up)
	{
		Slot_Up->SetPosition(BasePosUp + FVector2D(0.f, -InSpread));
	}
	if (Slot_Down)
	{
		Slot_Down->SetPosition(BasePosDown + FVector2D(0.f, InSpread));
	}
	if (Slot_Left)
	{
		Slot_Left->SetPosition(BasePosLeft + FVector2D(-InSpread, 0.f));
	}
	if (Slot_Right)
	{
		Slot_Right->SetPosition(BasePosRight + FVector2D(InSpread, 0.f));
	}
}

void UCrosshairWidget::ShowHitMarker(float Duration)
{
	if (!HitMarker)
	{
		return;
	}

	HitMarker->SetVisibility(ESlateVisibility::Visible);
	HitMarker->SetOpacity(1.f);

	auto World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(HitMarkerTimer);
	World->GetTimerManager().SetTimer(HitMarkerTimer, this, &UCrosshairWidget::HideHitMarker, Duration);
}

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Slot_Up = Cast<UCanvasPanelSlot>(Crosshair_Up ? Crosshair_Up->Slot : nullptr);
	Slot_Down = Cast<UCanvasPanelSlot>(Crosshair_Down ? Crosshair_Down->Slot : nullptr);
	Slot_Left = Cast<UCanvasPanelSlot>(Crosshair_Left ? Crosshair_Left->Slot : nullptr);
	Slot_Right = Cast<UCanvasPanelSlot>(Crosshair_Right ? Crosshair_Right->Slot : nullptr);

	if (Slot_Up)
	{
		BasePosUp = Slot_Up->GetPosition();
	}
	if (Slot_Down)
	{
		BasePosDown = Slot_Down->GetPosition();
	}
	if (Slot_Left)
	{
		BasePosLeft = Slot_Left->GetPosition();
	}
	if (Slot_Right)
	{
		BasePosRight = Slot_Right->GetPosition();
	}

	if (HitMarker)
	{
		HitMarker->SetVisibility(ESlateVisibility::Hidden);
		HitMarker->SetOpacity(0.f);
	}
}

void UCrosshairWidget::HideHitMarker()
{
	if (!HitMarker)
	{
		return;
	}

	HitMarker->SetVisibility(ESlateVisibility::Hidden);
	HitMarker->SetOpacity(0.f);
}
