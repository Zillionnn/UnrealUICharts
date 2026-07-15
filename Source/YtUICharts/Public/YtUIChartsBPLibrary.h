#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "YtChartTypes.h"
#include "YtUIChartsBPLibrary.generated.h"

UCLASS()
class YTUICHARTS_API UYtUIChartsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, Category = "YtChart|Util")
	static float Max(const TArray<FYtChartSeries>& Series);

	UFUNCTION(BlueprintPure, Category = "YtChart|Util")
	static float Min(const TArray<FYtChartSeries>& Series);

	UFUNCTION(BlueprintCallable, Category = "YtChart|Util")
	static TArray<float> Normalize(const TArray<float>& Data, float ToMin = 0.0f, float ToMax = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "YtChart|Util")
	static TArray<float> Smooth(const TArray<float>& Data, int32 Window = 3);
};
