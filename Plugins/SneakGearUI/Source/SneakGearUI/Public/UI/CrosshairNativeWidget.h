#pragma once

#include "CoreMinimal.h"
#include "UI/CrosshairWidget.h"
#include "CrosshairNativeWidget.generated.h"

class UTexture2D;

UCLASS()
class SNEAKGEARUI_API UCrosshairNativeWidget : public UCrosshairWidget
{
	GENERATED_BODY()

public:
	UCrosshairNativeWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style")
	FLinearColor CrosshairColor = FLinearColor(1.f, 1.f, 1.f, 0.95f);

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style")
	FLinearColor HitMarkerColor = FLinearColor(1.f, 0.25f, 0.25f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style", meta=(ClampMin="1.0"))
	float ArmLength = 12.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style", meta=(ClampMin="1.0"))
	float ArmThickness = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style", meta=(ClampMin="0.0"))
	float CenterGap = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Crosshair|Style", meta=(ClampMin="1.0"))
	float HitMarkerSize = 8.f;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> WhiteTexture;

	void BuildWidgetTree();
};
