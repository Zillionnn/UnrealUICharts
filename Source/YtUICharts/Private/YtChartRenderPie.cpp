#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

void UYtChartWidget::DrawPie(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D Center, float R) const
{
	float Total = 0;
	for (int32 si = 0; si < Series.Num(); ++si)
		if (Series[si].bVisible) Total += FMath::Max(0.0f, Series[si].Values.Num() > 0 ? Series[si].Values[0] : 0);
	if (Total < 0.001f) return;

	float AP = Anim();
	float IR = Style.PieHole * R;
	float Angle = -90.0f;
	TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	for (int32 si = 0; si < Series.Num(); ++si)
	{
		if (!Series[si].bVisible) continue;
		float V = Series[si].Values.Num() > 0 ? Series[si].Values[0] : 0;
		float Slice = (V / Total) * 360.0f * AP;
		if (Slice <= 0.0f) { Angle += (V / Total) * 360.0f; continue; }

		FLinearColor Col = SeriesColor(si);
		float End = Angle + Slice;
		int32 Segs = FMath::Max(3, (int32)(Slice / 4.0f));

		TArray<FVector2D> Outer;
		TArray<FVector2D> Inner;
		Outer.Reserve(Segs + 1);
		Inner.Reserve(Segs + 1);
		for (int32 j = 0; j <= Segs; ++j)
		{
			float A = FMath::DegreesToRadians(Angle + (float)j / (float)Segs * Slice);
			Outer.Emplace(Center.X + FMath::Cos(A) * R, Center.Y + FMath::Sin(A) * R);
			Inner.Emplace(Center.X + FMath::Cos(A) * IR, Center.Y + FMath::Sin(A) * IR);
		}

		for (int32 j = 0; j < Segs; ++j)
		{
			if (Style.PieHole > 0.001f)
			{
				TArray<FVector2D> T1 = { Outer[j], Outer[j+1], Inner[j+1] };
				FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), T1, ESlateDrawEffect::None, Col, true, 0.0f);
				TArray<FVector2D> T2 = { Outer[j], Inner[j+1], Inner[j] };
				FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), T2, ESlateDrawEffect::None, Col, true, 0.0f);
			}
			else
			{
				TArray<FVector2D> Tri = { Center, Outer[j], Outer[j+1] };
				FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Tri, ESlateDrawEffect::None, Col, true, 0.0f);
			}
		}

		if (Style.bPct && Slice > 20.0f)
		{
			float MA = FMath::DegreesToRadians((Angle + End) * 0.5f);
			float LR = Style.PieHole > 0.001f ? (R + IR) * 0.5f : R * 0.6f;
			FVector2D LP(Center.X + FMath::Cos(MA) * LR, Center.Y + FMath::Sin(MA) * LR);
			FString Pct = FString::Printf(TEXT("%.0f%%"), (V / Total) * 100.0f);
			FVector2D TS = FM->Measure(Pct, Font(11));
			FSlateDrawElement::MakeText(O, L++, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(LP.X - TS.X * 0.5f, LP.Y - TS.Y * 0.5f))), FText::FromString(Pct), Font(11), ESlateDrawEffect::None, FLinearColor::White);
		}
		Angle = End;
	}
}
