#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Tickable.h"
#include "Stats/Stats.h"
#include "Engine/CancellableAsyncAction.h"
#include "EasingFunction/Public/EasingFunctionType.h"
#include "EasingAsyncAction.generated.h"

UCLASS(Abstract)
class EASINGFUNCTION_API UAsyncEasingAction : public UCancellableAsyncAction, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	EEasingType EasingType = EEasingType::Linear;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	float Exponential = 2.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	float Duration = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library", meta = (DisplayName = "Tickable when Paused"))
	bool bTickableWhenPaused = false;

protected:
	bool bIsRunning = false;
	bool bTickable = false;
	float ElapsedTime = 0.0f;

#pragma region TickableGameObject
public:
	virtual void Tick(float DeltaTime) override;
	bool IsTickable() const override { return bTickable; }
	bool IsTickableInEditor() const override { return false; }
	bool IsTickableWhenPaused() const override { return bTickableWhenPaused; }
	TStatId GetStatId() const override;
#pragma endregion

public:
	virtual void Activate() override;
	virtual void Cancel() override;
	// Cancel과 같은 비동기 액션 종료 로직입니다. 다만, 비동기 액션을 '완료'로 처리 합니다.
	UFUNCTION(BlueprintCallable, Category = "Async Action")
	virtual void Complete();

	virtual UWorld* GetWorld() const override
	{
		return WorldContextObject.IsValid() ? WorldContextObject.Get() : nullptr;
	}

protected:
	void InitializeBase(UWorld* InContextWorld, EEasingType InEasingType, float InExponential, float InDuration, bool bInTickableWhenPaused);
	float GetBlendValue(float percent);
	virtual void Blend(float percent) {};

public:
	UFUNCTION(BlueprintPure, Category = "Easing Function Library")
	float GetPercent();
	UFUNCTION(BlueprintPure, Category = "Easing Function Library")
	bool IsRunning() { return bIsRunning; };

protected:
	TWeakObjectPtr<UWorld> WorldContextObject = nullptr;

};

UCLASS()
class EASINGFUNCTION_API UAsyncEasingAction_Float : public UAsyncEasingAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Float Update;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Float Completed;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Float Canceled;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	float Begin;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	float End;

public:
	UFUNCTION(BlueprintCallable, Category = "Easing Function Library", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Easing (Float)"
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InDuration = "1.0f", InBegin = "0.0f", InEnd = "1.0f"
		, InTickableWhenPaused = "false", AdvancedDisplay = "InTickableWhenPaused"))
	static UAsyncEasingAction_Float* StartEasing_Float(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration
		, float InBegin, float InEnd, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent) override;

};

UCLASS()
class EASINGFUNCTION_API UAsyncEasingAction_Vector2D : public UAsyncEasingAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector2D Update;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector2D Completed;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector2D Canceled;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FVector2D Begin;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FVector2D End;

public:
	UFUNCTION(BlueprintCallable, Category = "Easing Function Library", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Easing (Vector2D)"
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InDuration = "1.0f"
		, InTickableWhenPaused = "false", AdvancedDisplay = "InTickableWhenPaused"))
	static UAsyncEasingAction_Vector2D* StartEasing_Vector2D(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration
		, FVector2D InBegin, FVector2D InEnd, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent) override;

};

UCLASS()
class EASINGFUNCTION_API UAsyncEasingAction_Vector : public UAsyncEasingAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector Update;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector Completed;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Vector Canceled;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FVector Begin;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FVector End;

public:
	UFUNCTION(BlueprintCallable, Category = "Easing Function Library", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Easing (Vector)"
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InDuration = "1.0f"
		, InTickableWhenPaused = "false", AdvancedDisplay = "InTickableWhenPaused"))
	static UAsyncEasingAction_Vector* StartEasing_Vector(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration
		, FVector InBegin, FVector InEnd, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent) override;

};

UCLASS()
class EASINGFUNCTION_API UAsyncEasingAction_Color : public UAsyncEasingAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Color Update;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Color Completed;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Color Canceled;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FLinearColor Begin;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FLinearColor End;

public:
	UFUNCTION(BlueprintCallable, Category = "Easing Function Library", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Easing (Color)"
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InDuration = "1.0f"
		, InTickableWhenPaused = "false", AdvancedDisplay = "InTickableWhenPaused"))
	static UAsyncEasingAction_Color* StartEasing_Color(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration
		, FLinearColor InBegin, FLinearColor InEnd, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent) override;

};

UCLASS()
class EASINGFUNCTION_API UAsyncEasingAction_Rotator : public UAsyncEasingAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Rotator Update;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Rotator Completed;
	UPROPERTY(BlueprintAssignable)
	FEasingFunctionDelegate_Rotator Canceled;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FRotator Begin;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	FRotator End;

public:
	UFUNCTION(BlueprintCallable, Category = "Easing Function Library", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Start Easing (Rotator)"
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InDuration = "1.0f"
		, InTickableWhenPaused = "false", AdvancedDisplay = "InTickableWhenPaused"))
	static UAsyncEasingAction_Rotator* StartEasing_Rotator(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration
		, FRotator InBegin, FRotator InEnd, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent) override;

};
