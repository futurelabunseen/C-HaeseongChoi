// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// NetMode를 가져오는 매크로
#define LOG_NETMODINFO ((GetNetMode() == NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : (GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))

// 어떤 함수인지 정보를 알 수 있는 예약어
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define SS_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] %s %s"), LOG_NETMODINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 카테고리 생성
DECLARE_LOG_CATEGORY_EXTERN(LogSSNetwork, Log, All);