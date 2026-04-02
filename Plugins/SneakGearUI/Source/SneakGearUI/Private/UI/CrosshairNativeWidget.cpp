#include "UI/CrosshairNativeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	void ConfigureCanvasSlot(UCanvasPanelSlot* Slot, const FVector2D& Size, const FVector2D& Position)
	{
		if (!Slot)
		{
			return;
		}

		Slot->SetAutoSize(false);
		Slot->SetAnchors(FAnchors(0.5f, 0.5f));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetSize(Size);
		Slot->SetPosition(Position);
	}
}

UCrosshairNativeWidget::UCrosshairNativeWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> WhiteTextureFinder(
		TEXT("/Engine/EngineResources/WhiteSquareTexture.WhiteSquareTexture"));
	if (WhiteTextureFinder.Succeeded())
	{
		WhiteTexture = WhiteTextureFinder.Object;
	}
}

bool UCrosshairNativeWidget::Initialize()
{
	const bool Result = Super::Initialize();
	BuildWidgetTree();
	return Result;
}

void UCrosshairNativeWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCrosshairNativeWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* Root = Cast<UCanvasPanel>(WidgetTree->RootWidget);
	if (!Root)
	{
		Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CrosshairRoot"));
		WidgetTree->RootWidget = Root;
	}

	if (!Crosshair_Up)
	{
		Crosshair_Up = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Crosshair_Up"));
		Crosshair_Up->SetBrushFromTexture(WhiteTexture, true);
		Crosshair_Up->SetColorAndOpacity(CrosshairColor);
		ConfigureCanvasSlot(Cast<UCanvasPanelSlot>(Root->AddChild(Crosshair_Up)), FVector2D(ArmThickness, ArmLength),
		                    FVector2D(0.f, -(CenterGap + ArmLength)));
	}

	if (!Crosshair_Down)
	{
		Crosshair_Down = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Crosshair_Down"));
		Crosshair_Down->SetBrushFromTexture(WhiteTexture, true);
		Crosshair_Down->SetColorAndOpacity(CrosshairColor);
		ConfigureCanvasSlot(Cast<UCanvasPanelSlot>(Root->AddChild(Crosshair_Down)), FVector2D(ArmThickness, ArmLength),
		                    FVector2D(0.f, CenterGap));
	}

	if (!Crosshair_Left)
	{
		Crosshair_Left = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Crosshair_Left"));
		Crosshair_Left->SetBrushFromTexture(WhiteTexture, true);
		Crosshair_Left->SetColorAndOpacity(CrosshairColor);
		ConfigureCanvasSlot(Cast<UCanvasPanelSlot>(Root->AddChild(Crosshair_Left)), FVector2D(ArmLength, ArmThickness),
		                    FVector2D(-(CenterGap + ArmLength), 0.f));
	}

	if (!Crosshair_Right)
	{
		Crosshair_Right = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Crosshair_Right"));
		Crosshair_Right->SetBrushFromTexture(WhiteTexture, true);
		Crosshair_Right->SetColorAndOpacity(CrosshairColor);
		ConfigureCanvasSlot(Cast<UCanvasPanelSlot>(Root->AddChild(Crosshair_Right)), FVector2D(ArmLength, ArmThickness),
		                    FVector2D(CenterGap, 0.f));
	}

	if (!HitMarker)
	{
		HitMarker = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("HitMarker"));
		HitMarker->SetBrushFromTexture(WhiteTexture, true);
		HitMarker->SetColorAndOpacity(HitMarkerColor);
		ConfigureCanvasSlot(Cast<UCanvasPanelSlot>(Root->AddChild(HitMarker)), FVector2D(HitMarkerSize, HitMarkerSize),
		                    FVector2D(0.f, 0.f));
	}
}


