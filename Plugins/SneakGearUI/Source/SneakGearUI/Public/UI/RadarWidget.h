#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/RadarTypes.h"
#include "RadarWidget.generated.h"

USTRUCT(BlueprintType)
struct FWallSample
{
	GENERATED_BODY()

	FVector2D RadarPos = FVector2D::ZeroVector;
	bool bHit = false;
};

UCLASS()
class SNEAKGEARUI_API URadarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Contacts", meta=(ClampMin="1.0", ClampMax="180.0"))
	float VisionArcDegrees = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls")
	bool bDrawWalls = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls", meta=(ClampMin="8", ClampMax="256"))
	int32 WallSampleCount = 96;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls", meta=(ClampMin="0.01"))
	float WallRefreshInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls")
	float WallTraceHeight = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls")
	TEnumAsByte<ECollisionChannel> WallTraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls")
	FLinearColor WallColor = FLinearColor(0.2f, 0.9f, 1.0f, 0.55f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar|Walls", meta=(ClampMin="0.5"))
	float WallThickness = 1.5f;

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

private:
	void UpdateWallSamples(float EffectiveRadius);

	UPROPERTY(Transient)
	TArray<FWallSample> WallSamples;

	float WallRefreshTimer = 0.f;
};
