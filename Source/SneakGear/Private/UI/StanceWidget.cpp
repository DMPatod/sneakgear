#include "UI/StanceWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/Player/ThirdPersonPlayerCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"

void UStanceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TryCachePlayer();

	UpdateFromPlayer();
}

void UStanceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// If widgets are already supplied by a UMG blueprint, do not build a runtime fallback tree.
	if (StanceImage || StanceText)
	{
		TryCachePlayer();
		UpdateFromPlayer();
		return;
	}

	auto Root = WidgetTree->RootWidget;
	if (!Root)
	{
		Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StanceRoot"));
		WidgetTree->RootWidget = Root;
	}

	auto VerticalBox = Cast<UVerticalBox>(Root);
	if (!VerticalBox)
	{
		Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("StanceRoot"));
		WidgetTree->RootWidget = Root;
		VerticalBox = Cast<UVerticalBox>(Root);
	}

	if (!VerticalBox)
	{
		return;
	}

	StanceImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("StanceImage"));
	VerticalBox->AddChild(StanceImage);

	StanceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StanceText"));
	VerticalBox->AddChild(StanceText);

	TryCachePlayer();
	UpdateFromPlayer();
}

void UStanceWidget::NativeDestruct()
{
	CachedPlayer.Reset();
	bHasInitializedUI = false;

	Super::NativeDestruct();
}

bool UStanceWidget::TryCachePlayer()
{
	if (CachedPlayer.IsValid())
	{
		return true;
	}

	CachedPlayer = Cast<AThirdPersonPlayerCharacter>(GetOwningPlayerPawn());
	if (CachedPlayer.IsValid())
	{
		return true;
	}

	if (const APlayerController* OwningController = GetOwningPlayer())
	{
		CachedPlayer = Cast<AThirdPersonPlayerCharacter>(OwningController->GetPawn());
	}

	return CachedPlayer.IsValid();
}

void UStanceWidget::UpdateFromPlayer()
{
	auto Player = CachedPlayer.Get();
	if (!Player || (!StanceImage && !StanceText))
	{
		return;
	}

	auto Stance = Player->Stance;
	if (!bHasInitializedUI || Stance != CurrentStance)
	{
		if (StanceImage)
		{
			if (const TObjectPtr<UTexture2D>* Texture = StanceTextures.Find(Stance))
			{
				StanceImage->SetBrushFromTexture(*Texture);
			}
		}

		if (StanceText)
		{
			const UEnum* StanceEnum = StaticEnum<EStance>();
			const FText DisplayName = StanceEnum
				                          ? StanceEnum->GetDisplayNameTextByValue(static_cast<int64>(Stance))
				                          : FText::FromString(TEXT("Unknown"));
			StanceText->SetText(DisplayName);
		}

		CurrentStance = Stance;
		bHasInitializedUI = true;
	}
}
