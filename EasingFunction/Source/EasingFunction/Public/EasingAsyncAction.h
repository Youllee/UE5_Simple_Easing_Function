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
	// v1.1 : Loop를 1회 수행하는데 걸리는 시간입니다.
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	float LoopDuration = 1.0f;
	// v1.1 : 진행할 Loop 횟수입니다. Loop가 끝날때마다 Complete를 호출합니다. 0보다 작으면 무한히 루프합니다.
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	int32 LoopCount = 1;
	// v1.1 : 왕복 액션을 수행햅니다. true이면 Begin -> End -> Begin을 하나의 Loop로 취급합니다.
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	bool bRoundTrip = false;
	// v1.1 : 초당 최대 Update 호출 횟수입니다. 0이면 Update를 호출하지 않습니다. 0보다 작으면 제한없이 Update를 호출합니다.
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library")
	int32 MaxUpdateRate = 60;
	UPROPERTY(BlueprintReadWrite, Category = "Easing Function Library", meta = (DisplayName = "Tickable when Paused"))
	bool bTickableWhenPaused = false;

protected:
	bool bIsRunning = false;
	bool bTickable = false;
	// v1.1 : 현재 Loop 안에서 흐른 시간입니다. Loop가 시작될 때마다 0으로 초기화됩니다.
	float ElapsedTime = 0.0f;
	// v1.1 : MaxUpdateRate 제한을 위한 별도 누적 시간입니다.
	float UpdateElapsedTime = 0.0f;
	// v1.1 : MaxUpdateRate 제한에 따른, Update 호출 주기입니다.
	float UpdateInterval = 0.0f;
	// v1.1 : 현재까지 Loop가 완료된 횟수입니다.
	int32 CurrentLoopCount = 0;

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
	// v1.0 : 비동기 액션 종료 로직입니다. 다만, 비동기 액션을 '취소'가 아닌 '완료'로 처리 합니다.
	UFUNCTION(BlueprintCallable, Category = "Async Action")
	virtual void Complete();

	virtual UWorld* GetWorld() const override
	{
		return WorldContextObject.IsValid() ? WorldContextObject.Get() : nullptr;
	}

protected:
	// v1.0 :  각 타입별 StartEasing_* 함수에서 공통 설정을 초기화하기 위한 함수입니다.
	void InitializeBase(UWorld* InContextWorld, EEasingType InEasingType, float InExponential, float InLoopDuration, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool bInTickableWhenPaused);
	// v1.0 : 액션을 실제로 종료하고 Tick을 멈춘 뒤 GC 대상이 되도록 합니다.
	void Finish();
	// v1.1 : 이번 Tick에서 Update 델리게이트를 호출할지 판단합니다.
	bool ShouldUpdate(float DeltaTime);
	// v1.0 : EasingType과 Exponential을 적용해 보간용 값(0.0 ~ 1.0)을 반환합니다.
	float GetBlendValue(float percent);
	// v1.1 : 현재 Loop의 순수 진행률입니다. (0.0 ~ 1.0)
	float GetLocalPercent();
	// v1.1 : 실제 Begin/End 보간에 사용할 percent를 계산합니다.
	float GetLoopBlendPercent(float percent);

	// v1.0 : Update 델리게이트를 발생시키는 타입별 구현입니다.
	virtual void Blend(float percent, float blendPercent) {};
	// v1.0 : Completed 델리게이트를 발생시키는 타입별 구현입니다.
	virtual void BroadcastCompleted(float percent) {};

public:
	// v1.1 : 현재 Action의 누적 진행률입니다. N회 루프 시 0.0 ~ N.0 사이의 값을 반환합니다.
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
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InLoopDuration = "1.0f", InBegin = "0.0f", InEnd = "1.0f"
		, InLoopCount = "1", bInRoundTrip = "false", InMaxUpdateRate = "60", InTickableWhenPaused = "false", AdvancedDisplay = "InMaxUpdateRate, InTickableWhenPaused"))
	static UAsyncEasingAction_Float* StartEasing_Float(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration
		, float InBegin, float InEnd, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent, float blendPercent) override;
	virtual void BroadcastCompleted(float percent) override;

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
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InLoopDuration = "1.0f"
		, InLoopCount = "1", bInRoundTrip = "false", InMaxUpdateRate = "60", InTickableWhenPaused = "false", AdvancedDisplay = "InMaxUpdateRate, InTickableWhenPaused"))
	static UAsyncEasingAction_Vector2D* StartEasing_Vector2D(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration
		, FVector2D InBegin, FVector2D InEnd, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent, float blendPercent) override;
	virtual void BroadcastCompleted(float percent) override;

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
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InLoopDuration = "1.0f"
		, InLoopCount = "1", bInRoundTrip = "false", InMaxUpdateRate = "60", InTickableWhenPaused = "false", AdvancedDisplay = "InMaxUpdateRate, InTickableWhenPaused"))
	static UAsyncEasingAction_Vector* StartEasing_Vector(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration
		, FVector InBegin, FVector InEnd, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent, float blendPercent) override;
	virtual void BroadcastCompleted(float percent) override;

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
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InLoopDuration = "1.0f"
		, InLoopCount = "1", bInRoundTrip = "false", InMaxUpdateRate = "60", InTickableWhenPaused = "false", AdvancedDisplay = "InMaxUpdateRate, InTickableWhenPaused"))
	static UAsyncEasingAction_Color* StartEasing_Color(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration
		, FLinearColor InBegin, FLinearColor InEnd, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent, float blendPercent) override;
	virtual void BroadcastCompleted(float percent) override;

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
		, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", InExponential = "2.0f", InLoopDuration = "1.0f"
		, InLoopCount = "1", bInRoundTrip = "false", InMaxUpdateRate = "60", InTickableWhenPaused = "false", AdvancedDisplay = "InMaxUpdateRate, InTickableWhenPaused"))
	static UAsyncEasingAction_Rotator* StartEasing_Rotator(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration
		, FRotator InBegin, FRotator InEnd, int32 InLoopCount, bool bInRoundTrip, int32 InMaxUpdateRate, bool InTickableWhenPaused);

	virtual void BeginDestroy() override;
	virtual void Cancel() override;
	virtual void Complete() override;

protected:
	virtual void Blend(float percent, float blendPercent) override;
	virtual void BroadcastCompleted(float percent) override;

};
