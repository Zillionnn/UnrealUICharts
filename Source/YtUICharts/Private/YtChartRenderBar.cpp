#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

void UYtChartWidget::DrawBar(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect A) const
{
	int32 V = CountVisible(); if (V == 0) return;
	float Max = GetMax(); if (Max < 0.001f) Max = 1.0f;
	Max *= 1.1f;
	float AP = Anim();

	int32 MaxN = 0;
	for (int32 si = 0; si < Series.Num(); ++si) if (Series[si].bVisible) MaxN = FMath::Max(MaxN, Series[si].Values.Num());
	if (MaxN == 0) return;

	float GW = A.GetSize().X / (float)MaxN * 0.75f;
	float BW = GW / (float)V;
	int32 vi = 0;

	for (int32 si = 0; si < Series.Num(); ++si)
	{
		auto& S = Series[si]; if (!S.bVisible) continue;
		FLinearColor C = SeriesColor(si);

		for (int32 i = 0; i < S.Values.Num(); ++i)
		{
			float XF = (float)i / (float)MaxN;
			float CX = A.Left + (XF + 0.5f / (float)MaxN) * A.GetSize().X;
			float BH = (S.Values[i] / Max) * A.GetSize().Y * AP;

			if (Style.BarStyle == EYtBarLayout::Group)
			{
				float BX = CX - GW * 0.5f + vi * BW + 1.0f;
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(BW - 2.0f, BH), FSlateLayoutTransform(FVector2D(BX, A.Bottom - BH))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);
			}
			else
			{
				float Stack = 0, Prev = 0, Total = 0;
				for (int32 sj = 0; sj < Series.Num(); ++sj)
					if (Series[sj].bVisible && Series[sj].Values.IsValidIndex(i))
					{
						Total += Series[sj].Values[i];
						if (sj <= si) Stack += Series[sj].Values[i];
						if (sj < si) Prev += Series[sj].Values[i];
					}
				if (Total < 0.001f) Total = 1.0f;
				float SF = Stack / Total, PF = Prev / Total;
				float SH = SF * A.GetSize().Y * AP, PH = PF * A.GetSize().Y * AP;
				float BX = CX - GW * 0.5f + 1.0f;
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(GW - 2.0f, SH - PH), FSlateLayoutTransform(FVector2D(BX, A.Bottom - SH))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);
			}
		}
		vi++;
	}
}
