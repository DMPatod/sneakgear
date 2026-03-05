#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/RadarTypes.h"
#include "RadarWidget.generated.h"

UCLASS()
class SNEAKGEAR_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	float RadarRadiusPx = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	bool bUseWidgetSizeForRadius = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	float RadarRangeWorld = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	bool bDrawWorldNorth = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	float WorldNorthYawDeg = 0.f;

	UFUNCTION(BlueprintCallable)
	void SetContacts(const TArray<FRadarContact>& NewContacts)
	{
		Contacts = NewContacts;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FRadarContact> Contacts;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	                          const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
