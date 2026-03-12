#include "UI/RadarWidget.h"

#include "Components/Widget.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

void URadarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetClipping(EWidgetClipping::ClipToBounds);
}

void URadarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bDrawWalls)
	{
		WallSamples.Reset();
		return;
	}

	WallRefreshTimer += InDeltaTime;
	if (WallRefreshTimer < FMath::Max(WallRefreshInterval, 0.01f))
	{
		return;
	}

	WallRefreshTimer = 0.f;

	const FVector2D Size = MyGeometry.GetLocalSize();
	const float RadiusFromSize = 0.5f * FMath::Min(Size.X, Size.Y);
	const float EffectiveRadius = (bUseWidgetSizeForRadius && RadiusFromSize > 1.f) ? RadiusFromSize : RadarRadiusPx;
	UpdateWallSamples(EffectiveRadius);
}

int32 URadarWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                int32 LayerId,
                                const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto Size = AllottedGeometry.GetLocalSize();
	auto Center = Size * 0.5f;
	const float RadiusFromSize = 0.5f * FMath::Min(Size.X, Size.Y);
	const float EffectiveRadius = bUseWidgetSizeForRadius && RadiusFromSize > 1.f ? RadiusFromSize : RadarRadiusPx;

	auto Segments = 48;
	TArray<FVector2D> CirclePts;
	CirclePts.Reserve(Segments + 1);
	for (auto i = 0; i <= Segments; i++)
	{
		auto T = 2.f * PI * static_cast<float>(i) / static_cast<float>(Segments);
		CirclePts.Add(Center + FVector2D(FMath::Cos(T), FMath::Sin(T)) * EffectiveRadius);
	}
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CirclePts,
	                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, 0.8));
	LayerId++;

	if (bDrawWalls && WallSamples.Num() > 1)
	{
		for (int32 i = 0; i < WallSamples.Num(); ++i)
		{
			const int32 Next = (i + 1) % WallSamples.Num();
			const FWallSample& A = WallSamples[i];
			const FWallSample& B = WallSamples[Next];
			if (!A.bHit || !B.bHit)
			{
				continue;
			}

			TArray<FVector2D> Segment;
			Segment.Reserve(2);
			Segment.Add(Center + A.RadarPos);
			Segment.Add(Center + B.RadarPos);
			FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Segment,
			                             ESlateDrawEffect::None, WallColor, true, WallThickness);
		}

		LayerId++;
	}

	for (auto& C : Contacts)
	{
		auto P = Center + C.RadarPos;

		auto VisionPx = C.VisionRange / RadarRangeWorld * EffectiveRadius;
		auto HearingPX = C.HearingRange / RadarRangeWorld * EffectiveRadius;

		auto DrawRing = [&](float RadiusPx, float Alpha, float Thickness)
		{
			TArray<FVector2D> Ring;
			Ring.Reserve(Segments + 1);
			for (auto i = 0; i <= Segments; i++)
			{
				auto T = 2.f * PI * static_cast<float>(i) / static_cast<float>(Segments);
				Ring.Add(P + FVector2D(FMath::Cos(T) * RadiusPx, FMath::Sin(T) * RadiusPx));
			}
			FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Ring,
			                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, Alpha), true, Thickness);
		};

		auto DrawArc = [&](float RadiusPx, float CenterAngleRad, float HalfArcRad, float Alpha, float Thickness)
		{
			const int32 ArcSegments = FMath::Max(12, Segments / 4);
			TArray<FVector2D> Arc;
			Arc.Reserve(ArcSegments + 1);

			for (int32 i = 0; i <= ArcSegments; ++i)
			{
				const float ArcT = static_cast<float>(i) / static_cast<float>(ArcSegments);
				const float T = CenterAngleRad - HalfArcRad + (2.f * HalfArcRad * ArcT);
				Arc.Add(P + FVector2D(FMath::Cos(T) * RadiusPx, FMath::Sin(T) * RadiusPx));
			}

			FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Arc,
			                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, Alpha), true, Thickness);
		};

		auto A = FMath::Clamp(C.Awareness, 0.f, 1.f);
		auto BaseAlpha = 0.15f + 0.55f * A;
		auto LOSBoost = C.bHasLOS ? 0.25f : 0.f;

		DrawRing(HearingPX, FMath::Clamp(BaseAlpha * 0.7f, 0.f, 1.f), 1.f);
		const float VisionCenterRad = FMath::DegreesToRadians(C.LookYawOnRadarDeg);
		const float VisionHalfArcRad = FMath::DegreesToRadians(FMath::Clamp(VisionArcDegrees, 1.f, 180.f) * 0.5f);
		DrawArc(VisionPx, VisionCenterRad, VisionHalfArcRad, FMath::Clamp(BaseAlpha + LOSBoost, 0.f, 1.f), 1.5f);

		LayerId++;

		auto DotR = 3.5f + 2.f * A;
		TArray<FVector2D> Dot;
		for (auto i = 0; i <= Segments; i++)
		{
			auto T = 2.f * PI * float(i) / float(Segments);
			Dot.Add(P + FVector2D(FMath::Cos(T), FMath::Sin(T)) * DotR);
		}
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Dot,
		                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, 1.f), true, 2.f);
		LayerId++;
	}

	if (bDrawWorldNorth)
	{
		const float NorthAngleRad = FMath::DegreesToRadians(-WorldNorthYawDeg);
		const FVector2D NorthDir(FMath::Cos(NorthAngleRad), FMath::Sin(NorthAngleRad));
		const FVector2D NorthPos = Center + NorthDir * EffectiveRadius;

		TArray<FVector2D> NorthLine;
		NorthLine.Add(Center + NorthDir * (EffectiveRadius - 10.f));
		NorthLine.Add(NorthPos);
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), NorthLine,
		                             ESlateDrawEffect::None, FLinearColor(0.2f, 0.8f, 1.f, 0.9f), true, 2.f);
		LayerId++;
	}

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}

void URadarWidget::UpdateWallSamples(float EffectiveRadius)
{
	UWorld* World = GetWorld();
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!World || !OwnerPawn || RadarRangeWorld <= KINDA_SMALL_NUMBER)
	{
		WallSamples.Reset();
		return;
	}

	const int32 SampleCount = FMath::Clamp(WallSampleCount, 8, 256);
	WallSamples.SetNum(SampleCount);

	const FVector RadarOrigin = OwnerPawn->GetActorLocation() + FVector(0.f, 0.f, WallTraceHeight);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(RadarWallScan), false);
	Params.AddIgnoredActor(OwnerPawn);

	const float RefYawRad = FMath::DegreesToRadians(WorldNorthYawDeg);
	const float Cos = FMath::Cos(-RefYawRad);
	const float Sin = FMath::Sin(-RefYawRad);

	for (int32 i = 0; i < SampleCount; ++i)
	{
		const float T = 2.f * PI * static_cast<float>(i) / static_cast<float>(SampleCount);
		const FVector Dir(FMath::Cos(T), FMath::Sin(T), 0.f);
		const FVector End = RadarOrigin + Dir * RadarRangeWorld;

		FHitResult Hit;
		const bool bHit = World->LineTraceSingleByChannel(Hit, RadarOrigin, End, WallTraceChannel, Params);

		FWallSample& Sample = WallSamples[i];
		Sample.bHit = bHit;
		Sample.RadarPos = FVector2D::ZeroVector;

		if (!bHit)
		{
			continue;
		}

		const FVector Delta = Hit.ImpactPoint - RadarOrigin;
		const FVector2D D2(Delta.X, Delta.Y);
		const FVector2D Rotated(D2.X * Cos - D2.Y * Sin, D2.X * Sin + D2.Y * Cos);

		const float Dist = Rotated.Size();
		const float ClampedDist = FMath::Min(Dist, RadarRangeWorld);
		const FVector2D Dir2D = Dist > 1.f ? (Rotated / Dist) : FVector2D::ZeroVector;
		const float PxDist = ClampedDist / RadarRangeWorld * EffectiveRadius;
		Sample.RadarPos = Dir2D * PxDist;
	}
}
