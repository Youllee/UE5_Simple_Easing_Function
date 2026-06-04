#include "EasingAsyncAction.h"
#include "EasingFunctionLibrary.h"
#include "EasingFunction.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UAsyncEasingAction::Tick(float DeltaTime)
{
	if (bIsRunning == false)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	// localPercent : 현재 Loop의 진행률 (0.0 ~ 1.0)
	float localPercent = GetLocalPercent();
	// percent : 현재 Action의 누적 진행률, (LoopCount + LocalPercent)
	float percent = static_cast<float>(CurrentLoopCount) + localPercent;
	if (ShouldUpdate(DeltaTime))
	{
		// blendPercent : 실제 Begin/End 보간에 사용할 진행률 (0.0 ~ 1.0)
		float blendPercent = GetLoopBlendPercent(localPercent);
		Blend(percent, blendPercent);
	}

	// Loop 1회 완료
	if (ElapsedTime >= LoopDuration)
	{
		++CurrentLoopCount;
		// Loop가 완료될 때마다 Complete 이벤트 호출 (누적 완료 횟수 반환)
		BroadcastCompleted(static_cast<float>(CurrentLoopCount));
		// 루프 횟수 체크, LoopCount가 0보다 작으면 무한루프
		if (LoopCount < 0 || CurrentLoopCount < LoopCount)
		{
			ElapsedTime = 0.0f;
			return;
		}

		// 액션 종료
		Finish();
	}
}

TStatId UAsyncEasingAction::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAsyncEasingAction, STATGROUP_Tickables);
}

void UAsyncEasingAction::Activate()
{
	bIsRunning = true;
	bTickable = true;
	ElapsedTime = 0.0f;
	UpdateElapsedTime = 0.0f;
	CurrentLoopCount = 0;

	Super::Activate();
}

void UAsyncEasingAction::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction::Complete()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	BroadcastCompleted(GetPercent());
	Finish();
}

void UAsyncEasingAction::Finish()
{
	bIsRunning = false;
	bTickable = false;
	ElapsedTime = 0.0f;

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction::InitializeBase(UWorld* InContextWorld, EEasingType InEasingType, float InExponential, float InLoopDuration, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool bInTickableWhenPaused)
{
	WorldContextObject = InContextWorld;
	EasingType = InEasingType;
	Exponential = FMath::Max(InExponential, 0.0f);
	LoopDuration = FMath::Max(InLoopDuration, 0.0f);

	LoopCount = InLoopCount;
	MaxUpdateRate = InMaxUpdateRate;
	UpdateInterval = (MaxUpdateRate > 0) ? 1.0f / static_cast<float>(MaxUpdateRate) : 0.0f;
	bRoundTrip = bInRoundTrip;
	bTickableWhenPaused = bInTickableWhenPaused;
}

bool UAsyncEasingAction::ShouldUpdate(float DeltaTime)
{
	// MaxUpdateRate < 0 : Update 호출 제한 없음
	if (MaxUpdateRate < 0)
	{
		return true;
	}

	// MaxUpdateRate == 0 : Update 호출 불가, (Complete/Cancel만 호출합니다.)
	if (MaxUpdateRate == 0)
	{
		return false;
	}

	UpdateElapsedTime += DeltaTime;
	if (UpdateElapsedTime < UpdateInterval)
	{
		return false;
	}

	// UpdateInterval을 초과했다면, Update 이벤트 호출합니다.
	UpdateElapsedTime = FMath::Fmod(UpdateElapsedTime, UpdateInterval);
	return true;
}

float UAsyncEasingAction::GetBlendValue(float percent)
{
	return UEasingFunctionLibrary::EasingFunction(percent, EasingType, Exponential);
}

float UAsyncEasingAction::GetLocalPercent()
{
	// 현재 Loop의 진행률 (0.0 ~ 1.0)
	return (LoopDuration <= 0.0f) ? 1.0f : FMath::Clamp(ElapsedTime / LoopDuration, 0.0f, 1.0f);
}

float UAsyncEasingAction::GetLoopBlendPercent(float percent)
{
	// 단방향 액션(Begin -> End) : LocalPercent = BlendPercent
	if (bRoundTrip == false)
	{
		return percent;
	}

	// 왕복 액션(Begin -> End -> Begin) : 0.0 ~ 0.5 -> 0.0 ~ 1.0, 0.5 ~ 1.0 -> 1.0 ~ 0.0
	return (percent < 0.5f) ? percent * 2.0f : (1.0f - percent) * 2.0f;
}

float UAsyncEasingAction::GetPercent()
{
	// 현재 액션의 누적 진행률, N회 루프 시 0.0 ~ N.0 반환
	return static_cast<float>(CurrentLoopCount) + GetLocalPercent();
}

UAsyncEasingAction_Float* UAsyncEasingAction_Float::StartEasing_Float(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration, float InBegin, float InEnd, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool InTickableWhenPaused)
{
	if (IsValid(GEngine) == false)
	{
		return nullptr;
	}

	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (IsValid(ContextWorld) == false)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = ContextWorld->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		return nullptr;
	}

	UAsyncEasingAction_Float* NewAction = NewObject<UAsyncEasingAction_Float>(GameInstance);
	if (IsValid(NewAction) == false)
	{
		return nullptr;
	}

	NewAction->Begin = InBegin;
	NewAction->End = InEnd;
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InLoopDuration, InLoopCount, InMaxUpdateRate, bInRoundTrip, InTickableWhenPaused);
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UAsyncEasingAction_Float::BeginDestroy()
{
	Update.Clear();
	Completed.Clear();
	Canceled.Clear();

	Super::BeginDestroy();
}

void UAsyncEasingAction_Float::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		float result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(GetLocalPercent())));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Float::Complete()
{
	Super::Complete();
}

void UAsyncEasingAction_Float::Blend(float percent, float blendPercent)
{
	if (Update.IsBound())
	{
		float result = FMath::Lerp(Begin, End, GetBlendValue(blendPercent));
		Update.Broadcast(result, percent);
	}
}

void UAsyncEasingAction_Float::BroadcastCompleted(float percent)
{
	if (Completed.IsBound())
	{
		float result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(1.0f)));
		Completed.Broadcast(result, percent);
	}
}


UAsyncEasingAction_Vector2D* UAsyncEasingAction_Vector2D::StartEasing_Vector2D(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration, FVector2D InBegin, FVector2D InEnd, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool InTickableWhenPaused)
{
	if (IsValid(GEngine) == false)
	{
		return nullptr;
	}

	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (IsValid(ContextWorld) == false)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = ContextWorld->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		return nullptr;
	}

	UAsyncEasingAction_Vector2D* NewAction = NewObject<UAsyncEasingAction_Vector2D>(GameInstance);
	if (IsValid(NewAction) == false)
	{
		return nullptr;
	}

	NewAction->Begin = InBegin;
	NewAction->End = InEnd;
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InLoopDuration, InLoopCount, InMaxUpdateRate, bInRoundTrip, InTickableWhenPaused);
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UAsyncEasingAction_Vector2D::BeginDestroy()
{
	Update.Clear();
	Completed.Clear();
	Canceled.Clear();

	Super::BeginDestroy();
}

void UAsyncEasingAction_Vector2D::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(GetLocalPercent())));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector2D::Complete()
{
	Super::Complete();
}

void UAsyncEasingAction_Vector2D::Blend(float percent, float blendPercent)
{
	if (Update.IsBound())
	{
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(blendPercent));
		Update.Broadcast(result, percent);
	}
}

void UAsyncEasingAction_Vector2D::BroadcastCompleted(float percent)
{
	if (Completed.IsBound())
	{
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(1.0f)));
		Completed.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Vector* UAsyncEasingAction_Vector::StartEasing_Vector(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration, FVector InBegin, FVector InEnd, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool InTickableWhenPaused)
{
	if (IsValid(GEngine) == false)
	{
		return nullptr;
	}

	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (IsValid(ContextWorld) == false)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = ContextWorld->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		return nullptr;
	}

	UAsyncEasingAction_Vector* NewAction = NewObject<UAsyncEasingAction_Vector>(GameInstance);
	if (IsValid(NewAction) == false)
	{
		return nullptr;
	}

	NewAction->Begin = InBegin;
	NewAction->End = InEnd;
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InLoopDuration, InLoopCount, InMaxUpdateRate, bInRoundTrip, InTickableWhenPaused);
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UAsyncEasingAction_Vector::BeginDestroy()
{
	Update.Clear();
	Completed.Clear();
	Canceled.Clear();

	Super::BeginDestroy();
}

void UAsyncEasingAction_Vector::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(GetLocalPercent())));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector::Complete()
{
	Super::Complete();
}

void UAsyncEasingAction_Vector::Blend(float percent, float blendPercent)
{
	if (Update.IsBound())
	{
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(blendPercent));
		Update.Broadcast(result, percent);
	}
}

void UAsyncEasingAction_Vector::BroadcastCompleted(float percent)
{
	if (Completed.IsBound())
	{
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(1.0f)));
		Completed.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Color* UAsyncEasingAction_Color::StartEasing_Color(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration, FLinearColor InBegin, FLinearColor InEnd, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool InTickableWhenPaused)
{
	if (IsValid(GEngine) == false)
	{
		return nullptr;
	}

	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (IsValid(ContextWorld) == false)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = ContextWorld->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		return nullptr;
	}

	UAsyncEasingAction_Color* NewAction = NewObject<UAsyncEasingAction_Color>(GameInstance);
	if (IsValid(NewAction) == false)
	{
		return nullptr;
	}

	NewAction->Begin = InBegin;
	NewAction->End = InEnd;
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InLoopDuration, InLoopCount, InMaxUpdateRate, bInRoundTrip, InTickableWhenPaused);
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UAsyncEasingAction_Color::BeginDestroy()
{
	Update.Clear();
	Completed.Clear();
	Canceled.Clear();

	Super::BeginDestroy();
}

void UAsyncEasingAction_Color::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(GetLocalPercent())));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Color::Complete()
{
	Super::Complete();
}

void UAsyncEasingAction_Color::Blend(float percent, float blendPercent)
{
	if (Update.IsBound())
	{
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(blendPercent));
		Update.Broadcast(result, percent);
	}
}

void UAsyncEasingAction_Color::BroadcastCompleted(float percent)
{
	if (Completed.IsBound())
	{
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(GetLoopBlendPercent(1.0f)));
		Completed.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Rotator* UAsyncEasingAction_Rotator::StartEasing_Rotator(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InLoopDuration, FRotator InBegin, FRotator InEnd, int32 InLoopCount, int32 InMaxUpdateRate, bool bInRoundTrip, bool InTickableWhenPaused)
{
	if (IsValid(GEngine) == false)
	{
		return nullptr;
	}

	UWorld* ContextWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (IsValid(ContextWorld) == false)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = ContextWorld->GetGameInstance();
	if (IsValid(GameInstance) == false)
	{
		return nullptr;
	}

	UAsyncEasingAction_Rotator* NewAction = NewObject<UAsyncEasingAction_Rotator>(GameInstance);
	if (IsValid(NewAction) == false)
	{
		return nullptr;
	}

	NewAction->Begin = InBegin;
	NewAction->End = InEnd;
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InLoopDuration, InLoopCount, InMaxUpdateRate, bInRoundTrip, InTickableWhenPaused);
	NewAction->RegisterWithGameInstance(ContextWorld->GetGameInstance());

	return NewAction;
}

void UAsyncEasingAction_Rotator::BeginDestroy()
{
	Update.Clear();
	Completed.Clear();
	Canceled.Clear();

	Super::BeginDestroy();
}

void UAsyncEasingAction_Rotator::Cancel()
{
	if (bIsRunning == false)
	{
		SetReadyToDestroy();
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FQuat rotation = FQuat::Slerp(FQuat(Begin), FQuat(End), GetBlendValue(GetLoopBlendPercent(GetLocalPercent())));
		Canceled.Broadcast(rotation.Rotator(), percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Rotator::Complete()
{
	Super::Complete();
}

void UAsyncEasingAction_Rotator::Blend(float percent, float blendPercent)
{
	if (Update.IsBound())
	{
		FQuat BeginQuat = FQuat(Begin);
		FQuat EndQuat = FQuat(End);
		FQuat ResultQuat = FQuat::Slerp(BeginQuat, EndQuat, GetBlendValue(blendPercent));
		Update.Broadcast(ResultQuat.Rotator(), percent);
	}
}

void UAsyncEasingAction_Rotator::BroadcastCompleted(float percent)
{
	if (Completed.IsBound())
	{
		FQuat rotation = FQuat::Slerp(FQuat(Begin), FQuat(End), GetBlendValue(GetLoopBlendPercent(1.0f)));
		Completed.Broadcast(rotation.Rotator(), percent);
	}
}
