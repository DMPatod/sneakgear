#include "UI/DefaultHudLayoutBase.h"

#include "UI/HealthStaminaBarsBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UDefaultHudLayoutBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Crosshair)
	{
		if (UCanvasPanelSlot* Canvas = Cast<UCanvasPanelSlot>(Crosshair->Slot))
		{
			FAnchors Anchors;
			Anchors.Minimum = FVector2D(0.5, 0.5);
			Anchors.Maximum = FVector2D(0.5, 0.5);

			Canvas->SetAnchors(Anchors);
			Canvas->SetSize(FVector2D(CrosshairSize, CrosshairSize));
			Canvas->SetAlignment(FVector2D(0.5, 0.5));
		}
	}
}
