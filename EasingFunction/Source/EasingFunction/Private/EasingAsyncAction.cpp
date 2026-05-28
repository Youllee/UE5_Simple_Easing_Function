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

	float percent = GetPercent();
	Blend(percent);

	if (ElapsedTime >= Duration)
	{
		Complete();
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

	Super::Activate();
}

void UAsyncEasingAction::Cancel()
{
	if (bIsRunning == false)
	{
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction::InitializeBase(UWorld* InContextWorld, EEasingType InEasingType, float InExponential, float InDuration, bool bInTickableWhenPaused)
{
	WorldContextObject = InContextWorld;
	EasingType = InEasingType;
	Exponential = FMath::Max(InExponential, 0.0f);
	Duration = FMath::Max(InDuration, 0.0f);
	bTickableWhenPaused = bInTickableWhenPaused;
}

float UAsyncEasingAction::GetBlendValue(float percent)
{
	return UEasingFunctionLibrary::EasingFunction(percent, EasingType, Exponential);
}

float UAsyncEasingAction::GetPercent()
{
	return (Duration <= 0.0f) ? 1.0f : FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
}

UAsyncEasingAction_Float* UAsyncEasingAction_Float::StartEasing_Float(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration, float InBegin, float InEnd, bool InTickableWhenPaused)
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
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InDuration, InTickableWhenPaused);
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		float result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Float::Complete()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Completed.IsBound())
	{
		float result = FMath::Lerp(Begin, End, GetBlendValue(1.0f));
		Completed.Broadcast(result, 1.0f);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Float::Blend(float percent)
{
	if (Update.IsBound())
	{
		float result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Update.Broadcast(result, percent);
	}
}


UAsyncEasingAction_Vector2D* UAsyncEasingAction_Vector2D::StartEasing_Vector2D(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration, FVector2D InBegin, FVector2D InEnd, bool InTickableWhenPaused)
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
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InDuration, InTickableWhenPaused);
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector2D::Complete()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Completed.IsBound())
	{
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(1.0f));
		Completed.Broadcast(result, 1.0f);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector2D::Blend(float percent)
{
	if (Update.IsBound())
	{
		FVector2D result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Update.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Vector* UAsyncEasingAction_Vector::StartEasing_Vector(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration, FVector InBegin, FVector InEnd, bool InTickableWhenPaused)
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
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InDuration, InTickableWhenPaused);
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector::Complete()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Completed.IsBound())
	{
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(1.0f));
		Completed.Broadcast(result, 1.0f);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Vector::Blend(float percent)
{
	if (Update.IsBound())
	{
		FVector result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Update.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Color* UAsyncEasingAction_Color::StartEasing_Color(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration, FLinearColor InBegin, FLinearColor InEnd, bool InTickableWhenPaused)
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
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InDuration, InTickableWhenPaused);
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Canceled.Broadcast(result, percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Color::Complete()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Completed.IsBound())
	{
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(1.0f));
		Completed.Broadcast(result, 1.0f);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Color::Blend(float percent)
{
	if (Update.IsBound())
	{
		FLinearColor result = FMath::Lerp(Begin, End, GetBlendValue(percent));
		Update.Broadcast(result, percent);
	}
}

UAsyncEasingAction_Rotator* UAsyncEasingAction_Rotator::StartEasing_Rotator(const UObject* WorldContextObject, EEasingType InEasingType, float InExponential, float InDuration, FRotator InBegin, FRotator InEnd, bool InTickableWhenPaused)
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
	NewAction->InitializeBase(ContextWorld, InEasingType, InExponential, InDuration, InTickableWhenPaused);
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
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Canceled.IsBound())
	{
		float percent = GetPercent();
		FQuat rotation = FQuat::Slerp(FQuat(Begin), FQuat(End), GetBlendValue(percent));
		Canceled.Broadcast(rotation.Rotator(), percent);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Rotator::Complete()
{
	if (bIsRunning == false)
	{
		return;
	}

	bIsRunning = false;
	bTickable = false;

	if (Completed.IsBound())
	{
		FQuat rotation = FQuat::Slerp(FQuat(Begin), FQuat(End), GetBlendValue(1.0f));
		Completed.Broadcast(rotation.Rotator(), 1.0f);
	}

//	Super::Cancel();
	SetReadyToDestroy();
}

void UAsyncEasingAction_Rotator::Blend(float percent)
{
	if (Update.IsBound())
	{
		FQuat BeginQuat = FQuat(Begin);
		FQuat EndQuat = FQuat(End);
		FQuat ResultQuat = FQuat::Slerp(BeginQuat, EndQuat, GetBlendValue(percent));
		Update.Broadcast(ResultQuat.Rotator(), percent);
	}
}
