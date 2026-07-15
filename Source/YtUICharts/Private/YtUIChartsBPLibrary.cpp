#include "YtUIChartsBPLibrary.h"

UYtUIChartsBPLibrary::UYtUIChartsBPLibrary(const FObjectInitializer& OI) : Super(OI) {}

float UYtUIChartsBPLibrary::Max(const TArray<FYtChartSeries>& Series)
{
	float M = 0; bool Set = false;
	for (auto& S : Series) for (float V : S.Values) { if (!Set) { M = V; Set = true; } else M = FMath::Max(M, V); }
	return M;
}

float UYtUIChartsBPLibrary::Min(const TArray<FYtChartSeries>& Series)
{
	float M = 0; bool Set = false;
	for (auto& S : Series) for (float V : S.Values) { if (!Set) { M = V; Set = true; } else M = FMath::Min(M, V); }
	return M;
}

TArray<float> UYtUIChartsBPLibrary::Normalize(const TArray<float>& Data, float ToMin, float ToMax)
{
	TArray<float> R; if (Data.Num() == 0) return R;
	float Lo = Data[0], Hi = Data[0];
	for (float V : Data) { Lo = FMath::Min(Lo, V); Hi = FMath::Max(Hi, V); }
	float Ra = Hi - Lo; if (Ra < 0.0001f) Ra = 1.0f;
	for (float V : Data) R.Add(ToMin + (V - Lo) / Ra * (ToMax - ToMin));
	return R;
}

TArray<float> UYtUIChartsBPLibrary::Smooth(const TArray<float>& Data, int32 W)
{
	TArray<float> R; if (Data.Num() == 0 || W < 1) return R;
	int32 H = W / 2;
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		float S = 0; int32 C = 0;
		for (int32 j = -H; j <= H; ++j)
			if (i + j >= 0 && i + j < Data.Num()) { S += Data[i + j]; C++; }
		R.Add(C > 0 ? S / (float)C : 0.0f);
	}
	return R;
}
