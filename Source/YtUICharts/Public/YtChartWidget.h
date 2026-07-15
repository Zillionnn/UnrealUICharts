#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YtChartTypes.h"
#include "YtChartWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class YTUICHARTS_API UYtChartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UYtChartWidget(const FObjectInitializer& OI);

	// ── Config (editable in BP editor or at runtime) ──

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YtChart")
	EYtChartType Type = EYtChartType::Line;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YtChart")
	TArray<FYtChartSeries> Series;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YtChart")
	TArray<FString> Labels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YtChart")
	FYtChartStyle Style;

	// ── Public API ──

	UFUNCTION(BlueprintCallable, Category = "YtChart")
	void SetData(const TArray<float>& Raw, FName Name = "Data");

	UFUNCTION(BlueprintCallable, Category = "YtChart")
	void AddSeries(FName Name, const TArray<float>& Vals, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "YtChart")
	void Clear();

	UFUNCTION(BlueprintCallable, Category = "YtChart")
	void Refresh();

	// ── Overrides ──

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geo, const FSlateRect& Clip, FSlateWindowElementList& Out, int32 Layer, const FWidgetStyle& Style_, bool bParent) const override;
	virtual void NativeTick(const FGeometry& Geo, float dt) override;
	virtual FReply NativeOnMouseMove(const FGeometry& Geo, const FPointerEvent& Ev) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& Ev) override;

protected:
	// ── Render passes ──
	void DrawBg(const FGeometry& G, FSlateWindowElementList& O, int32& L) const;
	void DrawTitle(const FGeometry& G, FSlateWindowElementList& O, int32& L) const;
	void DrawLegend(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D& OutSize) const;
	void DrawTooltip(const FGeometry& G, FSlateWindowElementList& O, int32& L, const FVector2D& Pos, const FString& Text) const;

	// ── Chart-type renderers ──
	void DrawLine(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect Area) const;
	void DrawBar(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect Area) const;
	void DrawPie(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D Center, float Radius) const;
	void DrawRadar(const FGeometry& G, FSlateWindowElementList& O, int32& L, FVector2D Center, float Radius) const;
	void DrawScatter(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect Area) const;
	void DrawGrid(const FGeometry& G, FSlateWindowElementList& O, int32& L, FSlateRect Area, float Min, float Max) const;

	// ── Helpers ──
	FSlateFontInfo Font(int32 Size) const;
	FLinearColor SeriesColor(int32 i) const;
	int32 CountVisible() const;
	float GetMax() const;
	float GetMin() const;
	float Anim() const;

	// ── State ──
	mutable float AnimPct = 1.0f;
	mutable float AnimTime = 0.0f;
	mutable bool bAnimating = false;
	FVector2D MousePos;
	bool bHover = false;
};
