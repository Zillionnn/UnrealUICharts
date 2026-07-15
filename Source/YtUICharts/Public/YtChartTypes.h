#pragma once
#include "CoreMinimal.h"
#include "YtChartTypes.generated.h"

UENUM(BlueprintType)
enum class EYtChartType : uint8
{
	Line,
	Bar,
	Pie,
	Doughnut,
	Radar,
	Scatter
};

UENUM(BlueprintType)
enum class EYtLegendPos : uint8
{
	Top,
	Bottom,
	Left,
	Right,
	Hide
};

UENUM(BlueprintType)
enum class EYtBarLayout : uint8
{
	Group,
	Stack,
	Percent
};

UENUM(BlueprintType)
enum class EYtLineCurve : uint8
{
	Straight,
	Smooth,
	Step
};

USTRUCT(BlueprintType)
struct FYtChartSeries
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Values;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor(0.28f, 0.50f, 0.82f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDots = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVisible = true;
};

USTRUCT(BlueprintType)
struct FYtChartStyle
{
	GENERATED_BODY()

	// === Canvas ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Background = FLinearColor(0.04f, 0.04f, 0.04f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMargin Pad = FMargin(12.0f);

	// === Title ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTitle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TitleFont = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TitleColor = FLinearColor::White;

	// === Grid & Axis ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrid = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GridColor = FLinearColor(0.15f, 0.15f, 0.15f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor AxisColor = FLinearColor(0.35f, 0.35f, 0.35f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLabels = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LabelFont = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LabelColor = FLinearColor(0.75f, 0.75f, 0.75f);

	// === Legend ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLegend = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EYtLegendPos LegendPos = EYtLegendPos::Top;

	// === Tooltip ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTooltip = true;

	// === Animation ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAnimate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimSec = 0.6f;

	// === Bar ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EYtBarLayout BarStyle = EYtBarLayout::Group;

	// === Line ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EYtLineCurve Curve = EYtLineCurve::Straight;

	// === Pie ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PieHole = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPct = true;

	// === Palette ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLinearColor> Palette;

	FYtChartStyle()
	{
		Palette = {
			FLinearColor(0.282f, 0.502f, 0.816f),
			FLinearColor(0.859f, 0.322f, 0.318f),
			FLinearColor(0.553f, 0.773f, 0.278f),
			FLinearColor(0.965f, 0.714f, 0.188f),
			FLinearColor(0.584f, 0.376f, 0.737f),
			FLinearColor(0.859f, 0.486f, 0.125f),
			FLinearColor(0.318f, 0.749f, 0.816f),
		};
	}
};
