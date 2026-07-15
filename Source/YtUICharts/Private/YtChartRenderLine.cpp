#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

void UYtChartWidget::DrawLine(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect A) const
{
	float Max = GetMax(), Min = GetMin();
	float Range = Max - Min; if (Range < 0.001f) Range = 1.0f;
	Min -= Range * 0.08f; Max += Range * 0.08f; Range = Max - Min;
	float AP = Anim();

	for (int32 si = 0; si < Series.Num(); ++si)
	{
		if (!Series[si].bVisible || Series[si].Values.Num() == 0) continue;
		int32 N = Series[si].Values.Num();
		FLinearColor C = SeriesColor(si);

		TArray<FVector2D> Pts;
		Pts.Reserve(N);
		for (int32 i = 0; i < N; ++i)
		{
			float XF = N > 1 ? (float)i / (float)(N - 1) : 0.5f;
			float YF = (Series[si].Values[i] - Min) / Range * AP;
			Pts.Emplace(A.Left + XF * A.GetSize().X, A.Bottom - YF * A.GetSize().Y);
		}

		TArray<FVector2D> Fill;
		Fill.Reserve(N + 2);
		Fill.Emplace(A.Left, A.Bottom);
		for (int32 i = 0; i < Pts.Num(); ++i) Fill.Add(Pts[i]);
		Fill.Emplace(A.Right, A.Bottom);
		FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Fill, ESlateDrawEffect::None, FLinearColor(C.R, C.G, C.B, 0.12f), true, 0.0f);

		if (Style.Curve == EYtLineCurve::Smooth && N >= 2)
		{
			TArray<FVector2D> Smooth;
			Smooth.Add(Pts[0]);
			for (int32 i = 0; i < N - 1; ++i)
			{
				FVector2D P0 = Pts[FMath::Max(0, i - 1)];
				FVector2D P1 = Pts[i];
				FVector2D P2 = Pts[i + 1];
				FVector2D P3 = Pts[FMath::Min(N - 1, i + 2)];
				for (int32 j = 1; j <= 8; ++j)
				{
					float T = (float)j / 8.0f, T2 = T * T, T3 = T2 * T;
					Smooth.Add(0.5f * ((2.0f * P1) + (-P0 + P2) * T + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * T2 + (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * T3));
				}
			}
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Smooth, ESlateDrawEffect::None, C, false, Series[si].Thickness);
		}
		else if (Style.Curve == EYtLineCurve::Step)
		{
			TArray<FVector2D> Step;
			Step.Reserve(N * 2);
			for (int32 i = 0; i < N - 1; ++i) { Step.Add(Pts[i]); Step.Emplace(Pts[i + 1].X, Pts[i].Y); }
			Step.Add(Pts.Last());
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Step, ESlateDrawEffect::None, C, false, Series[si].Thickness);
		}
		else
		{
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Pts, ESlateDrawEffect::None, C, false, Series[si].Thickness);
		}

		if (Series[si].bDots)
			for (int32 pi = 0; pi < Pts.Num(); ++pi)
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(6.0f, 6.0f), FSlateLayoutTransform(FVector2D(Pts[pi].X - 3.0f, Pts[pi].Y - 3.0f))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);
	}
}
