#include "UI/StatBarBase.h"

#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void UStatBarBase::OnIntStatUpdated(int32 OldValue, int32 NewValue, int32 MaxValue)
{
	OnFloatStatUpdated(static_cast<float>(OldValue), static_cast<float>(NewValue), MaxValue);
}

void UStatBarBase::OnFloatStatUpdated(float OldValue, float NewValue, float MaxValue)
{
	if (MaxValue == 0.f)
	{
		MaxValue = KINDA_SMALL_NUMBER;
	}

	CurrentPercentage = FMath::Clamp(NewValue / MaxValue, 0.f, 1.f);
	CurrentValue = NewValue;
	UpdatedWidget();
}

void UStatBarBase::SetDetails(bool NewDetails)
{
	Details = NewDetails;
	UpdatedWidget();
}

void UStatBarBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UpdatedWidget();
}

void UStatBarBase::ProcessCurrentValueText()
{
	FString FloatString;

	if (CurrentValue < 1000.f)
	{
		FloatString = FString::SanitizeFloat(CurrentValue);

		if (CurrentValue < 100.f)
		{
			int32 StringLength = FloatString.Len();
			if (StringLength > 4)
			{
				FloatString = FloatString.Left(4);
			}
			else if (StringLength < 4)
			{
				FloatString = FloatString.Append("0", 4 - StringLength);
			}
		}
	}
	else
	{
		float ScaledValue = CurrentValue / 1000.f;
		FloatString = FString::SanitizeFloat(ScaledValue);
		if (ScaledValue < 10.f)
		{
			FloatString = FloatString.Left(3).Append(TEXT("k"));
		}
		else
		{
			FloatString = FloatString.Left(2).Append(TEXT("k"));
		}
	}

	CurrentValueText = FText::FromString(FloatString);
}

void UStatBarBase::UpdatedWidget()
{
	if (!PercentBar_Filled || !PercentBar_Empty)
	{
		return;
	}

	FSlateChildSize EmptySize = FSlateChildSize(ESlateSizeRule::Fill);
	EmptySize.Value = 1.f - CurrentPercentage;

	FSlateChildSize FilledSize = FSlateChildSize(ESlateSizeRule::Fill);
	FilledSize.Value = CurrentPercentage;

	if (UHorizontalBoxSlot* FilledSlot = Cast<UHorizontalBoxSlot>(PercentBar_Filled->Slot))
	{
		FilledSlot->SetSize(FilledSize);
	}

	if (UHorizontalBoxSlot* EmptySlot = Cast<UHorizontalBoxSlot>(PercentBar_Empty->Slot))
	{
		EmptySlot->SetSize(EmptySize);
	}

	Marker_Start->SetBrushColor(MakerColor);
	Marker_End->SetBrushColor(MakerColor);
	PercentBar_Filled->SetBrushColor(BarForegroundColor);
	PercentBar_Empty->SetBrushColor(BarBackgroundColor);

	NameText->SetText(FText::FromString(NameValue));
	if (Details)
	{
		ProcessCurrentValueText();
		ValueText->SetText(CurrentValueText);
		ValueText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ValueText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

#if WITH_EDITOR
void UStatBarBase::OnDesignerChanged(const FDesignerChangedEventArgs& EventArgs)
{
	Super::OnDesignerChanged(EventArgs);
	UpdatedWidget();
}

void UStatBarBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FString PropertyName = ((PropertyChangedEvent.Property != nullptr)
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None).ToString();

	if (PropertyName == TEXT("CurrentPercentage") || PropertyName == TEXT("CurrentValue") || PropertyName ==
		TEXT("NameValue") || PropertyName == TEXT("Details"))
	{
		UpdatedWidget();
	}
}
#endif