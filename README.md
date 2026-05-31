# UE5 Simple Easing Function

> Built as a lightweight Blueprint utility plugin for simple easing and async value interpolation in Unreal Engine 5.

**2026.05.31 v1.0.0에서 v1.1로 업데이트 되었습니다.**

간단한 가속/감속 함수(Easing Function)와 비동기 액션 노드를 제공하는 Unreal Engine 5 플러그인 입니다.

'프로그래머' 뿐 아니라 '디자이너', '아티스트' 등 모든 개발 직군이 쉽게 사용할 수 있도록, 최대한 단순하고 직관적인 형태로 만드는 데 주력했습니다.

이 플러그인을 활용하면 복잡한 타임라인이나 커브 에셋을 만들지 않아도, 일정 시간 동안 값이 자연스럽게 변화하는 동작을 간편하게 구현할 수 있습니다.

---

## Features

- 블루프린트에서 사용할 수 있는 Easing Function Library 제공
- Float / Vector2D / Vector / Color / Rotator 타입 지원
- 비동기 액션 기반의 보간 노드 지원 (Start Easing …)
- 비동기 액션에서 'Complete(완료)', 'Cancel(취소)' 함수 지원. 두 함수 모두 부모 클래스 'CancellableAsyncAction'의 'Cancel' 함수를 대체함.
- 비동기 액션에서 'Update(매 틱 호출)', 'Completed(액션 완료됨)', 'Canceled(액션 취소됨)' 이벤트 제공
- 비동기 액션에서 일시정지(Pause) 중 Tick 실행 여부 선택 가능
- 별도 플러그인 의존성 없이 사용 가능
- 콘텐츠 에셋 없이 C++ Runtime 모듈로만 구성

---

## Why This Plugin?

Unreal Engine 5에서는 Timeline, Curve, Tween 계열 로직 등 다양한 방식으로 값을 보간할 수 있습니다.

하지만 간단한 UI 애니메이션, 라이트 변화, 위치 이동, 색상 변화처럼,
"짧고 단순한 가•감속 동작"이 필요할 때마다 매번 Curve 에셋을 만들거나 Timeline을 구성하는 것은 번거로울 수 있습니다.

이 플러그인은 그런 상황에서 다음과 같은 목표로 사용할 수 있습니다.

- 빠르게 값을 보간하고 싶을 때
- Blueprint 노드를 복잡하게 만들고 싶지 않을 때
- 프로그래머가 아니어도 쉽게 사용할 수 있는 이징 노드가 필요할 때
- 간단한 연출용 애니메이션을 코드나 커브 없이 구성하고 싶을 때

복잡한 곡선 제어가 필요하거나 프로젝트 고유의 보간 방식이 필요한 경우에는,
Curve 에셋을 사용하거나 직접 타입을 추가하는 것을 권장합니다.

---

## Supported Value Types

![Async Action Node Pins](Docs/EasingAsyncAction_ValueType.png)

비동기 액션 노드는 다음 타입을 지원합니다.

| Type | Node |
| --- | --- |
| Float | `Start Easing (Float)` |
| Vector2D | `Start Easing (Vector2D)` |
| Vector | `Start Easing (Vector)` |
| Color | `Start Easing (Color)` |
| Rotator | `Start Easing (Rotator)` |

각 노드는 시작 값(Begin)과 종료 값(End)을 받아,
지정한 시간(Duration) 동안 선택한 Easing Type에 따라 값을 보간된 값(Value)과 진행도(Percent)를 반환합니다.

---

## Supported Easing Types

![Easing Function Example 1](Docs/Example_EasingFunc1.gif)

![Easing Function Example 2](Docs/Example_EasingFunc2.gif)

지원하는 Easing Type은 다음과 같습니다.

- Linear
- Step
- EaseIn / EaseOut / EaseInOut / EaseOutIn
- SineIn / SineOut / SineInOut / SineOutIn
- CircleIn / CircleOut / CircleInOut / CircleOutIn
- ElasticIn / ElasticOut / ElasticInOut / ElasticOutIn
- BackIn / BackOut / BackInOut / BackOutIn
- BounceIn / BounceOut / BounceInOut / BounceOutIn

---

## Node Pins

![Async Action Node Pins](Docs/EasingAsyncAction_NodePin.png)

기본 입력은 다음과 같습니다.

| In Pin | Description |
| --- | --- |
| Easing Type | 사용할 가/감속 방식입니다. |
| Exponential | 일부 Easing 계산에 사용되는 지수 값입니다. |
| LoopDuration | (v1.1) 1회 루프에 걸리는 시간입니다. v1.0.0의 Duration을 대체합니다. |
| Begin | 시작 값입니다. |
| End | 종료 값입니다. |
| LoopCount | (v1.1) 루프 횟수를 지정합니다. |
| RoundTrip | (v1.1) 액션을 왕복 운동으로 전환합니다. |
| MaxUpdateRate | (v1.1) 초당 Update 호출 횟수입니다. |
| Tickable When Paused | 게임이 일시정지된 상태에서도 Tick을 받을지 설정합니다. |

| Out Pin | Description |
| --- | --- |
| Update | 매 Tick마다 현재 보간 값과 진행률을 반환하는 이벤트 입니다. |
| Completed | 보간이 정상적으로 끝났을 때 호출되는 이벤트 입니다. |
| Canceled | 사용자가 액션을 취소했을 때 호출되는 이벤트 입니다. |
| Value | 반환된 보간 값입니다. |
| Percent | (v1.1) 비동기 액션의 누적 진행도 입니다. N회 루프 시 0.0 ~ N.0을 반환합니다. |

---

## Examples

### Actor Movement

Actor의 위치, 회전, 크기, 머티리얼 파라미터를 일정 시간 동안 자연스럽게 변화시킬 수 있습니다.

![Actor Example](Docs/Example_ActorBase.gif)

### Light / Color Change

Color 타입 비동기 액션을 사용하면 라이트 색상이나 UI 색상 변화에도 사용할 수 있습니다.

![Light Example](Docs/Example_EasingLight.gif)

---

## Function Library

`Easing Function`은 단독 Blueprint Pure 함수로도 사용할 수 있습니다.

입력한 `Percent` 값을 선택한 `Easing Type`과 `Exponential` 값에 따라 변환하여 반환합니다.

간단한 수식 기반 보간이 필요하거나, 직접 값을 계산해서 사용하고 싶은 경우 Function Library를 사용할 수 있습니다.

---

## Installation

1. 이 저장소의 최신 릴리즈에서 'Source code (zip)'을 다운로드 합니다.
   - 2026.05.31 기준 v1.1
2. 프로젝트의 `Plugins` 폴더에 플러그인 폴더를 복사합니다.
3. Unreal Editor를 실행합니다.
4. Plugins 창에서 `Simple Easing Function` 플러그인을 활성화합니다.
5. 프로젝트를 재시작합니다.

---

## Usage

Blueprint에서 다음 노드를 검색해 사용할 수 있습니다.

- `Easing Function`
- `Start Easing (Float)`
- `Start Easing (Vector2D)`
- `Start Easing (Vector)`
- `Start Easing (Color)`
- `Start Easing (Rotator)`

간단한 예시는 다음과 같습니다.

1. `Start Easing (Float)` 노드를 배치합니다.
2. `Begin`, `End`, `Duration`, `Easing Type`을 설정합니다.
3. `Update` 이벤트에서 반환된 값을 원하는 대상에 적용합니다.
4. 필요하다면 `Completed` 또는 `Canceled` 이벤트를 연결합니다.

---

## Recommended Use Cases

- UI 패널 이동
- 버튼/위젯 페이드 또는 스케일 변화
- Actor 위치 보간
- 카메라 회전 보간
- 라이트 색상 또는 강도 변화
- 간단한 연출용 애니메이션
- 짧은 Tween 스타일 동작

---

## Notes

이 플러그인은 간단하고 직관적인 Easing 사용을 목표로 합니다.

복잡한 애니메이션 제어, 세밀한 곡선 편집, 프로젝트 전용 커스텀 보간이 필요한 경우에는 다음 방식을 권장합니다.

- Unreal Engine Curve 에셋 사용
- Timeline 사용
- 프로젝트에 맞는 커스텀 Easing Type 추가
- 별도의 애니메이션 시스템 구성

---

## Design Note

플러그인의 제작 목적이 코드 스타일을 보여주기 위한 포트폴리오로서 제작된 만큼,
중복 제거보다 코드 가독성, 타입별 동작의 명확성, 블루프린트 노드 구조의 직관성을 우선했습니다.

---

## Current Status

- Portfolio project
- Runtime plugin
- Developed with Unreal Engine 5.7
- No content assets required

---

## License

This project is licensed under the MIT License.
