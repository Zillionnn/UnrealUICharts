#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

void UYtChartWidget::DrawRadar(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D Center, float R) const
{
	int32 N = 0;
	for (int32 si = 0; si < Series.Num(); ++si)
	{
		if (Series[si].bVisible) { N = Series[si].Values.Num(); break; }
	}
	if (N <= 2) return;

	float GlobalMax = 0.0f;
	for (int32 si = 0; si < Series.Num(); ++si)
	{
		if (!Series[si].bVisible) continue;
		for (int32 j = 0; j < Series[si].Values.Num(); ++j)
			GlobalMax = FMath::Max(GlobalMax, Series[si].Values[j]);
	}
	if (GlobalMax < 0.001f) GlobalMax = 1.0f;

	float AP = Anim();
	float GridR = R * (Labels.Num() > 0 ? 1.15f : 1.0f);

	TArray<FVector2D> OuterPts;
	OuterPts.Reserve(N);
	for (int32 i = 0; i < N; ++i)
	{
		float A = -PI / 2.0f + (float)i / (float)N * 2.0f * PI;
		OuterPts.Emplace(Center.X + FMath::Cos(A) * GridR, Center.Y + FMath::Sin(A) * GridR);
	}

	// 1. 最外层背景闭合：先复制到局部变量以通过 CheckAddress 验证
	TArray<FVector2D> ClosedOuterPts = OuterPts;
	if (N > 0)
	{
		FVector2D FirstPt = OuterPts[0];
		ClosedOuterPts.Add(FirstPt);
	}
	FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), ClosedOuterPts, ESlateDrawEffect::None, Style.GridColor, true, 0.5f);

	// 2. 内圈网格闭合：先复制到局部变量以通过 CheckAddress 验证
	if (N >= 3)
	{
		for (int32 r = 1; r <= 4; ++r)
		{
			float Frac = (float)r / 5.0f;
			TArray<FVector2D> Ring;
			Ring.Reserve(N + 1);
			for (int32 i = 0; i < N; ++i)
			{
				float A = -PI / 2.0f + (float)i / (float)N * 2.0f * PI;
				Ring.Emplace(Center.X + FMath::Cos(A) * GridR * Frac, Center.Y + FMath::Sin(A) * GridR * Frac);
			}
			if (N > 0)
			{
				FVector2D FirstPt = Ring[0]; // 显式复制到局部变量，避免触发 CheckAddress
				Ring.Add(FirstPt);
			}
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Ring, ESlateDrawEffect::None, Style.GridColor, true, 0.5f);
		}
	}

	for (int32 i = 0; i < N; ++i)
	{
		TArray<FVector2D> Ln = { Center, OuterPts[i] };
		FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Ln, ESlateDrawEffect::None, Style.GridColor, false, 0.5f);
	}

	if (Style.bLabels && Labels.Num() >= N)
	{
		TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		for (int32 i = 0; i < N; ++i)
		{
			float A = -PI / 2.0f + (float)i / (float)N * 2.0f * PI;
			FVector2D LP(Center.X + FMath::Cos(A) * (GridR + 14.0f), Center.Y + FMath::Sin(A) * (GridR + 14.0f));
			FVector2D TS = FM->Measure(Labels[i], Font(Style.LabelFont));
			FSlateDrawElement::MakeText(O, L++, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(LP.X - TS.X * 0.5f, LP.Y - TS.Y * 0.5f))), FText::FromString(Labels[i]), Font(Style.LabelFont), ESlateDrawEffect::None, FLinearColor::White);
		}
	}

	for (int32 si = 0; si < Series.Num(); ++si)
	{
		auto& S = Series[si]; if (!S.bVisible || S.Values.Num() < N) continue;
		FLinearColor Col = SeriesColor(si);
		TArray<FVector2D> Pts;
		Pts.Reserve(N);

		for (int32 i = 0; i < N; ++i)
		{
			float Norm = FMath::Clamp(S.Values[i] / GlobalMax, 0.0f, 1.0f) * AP;
			float A = -PI / 2.0f + (float)i / (float)N * 2.0f * PI;
			Pts.Emplace(Center.X + FMath::Cos(A) * GridR * Norm, Center.Y + FMath::Sin(A) * GridR * Norm);
		}

		// 3. 数据系列连线闭合：复制到局部变量以通过 CheckAddress 验证
		TArray<FVector2D> Poly = Pts;
		if (Pts.Num() > 0)
		{
			FVector2D FirstPt = Pts[0];
			Poly.Add(FirstPt);
		}

		FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Poly, ESlateDrawEffect::None, FLinearColor(Col.R, Col.G, Col.B, 0.2f), true, 0.0f);
		FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Poly, ESlateDrawEffect::None, Col, false, Series[si].Thickness);

		if (Series[si].bDots)
			for (int32 i = 0; i < Pts.Num(); ++i)
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(5.0f, 5.0f), FSlateLayoutTransform(FVector2D(Pts[i].X - 2.5f, Pts[i].Y - 2.5f))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, Col);
	}
}