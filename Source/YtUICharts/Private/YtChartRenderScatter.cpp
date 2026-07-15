#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

void UYtChartWidget::DrawScatter(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect A) const
{	float Max = GetMax(), Min = GetMin();
	float Range = Max - Min; if (Range < 0.001f) Range = 1.0f;
	Min -= Range * 0.08f; Max += Range * 0.08f; Range = Max - Min;

	for (int32 si = 0; si < Series.Num(); ++si)
	{
		if (!Series[si].bVisible || Series[si].Values.Num() == 0) continue;
		FLinearColor C = SeriesColor(si);

		for (int32 i = 0; i < Series[si].Values.Num(); ++i)
		{
			float XF = (Series[si].Values[i] - Min) / Range;
			for (int32 j = i + 1; j < Series[si].Values.Num(); ++j)
			{
				float YF = (Series[si].Values[j] - Min) / Range;
				float PX = A.Left + XF * A.GetSize().X;
				float PY = A.Bottom - YF * A.GetSize().Y;
				FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(6.0f, 6.0f), FSlateLayoutTransform(FVector2D(PX - 3.0f, PY - 3.0f))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);
			}
		}
	}
}
