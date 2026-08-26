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

	// DPI-aware pixel grid snapping — prevents sub-pixel jaggies (毛刺)
	float S = G.GetAccumulatedLayoutTransform().GetScale();
	auto Snap = [S](FVector2D& P)
	{
		P.X = FMath::RoundToFloat(P.X * S) / S;
		P.Y = FMath::RoundToFloat(P.Y * S) / S;
	};

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
			FVector2D P(A.Left + XF * A.GetSize().X, A.Bottom - YF * A.GetSize().Y);
			Snap(P);
			Pts.Emplace(P);
		}

		FVector2D BL(A.Left, A.Bottom); Snap(BL);
		FVector2D BR(A.Right, A.Bottom); Snap(BR);

		TArray<FVector2D> Fill;
		Fill.Reserve(N + 2);
		Fill.Emplace(BL);
		for (int32 i = 0; i < Pts.Num(); ++i) Fill.Add(Pts[i]);
		Fill.Emplace(BR);
		FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Fill, ESlateDrawEffect::None, FLinearColor(C.R, C.G, C.B, 0.12f), true, 0.0f);

		// ECharts-style smooth stroke:
		// 1) miter ribbon polygon fill
		// 2) filled circle at every vertex -> round joins + round caps
		// MakeLines(Thickness=0, bClosed=true) applies shader AA at polygon edges.
		float HW = FMath::Max(Series[si].Thickness * 0.5f, 0.5f);
		bool bRound = (Style.Curve != EYtLineCurve::Smooth); // dense smooth curve needs no round joins

		auto Stroke = [&](const TArray<FVector2D>& LinePts)
		{
			int32 M = LinePts.Num();
			if (M < 2) return;

			auto Perp = [](const FVector2D& D) { return FVector2D(-D.Y, D.X); };
			auto Dir = [&](int32 i)
			{
				FVector2D D = LinePts[FMath::Min(i + 1, M - 1)] - LinePts[i];
				float Len = D.Size();
				return Len > 0.001f ? D / Len : FVector2D(1.0f, 0.0f);
			};
			auto Miter = [&](int32 i)
			{
				FVector2D N1 = Perp(Dir(i - 1));
				FVector2D N2 = Perp(Dir(i));
				FVector2D B = N1 + N2;
				float BLen = B.Size();
				if (BLen < 0.001f) { B = N1; BLen = 1.0f; }
				B /= BLen;
				float D = FMath::Max(N1.Dot(B), 0.25f);
				return LinePts[i] + B * (HW / D);
			};

			TArray<FVector2D> Ribbon;
			Ribbon.Reserve(M * 2 + 2);
			Ribbon.Emplace(LinePts[0] + Perp(Dir(0)) * HW);
			for (int32 i = 1; i < M - 1; ++i) Ribbon.Add(Miter(i));
			Ribbon.Emplace(LinePts[M - 1] + Perp(Dir(M - 2)) * HW);
			Ribbon.Emplace(LinePts[M - 1] - Perp(Dir(M - 2)) * HW);
			for (int32 i = M - 2; i > 0; --i) Ribbon.Add(Miter(i));
			Ribbon.Emplace(LinePts[0] - Perp(Dir(0)) * HW);
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Ribbon, ESlateDrawEffect::None, C, true, 0.0f);

			if (bRound)
			{
				const int32 Segs = 10;
				TArray<FVector2D> Circ;
				Circ.Reserve(Segs + 1);
				for (int32 pi = 0; pi < M; ++pi)
				{
					Circ.Reset();
					for (int32 s = 0; s <= Segs; ++s)
					{
						float A = 2.0f * PI * (float)s / (float)Segs;
						Circ.Emplace(LinePts[pi].X + FMath::Cos(A) * HW, LinePts[pi].Y + FMath::Sin(A) * HW);
					}
					FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Circ, ESlateDrawEffect::None, C, true, 0.0f);
				}
			}
		};

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
			Stroke(Smooth);
		}
		else if (Style.Curve == EYtLineCurve::Step)
		{
			TArray<FVector2D> Step;
			Step.Reserve(N * 2);
			for (int32 i = 0; i < N - 1; ++i) { Step.Add(Pts[i]); Step.Emplace(Pts[i + 1].X, Pts[i].Y); }
			Step.Add(Pts.Last());
			Stroke(Step);
		}
		else
		{
			Stroke(Pts);
		}

		if (Series[si].bDots)
			for (int32 pi = 0; pi < Pts.Num(); ++pi)
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(6.0f, 6.0f), FSlateLayoutTransform(FVector2D(Pts[pi].X - 3.0f, Pts[pi].Y - 3.0f))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);
	}
}
