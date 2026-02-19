#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class URadarWidget;
class UPlayerVitalsWidget;
class UWeaponStatusWidget;
class UEventFeedWidget;

UCLASS()
class SNEAKGEAR_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	URadarWidget* GetRadarWidget() const
	{
		return RadarWidget;
	}

protected:
	UPROPERTY(EditAnywhere, Category="UI")
	int32 RadarZOrder = 0;

	UPROPERTY(EditAnywhere, Category="UI")
	int32 PlayerVitalsZOrder = 1;

	UPROPERTY(EditAnywhere, Category="UI")
	int32 WeaponStatusZOrder = 2;

	UPROPERTY(EditAnywhere, Category="UI")
	int32 EventFeedZOrder = 3;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<URadarWidget> RadarWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UPlayerVitalsWidget> PlayerVitalsWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWeaponStatusWidget> WeaponStatusWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UEventFeedWidget> EventFeedWidget;
};
