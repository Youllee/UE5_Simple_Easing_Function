#include "EasingFunctionLibrary.h"
#include "EasingFunction.h"

UEasingFunctionLibrary::UEasingFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

float UEasingFunctionLibrary::easeBase(float percent, float exponential)
{
	return (exponential != 1.0f && exponential != 0.0f) ? FMath::Pow(percent, exponential) : percent;
}

float UEasingFunctionLibrary::easeIn(float percent, float func(float))
{
	return func(percent);
}

float UEasingFunctionLibrary::easeOut(float percent, float func(float))
{
	return 1.0f - func(1.0f - percent);
}

float UEasingFunctionLibrary::easeInOut(float percent, float func(float))
{
	if (percent < 0.5f)
	{
		return func(percent * 2.0f) * 0.5f;
	}
	else
	{
		return 1.0f - (func(2.0f - (2.0f * percent)) * 0.5f);
	}
}

float UEasingFunctionLibrary::easeOutIn(float percent, float func(float))
{
	if (percent < 0.5f)
	{
		return (1.0f - func(1.0f - (2.0f * percent))) * 0.5f;
	}
	else
	{
		return (0.5f * func((percent * 2.0f) - 1.0f)) + 0.5f;
	}
}
/*
float UEasingFunctionLibrary::easeLinear(float percent)
{
	return percent;
}
*/
float UEasingFunctionLibrary::easeSine(float percent)
{
	return 1.0f - FMath::Cos(percent * PI * 0.5f);
}

float UEasingFunctionLibrary::easeCircle(float percent)
{
	return 1.0f - FMath::Sqrt(1.f - (percent * percent));
}

float UEasingFunctionLibrary::easeElastic(float percent)
{
	if (percent == 0.0f || percent == 1.0f)
	{
		return percent;
	}
	else
	{
		return FMath::Pow(2.0f, (10.f * percent) - 10.f) * FMath::Sin(((percent * 10.f) - 10.75f) * ((2.0f * PI) / 3.0f)) * -1.0f;
	}
}

float UEasingFunctionLibrary::easeBack(float percent)
{
	return percent * percent * ((3.0f * percent) - 2.0f);
}

float UEasingFunctionLibrary::easeBounce(float percent)
{
	float bounce = 4.0f;
	float pow2 = 16.0f;

	while (percent < (pow2 - 1.0f) / 11.0f)
	{
		pow2 = FMath::Pow(2.0f, --bounce);
	}

	return (1.0f / FMath::Pow(4.0f, (3.0f - bounce))) - (7.5625f * FMath::Pow(((((pow2 * 3.0f) - 2.0f) / 22.0f)) - percent, 2.0f));
}

float UEasingFunctionLibrary::easeStep(float percent, float StepCount)
{
	const float safeStepCount = FMath::Max(1.0f, StepCount);
	return (FMath::FloorToFloat(percent * safeStepCount) / safeStepCount);
}
/*
float UEasingFunctionLibrary::ApplyEaseIn(float percent, float exponential)
{
//	return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeLinear);

	return easeBase(percent, exponential);
}
*/
float UEasingFunctionLibrary::ApplyEaseOut(float percent, float exponential)
{
//	return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeLinear);
	return 1.0f - easeBase(1.0f - percent, exponential);
}

float UEasingFunctionLibrary::ApplyEaseInOut(float percent, float exponential)
{
//	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeLinear);

	if (percent < 0.5f)
	{
		return easeBase(percent * 2.0f, exponential) * 0.5f;
	}

	return 1.0f - (easeBase(2.0f - (2.0f * percent), exponential) * 0.5f);
}

float UEasingFunctionLibrary::ApplyEaseOutIn(float percent, float exponential)
{
//	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeLinear);

	if (percent < 0.5f)
	{
		return (1.0f - easeBase(1.0f - (2.0f * percent), exponential)) * 0.5f;
	}

	return (easeBase((percent * 2.0f) - 1.0f, exponential) * 0.5f) + 0.5f;
}

float UEasingFunctionLibrary::EasingFunction(float percent, EEasingType easingType, float exponential)
{
	// 이 함수는 매 틱마다 호출되기 때문에, 방어코드를 의도적으로 생략한다.
	// float safePercent = FMath::Clamp(percent, 0.0f, 1.0f);
	// float safeExponential = FMath::Max(exponential, 0.0f);

	switch (easingType)
	{
	case EEasingType::Linear:		return percent;
	case EEasingType::Step:			return easeStep(percent, exponential);

	case EEasingType::EaseIn:		return easeBase(percent, exponential);
	case EEasingType::EaseOut:		return ApplyEaseOut(percent, exponential);
	case EEasingType::EaseInOut:	return ApplyEaseInOut(percent, exponential);
	case EEasingType::EaseOutIn:	return ApplyEaseOutIn(percent, exponential);

	case EEasingType::SineIn:		return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeSine);
	case EEasingType::SineOut:		return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeSine);
	case EEasingType::SineInOut:	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeSine);
	case EEasingType::SineOutIn:	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeSine);

	case EEasingType::CircleIn:		return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeCircle);
	case EEasingType::CircleOut:	return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeCircle);
	case EEasingType::CircleInOut:	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeCircle);
	case EEasingType::CircleOutIn:	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeCircle);

	case EEasingType::ElasticIn:	return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeElastic);
	case EEasingType::ElasticOut:	return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeElastic);
	case EEasingType::ElasticInOut:	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeElastic);
	case EEasingType::ElasticOutIn:	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeElastic);

	case EEasingType::BackIn:		return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBack);
	case EEasingType::BackOut:		return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBack);
	case EEasingType::BackInOut:	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBack);
	case EEasingType::BackOutIn:	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBack);

	case EEasingType::BounceIn:		return easeIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBounce);
	case EEasingType::BounceOut:	return easeOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBounce);
	case EEasingType::BounceInOut:	return easeInOut(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBounce);
	case EEasingType::BounceOutIn:	return easeOutIn(easeBase(percent, exponential), &UEasingFunctionLibrary::easeBounce);
	}

	return percent;
}

float UEasingFunctionLibrary::K2_EasingFunction(float percent, EEasingType easingType, float exponential)
{
	// 블루프린트에서 직접 호출 시에만, 방어코드 적용
	float safePercent = FMath::Clamp(percent, 0.0f, 1.0f);
	float safeExponential = FMath::Max(exponential, 0.0f);

	return EasingFunction(safePercent, easingType, safeExponential);
}
