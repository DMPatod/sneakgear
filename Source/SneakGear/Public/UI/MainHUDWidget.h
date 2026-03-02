#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UStanceWidget;
class URadarWidget;
class UPlayerVitalsWidget;
class UWeaponStatusWidget;
class UEventFeedWidget;
class UStealthPlayerDebugWidget;

UCLASS()
class SNEAKGEAR_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

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
	
	UPROPERTY(EditAnywhere, Category="UI")
	int32 StanceZOrder = 4;

	UPROPERTY(EditAnywhere, Category="UI")
	int32 StealthDebugZOrder = 5;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<URadarWidget> RadarWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UPlayerVitalsWidget> PlayerVitalsWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UWeaponStatusWidget> WeaponStatusWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UEventFeedWidget> EventFeedWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UStanceWidget> StanceWidget;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UStealthPlayerDebugWidget> StealthDebugWidget;

	UPROPERTY(Transient)
	TObjectPtr<UStealthPlayerDebugWidget> RuntimeStealthDebugWidget;
};
