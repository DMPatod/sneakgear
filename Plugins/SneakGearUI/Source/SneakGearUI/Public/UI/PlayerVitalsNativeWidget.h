#pragma once

#include "CoreMinimal.h"
#include "UI/PlayerVitalsWidget.h"
#include "PlayerVitalsNativeWidget.generated.h"

class UBorder;
class UVerticalBox;

UCLASS()
class SNEAKGEARUI_API UPlayerVitalsNativeWidget : public UPlayerVitalsWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FSlateColor LabelColor = FLinearColor(0.93f, 0.95f, 0.98f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FSlateColor PanelColor = FLinearColor(0.05f, 0.07f, 0.1f, 0.78f);

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FLinearColor HealthFillColor = FLinearColor(0.82f, 0.2f, 0.2f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FLinearColor StaminaFillColor = FLinearColor(0.24f, 0.72f, 0.36f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FMargin PanelPadding = FMargin(14.f, 12.f);

	UPROPERTY(EditDefaultsOnly, Category="Vitals|Style")
	FVector2D BarSize = FVector2D(220.f, 14.f);

private:
	void BuildWidgetTree();

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootBorder;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> VitalsBox;
};
