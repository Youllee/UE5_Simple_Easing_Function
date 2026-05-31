#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "EasingFunctionType.h"
#include "EasingFunctionLibrary.generated.h"

UCLASS()
class EASINGFUNCTION_API UEasingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

protected:
	static float easeBase(float percent, float exponential);

	static float easeIn(float percent, float func(float));
	static float easeOut(float percent, float func(float));
	static float easeInOut(float percent, float func(float));
	static float easeOutIn(float percent, float func(float));

/*	static float easeLinear(float percent);*/
	static float easeSine(float percent);
	static float easeCircle(float percent);
	static float easeElastic(float percent);
	static float easeBack(float percent);
	static float easeBounce(float percent);
	// StepCount == exponential, 딱 맞아떨어지는 커브를 원한다면, 소수점을 반올림하여 사용할 것.
	static float easeStep(float percent, float StepCount);

protected:
/*	static float ApplyEaseIn(float percent, float exponential);*/
	static float ApplyEaseOut(float percent, float exponential);
	static float ApplyEaseInOut(float percent, float exponential);
	static float ApplyEaseOutIn(float percent, float exponential);

public:
	static float EasingFunction(float percent, EEasingType easingType = EEasingType::Linear, float exponential = 2.0f);

	UFUNCTION(BlueprintPure, Category = "Easing Function Library", meta = (DisplayName = "Easing Function"))
	static float K2_EasingFunction(float percent, EEasingType easingType = EEasingType::Linear, float exponential = 2.0f);

};
