#include "YtChartWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Fonts/FontMeasure.h"
#include "Styling/CoreStyle.h"

UYtChartWidget::UYtChartWidget(const FObjectInitializer& OI) : Super(OI)
{
	SetVisibility(ESlateVisibility::Visible);
}

// ═══════════════════════════════════════════════════════════════════
//  Public API
// ═══════════════════════════════════════════════════════════════════

void UYtChartWidget::SetData(const TArray<float>& Raw, FName Name)
{
	Series.Empty();
	FYtChartSeries S;
	S.Name = Name;
	S.Values = Raw;
	S.Color = Style.Palette.Num() > 0 ? Style.Palette[0] : FLinearColor(0.28f, 0.50f, 0.82f);
	Series.Add(S);
	Refresh();
}

void UYtChartWidget::AddSeries(FName Name, const TArray<float>& Vals, FLinearColor Color)
{
	FYtChartSeries S;
	S.Name = Name;
	S.Values = Vals;
	S.Color = Color;
	Series.Add(S);
	Refresh();
}

void UYtChartWidget::Clear()
{
	Series.Empty();
	Labels.Empty();
	Refresh();
}

void UYtChartWidget::Refresh()
{
	if (Style.bAnimate)
	{
		bAnimating = true;
		AnimTime = 0.0f;
		AnimPct = 0.0f;
	}
	Invalidate(EInvalidateWidgetReason::Paint);
}

// ═══════════════════════════════════════════════════════════════════
//  UE Overrides
// ═══════════════════════════════════════════════════════════════════

int32 UYtChartWidget::NativePaint(const FPaintArgs& Args, const FGeometry& Geo, const FSlateRect& Clip, FSlateWindowElementList& Out, int32 Layer, const FWidgetStyle& Style_, bool bParent) const
{
	DrawBg(Geo, Out, Layer);
	DrawTitle(Geo, Out, Layer);

	FVector2D Size = Geo.GetLocalSize();
	FVector2D LegSize;
	DrawLegend(Geo, Out, Layer, LegSize);

	float Top = 0, Bot = Size.Y, Left = 0, Right = Size.X;

	if (Style.bTitle && !Style.Title.IsEmpty())
	{
		TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		Top = FM->Measure(Style.Title, Font(Style.TitleFont)).Y + 8.0f;
	}
	if (Style.bLegend && Style.LegendPos != EYtLegendPos::Hide)
	{
		if (Style.LegendPos == EYtLegendPos::Top)    Top += LegSize.Y;
		if (Style.LegendPos == EYtLegendPos::Bottom) Bot -= LegSize.Y;
		if (Style.LegendPos == EYtLegendPos::Left)   Left += LegSize.X;
		if (Style.LegendPos == EYtLegendPos::Right)  Right -= LegSize.X;
	}

	FSlateRect Area(Left + Style.Pad.Left, Top + Style.Pad.Top, Right - Style.Pad.Right, Bot - Style.Pad.Bottom);
	if (Area.GetSize().X <= 0 || Area.GetSize().Y <= 0) return Layer + 1;

	float ALeft = Area.Left, AWidth = Area.GetSize().X, AHeight = Area.GetSize().Y;
	FVector2D ChartC((Area.Left + Area.Right) * 0.5f, (Area.Top + Area.Bottom) * 0.5f);

	switch (Type)
	{
	case EYtChartType::Line:
		DrawGrid(Geo, Out, Layer, Area, GetMin(), GetMax());
		DrawLine(Geo, Out, Layer, Area);
		break;
	case EYtChartType::Bar:
		DrawGrid(Geo, Out, Layer, Area, 0, GetMax());
		DrawBar(Geo, Out, Layer, Area);
		break;
	case EYtChartType::Pie:
	case EYtChartType::Doughnut:
		DrawPie(Geo, Out, Layer, ChartC, FMath::Min(AWidth, AHeight) * 0.45f);
		break;
	case EYtChartType::Radar:
		DrawRadar(Geo, Out, Layer, ChartC, FMath::Min(AWidth, AHeight) * 0.4f);
		break;
	case EYtChartType::Scatter:
		DrawGrid(Geo, Out, Layer, Area, GetMin(), GetMax());
		DrawScatter(Geo, Out, Layer, Area);
		break;
	}

	if (bHover && Style.bTooltip)
	{
		int32 N = Labels.Num();
		int32 Idx = -1;
		if (Type == EYtChartType::Line || Type == EYtChartType::Bar || Type == EYtChartType::Scatter)
		{
			if (N > 1)
				Idx = FMath::Clamp(FMath::RoundToInt((MousePos.X - ALeft) / AWidth * (N - 1)), 0, N - 1);
		}
		else if (Type == EYtChartType::Pie || Type == EYtChartType::Doughnut || Type == EYtChartType::Radar)
		{
			if (N > 0)
			{
				float A = FMath::Atan2(MousePos.Y - ChartC.Y, MousePos.X - ChartC.X) + PI / 2.0f;
				if (A < 0) A += 2.0f * PI;
				Idx = FMath::Clamp(FMath::FloorToInt(A / (2.0f * PI) * N), 0, N - 1);
			}
		}
		if (Idx >= 0)
		{
			FString Txt;
			if (Labels.IsValidIndex(Idx)) Txt = Labels[Idx];
			for (int32 si = 0; si < Series.Num(); ++si)
			{
				if (!Series[si].bVisible || !Series[si].Values.IsValidIndex(Idx)) continue;
				Txt += FString::Printf(TEXT("\n%s: %.1f"), *Series[si].Name.ToString(), Series[si].Values[Idx]);
			}
			if (!Txt.IsEmpty())
				DrawTooltip(Geo, Out, Layer, MousePos, Txt);
		}
	}
	return Layer + 1;
}

void UYtChartWidget::NativeTick(const FGeometry& Geo, float dt)
{
	Super::NativeTick(Geo, dt);
	if (bAnimating && AnimPct < 1.0f)
	{
		AnimTime += dt;
		AnimPct = FMath::Min(AnimTime / Style.AnimSec, 1.0f);
		if (AnimPct >= 1.0f) bAnimating = false;
	}
}

FReply UYtChartWidget::NativeOnMouseMove(const FGeometry& Geo, const FPointerEvent& Ev)
{
	MousePos = Geo.AbsoluteToLocal(Ev.GetScreenSpacePosition());
	bHover = true;
	return FReply::Unhandled();
}

void UYtChartWidget::NativeOnMouseLeave(const FPointerEvent& Ev)
{
	bHover = false;
}

// ═══════════════════════════════════════════════════════════════════
//  Helpers
// ═══════════════════════════════════════════════════════════════════

FSlateFontInfo UYtChartWidget::Font(int32 Size) const
{
	return FSlateFontInfo(FCoreStyle::GetDefaultFont(), Size);
}

FLinearColor UYtChartWidget::SeriesColor(int32 i) const
{
	if (Series.IsValidIndex(i) && Series[i].Color != FLinearColor(0.28f, 0.50f, 0.82f))
		return Series[i].Color;
	if (Style.Palette.IsValidIndex(i))
		return Style.Palette[i];
	return FLinearColor(0.28f, 0.50f, 0.82f);
}

int32 UYtChartWidget::CountVisible() const
{
	int32 n = 0;
	for (int32 i = 0; i < Series.Num(); ++i)
		if (Series[i].bVisible) n++;
	return n;
}

float UYtChartWidget::GetMax() const
{
	float M = 0.0f; bool bSet = false;
	for (int32 i = 0; i < Series.Num(); ++i)
	{
		if (!Series[i].bVisible) continue;
		for (int32 j = 0; j < Series[i].Values.Num(); ++j)
			{ float V = Series[i].Values[j]; if (!bSet) { M = V; bSet = true; } else M = FMath::Max(M, V); }
	}
	return M;
}

float UYtChartWidget::GetMin() const
{
	float M = 0.0f; bool bSet = false;
	for (int32 i = 0; i < Series.Num(); ++i)
	{
		if (!Series[i].bVisible) continue;
		for (int32 j = 0; j < Series[i].Values.Num(); ++j)
			{ float V = Series[i].Values[j]; if (!bSet) { M = V; bSet = true; } else M = FMath::Min(M, V); }
	}
	return M;
}

float UYtChartWidget::Anim() const
{
	return 1.0f - FMath::Pow(1.0f - AnimPct, 3.0f);
}

// ═══════════════════════════════════════════════════════════════════
//  Common draw functions
// ═══════════════════════════════════════════════════════════════════

void UYtChartWidget::DrawBg(const FGeometry& G, FSlateWindowElementList& O, int32& L) const
{
	if (Style.Background.A > 0.0f)
		FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, Style.Background);
}

void UYtChartWidget::DrawTitle(const FGeometry& G, FSlateWindowElementList& O, int32& L) const
{
	if (!Style.bTitle || Style.Title.IsEmpty()) return;
	FSlateFontInfo F = Font(Style.TitleFont);
	TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D Sz = FM->Measure(Style.Title, F);
	float X = (G.GetLocalSize().X - Sz.X) * 0.5f;
	FSlateDrawElement::MakeText(O, L++, G.ToPaintGeometry(Sz, FSlateLayoutTransform(FVector2D(X, 2.0f))), Style.Title, F, ESlateDrawEffect::None, Style.TitleColor);
}

void UYtChartWidget::DrawLegend(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D& OutSize) const
{
	OutSize = FVector2D::ZeroVector;
	if (!Style.bLegend || Style.LegendPos == EYtLegendPos::Hide || Series.Num() == 0) return;

	TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FSlateFontInfo F = Font(12);
	float Gap = 4.0f, Pad = 8.0f;
	TArray<FVector2D> Sizes;

	float MaxW = 0, TotH = 0;
	for (int32 i = 0; i < Series.Num(); ++i)
	{
		if (!Series[i].bVisible) continue;
		FVector2D TS = FM->Measure(Series[i].Name.ToString(), F);
		FVector2D IS(14.0f + Gap + TS.X, FMath::Max(12.0f, TS.Y));
		Sizes.Add(IS); MaxW = FMath::Max(MaxW, IS.X); TotH += IS.Y + Pad;
	}

	bool bHoriz = (Style.LegendPos == EYtLegendPos::Top || Style.LegendPos == EYtLegendPos::Bottom);
	float Pw = G.GetLocalSize().X - Pad * 2;

	if (bHoriz)
	{
		float RowX = 0, RowY = 0, LineH = 0;
		for (int32 si = 0; si < Sizes.Num(); ++si)
		{
			if (RowX + Sizes[si].X > Pw && RowX > 0) { RowY += LineH + Pad; RowX = 0; LineH = 0; }
			RowX += Sizes[si].X + Pad; LineH = FMath::Max(LineH, Sizes[si].Y);
		}
		OutSize = FVector2D(Pw, RowY + LineH);
	}
	else OutSize = FVector2D(MaxW + Pad * 2, TotH);

	float SX = Pad, SY = Pad;
	FVector2D PS = G.GetLocalSize();
	if (Style.LegendPos == EYtLegendPos::Top)    { SX = (PS.X - OutSize.X) * 0.5f; SY = Pad; }
	if (Style.LegendPos == EYtLegendPos::Bottom) { SX = (PS.X - OutSize.X) * 0.5f; SY = PS.Y - OutSize.Y - Pad; }
	if (Style.LegendPos == EYtLegendPos::Left)   { SX = Pad; SY = (PS.Y - OutSize.Y) * 0.5f; }
	if (Style.LegendPos == EYtLegendPos::Right)  { SX = PS.X - OutSize.X - Pad; SY = (PS.Y - OutSize.Y) * 0.5f; }

	float DX = SX, DY = SY, MaxLH = 0;
	int32 vi = 0;
	for (int32 i = 0; i < Series.Num(); ++i)
	{
		if (!Series[i].bVisible) continue;
		FVector2D IS = Sizes[vi];
		if (bHoriz && DX + IS.X > Pw && DX > SX) { DY += MaxLH + Pad; DX = SX; MaxLH = 0; }

		FLinearColor C = SeriesColor(i);
		FSlateDrawElement::MakeBox(O, L, G.ToPaintGeometry(FVector2D(12.0f, 12.0f), FSlateLayoutTransform(FVector2D(DX, DY))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, C);

		FVector2D TS = FM->Measure(Series[i].Name.ToString(), F);
		FSlateDrawElement::MakeText(O, L, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(DX + 16.0f, DY))), FText::FromName(Series[i].Name), F, ESlateDrawEffect::None, FLinearColor::White);
		DX += IS.X + Pad; MaxLH = FMath::Max(MaxLH, IS.Y);
		vi++;
	}
	L += Series.Num() * 2;
}

void UYtChartWidget::DrawTooltip(const FGeometry& G, FSlateWindowElementList& O, int32& L, const FVector2D& Pos, const FString& Text) const
{
	if (!Style.bTooltip || Text.IsEmpty()) return;
	FSlateFontInfo F = Font(10);
	TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TS = FM->Measure(Text, F);
	float W = TS.X + 12.0f, H = TS.Y + 8.0f;
	float X = Pos.X + 12.0f, Y = Pos.Y - H - 8.0f;
	FVector2D PS = G.GetLocalSize();
	if (X + W > PS.X) X = PS.X - W - 4.0f;
	if (Y < 0) Y = Pos.Y + 12.0f;
	FSlateDrawElement::MakeBox(O, L++, G.ToPaintGeometry(FVector2D(W, H), FSlateLayoutTransform(FVector2D(X, Y))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, FLinearColor(0.08f, 0.08f, 0.08f, 0.92f));
	FSlateDrawElement::MakeText(O, L++, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(X + 6.0f, Y + 4.0f))), FText::FromString(Text), F, ESlateDrawEffect::None, FLinearColor::White);
}

void UYtChartWidget::DrawGrid(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect A, float Min, float Max) const
{
	float R = Max - Min; if (R < 0.001f) R = 1.0f;
	Min -= R * 0.08f; Max += R * 0.08f; R = Max - Min;

	if (Style.bGrid)
	{
		for (int32 i = 0; i < 5; ++i)
		{
			float Frac = (float)i / 4.0f;
			float Y = A.Bottom - Frac * A.GetSize().Y;
			TArray<FVector2D> Pts = { FVector2D(A.Left, Y), FVector2D(A.Right, Y) };
			FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), Pts, ESlateDrawEffect::None, Style.GridColor, false, 0.5f);
			if (Style.bLabels)
			{
				float V = Min + Frac * R;
				FString S = FString::Printf(TEXT("%.0f"), V);
				FSlateFontInfo F = Font(Style.LabelFont);
				TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
				FVector2D TS = FM->Measure(S, F);
				FSlateDrawElement::MakeText(O, L, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(A.Left - TS.X - 4.0f, Y - TS.Y * 0.5f))), FText::FromString(S), F, ESlateDrawEffect::None, Style.LabelColor);
				L++;
			}
		}
	}

	if (Style.bLabels && Labels.Num() > 0)
	{
		for (int32 i = 0; i < Labels.Num(); ++i)
		{
			float Frac = Labels.Num() > 1 ? (float)i / (float)(Labels.Num() - 1) : 0.5f;
			float X = A.Left + Frac * A.GetSize().X;
			FSlateFontInfo F = Font(Style.LabelFont);
			TSharedRef<FSlateFontMeasure> FM = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
			FVector2D TS = FM->Measure(Labels[i], F);
			FSlateDrawElement::MakeText(O, L++, G.ToPaintGeometry(TS, FSlateLayoutTransform(FVector2D(X - TS.X * 0.5f, A.Bottom + 4.0f))), FText::FromString(Labels[i]), F, ESlateDrawEffect::None, Style.LabelColor);
		}
	}

	TArray<FVector2D> AxisX = { FVector2D(A.Left, A.Bottom), FVector2D(A.Right, A.Bottom) };
	FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), AxisX, ESlateDrawEffect::None, Style.AxisColor, false, 1.0f);
	TArray<FVector2D> AxisY = { FVector2D(A.Left, A.Top), FVector2D(A.Left, A.Bottom) };
	FSlateDrawElement::MakeLines(O, L++, G.ToPaintGeometry(), AxisY, ESlateDrawEffect::None, Style.AxisColor, false, 1.0f);
}
