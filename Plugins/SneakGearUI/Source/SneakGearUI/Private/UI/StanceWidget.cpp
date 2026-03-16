#include "UI/StanceWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "UI/PlayerUIDataSource.h"

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
	if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		PlayerUIDataSource->OnPlayerUIStanceChangedEvent().RemoveAll(this);
	}

	CachedPlayer.Reset();
	bHasInitializedUI = false;

	Super::NativeDestruct();
}

void UStanceWidget::SetObservedPlayer(APawn* InPawn)
{
	if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		PlayerUIDataSource->OnPlayerUIStanceChangedEvent().RemoveAll(this);
	}

	CachedPlayer = InPawn;

	if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
	{
		PlayerUIDataSource->OnPlayerUIStanceChangedEvent().AddUObject(this, &UStanceWidget::HandleStanceChanged);
	}

	UpdateFromPlayer();
}

bool UStanceWidget::TryCachePlayer()
{
	if (CachedPlayer.IsValid())
	{
		if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
		{
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().RemoveAll(this);
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().AddUObject(this, &UStanceWidget::HandleStanceChanged);
		}
		return true;
	}

	CachedPlayer = GetOwningPlayerPawn();
	if (CachedPlayer.IsValid())
	{
		if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
		{
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().RemoveAll(this);
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().AddUObject(this, &UStanceWidget::HandleStanceChanged);
		}
		return true;
	}

	if (const APlayerController* OwningController = GetOwningPlayer())
	{
		CachedPlayer = OwningController->GetPawn();
	}

	if (CachedPlayer.IsValid())
	{
		if (IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource())
		{
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().RemoveAll(this);
			PlayerUIDataSource->OnPlayerUIStanceChangedEvent().AddUObject(this, &UStanceWidget::HandleStanceChanged);
		}
	}

	return CachedPlayer.IsValid();
}

IPlayerUIDataSource* UStanceWidget::GetPlayerUIDataSource() const
{
	return CachedPlayer.IsValid() ? Cast<IPlayerUIDataSource>(CachedPlayer.Get()) : nullptr;
}

void UStanceWidget::HandleStanceChanged(EStance NewStance)
{
	(void)NewStance;
	UpdateFromPlayer();
}

void UStanceWidget::UpdateFromPlayer()
{
	if ((!StanceImage && !StanceText))
	{
		return;
	}

	IPlayerUIDataSource* PlayerUIDataSource = GetPlayerUIDataSource();
	EStance Stance = EStance::Standing;
	if (!PlayerUIDataSource || !PlayerUIDataSource->GetCurrentStanceForUI(Stance))
	{
		return;
	}

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
