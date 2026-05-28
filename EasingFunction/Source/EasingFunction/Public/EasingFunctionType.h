#pragma once

#include "CoreMinimal.h"
#include "EasingFunctionType.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasingFunctionDelegate_Float, float, value, float, percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasingFunctionDelegate_Vector2D, FVector2D, value, float, percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasingFunctionDelegate_Vector, FVector, value, float, percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasingFunctionDelegate_Color, FLinearColor, value, float, percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEasingFunctionDelegate_Rotator, FRotator, value, float, percent);

UENUM(BlueprintType)
enum class EEasingType : uint8
{
//	None,
	Linear,
	Step,
	EaseIn,
	EaseOut,
	EaseInOut,
	EaseOutIn,
	SineIn,
	SineOut,
	SineInOut,
	SineOutIn,
	CircleIn,
	CircleOut,
	CircleInOut,
	CircleOutIn,
	ElasticIn,
	ElasticOut,
	ElasticInOut,
	ElasticOutIn,
	BackIn,
	BackOut,
	BackInOut,
	BackOutIn,
	BounceIn,
	BounceOut,
	BounceInOut,
	BounceOutIn,
};
