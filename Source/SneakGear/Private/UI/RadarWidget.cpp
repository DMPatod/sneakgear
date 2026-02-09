#include "UI/RadarWidget.h"

#include "Components/Widget.h"

void URadarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetClipping(EWidgetClipping::ClipToBounds);
}

int32 URadarWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
                                int32 LayerId,
                                const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto Size = AllottedGeometry.GetLocalSize();
	auto Center = Size * 0.5f;
	const float RadiusFromSize = 0.5f * FMath::Min(Size.X, Size.Y);
	const float EffectiveRadius = (bUseWidgetSizeForRadius && RadiusFromSize > 1.f) ? RadiusFromSize : RadarRadiusPx;

	auto Segments = 48;
	TArray<FVector2D> CirclePts;
	CirclePts.Reserve(Segments + 1);
	for (auto i = 0; i <= Segments; i++)
	{
		auto T = 2.f * PI * float(i) / float(Segments);
		CirclePts.Add(Center + FVector2D(FMath::Cos(T), FMath::Sin(T)) * EffectiveRadius);
	}
	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), CirclePts,
	                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, 0.8));
	LayerId++;

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
				auto T = 2.f * PI * float(i) / float(Segments);
				Ring.Add(P + FVector2D(FMath::Cos(T) * RadiusPx, FMath::Sin(T) * RadiusPx));
			}
			FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), Ring,
			                             ESlateDrawEffect::None, FLinearColor(1, 1, 1, Alpha), true, Thickness);
		};

		auto A = FMath::Clamp(C.Awareness, 0.f, 1.f);
		auto BaseAlpha = 0.15f + 0.55f * A;
		auto LOSBoost = C.bHasLOS ? 0.25f : 0.f;

		DrawRing(HearingPX, FMath::Clamp(BaseAlpha * 0.7f, 0.f, 1.f), 1.f);
		DrawRing(VisionPx, FMath::Clamp(BaseAlpha + LOSBoost, 0.f, 1.f), 1.5);

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
