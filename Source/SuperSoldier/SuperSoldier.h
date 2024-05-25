// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))

#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))

// NetMode를 가져오는 매크로
#define LOG_NETMODEINFO ((GetNetMode() == NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : (GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))
#define LOG_SUBNETMODEINFO ((GetOwner()->GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), static_cast<int32>(GPlayInEditorID)) : ((GetOwner()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

// 어떤 함수인지 정보를 알 수 있는 예약어
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define SS_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NETMODEINFO, LOG_LOCALROLEINFO, LOG_REMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define SS_SUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_SUBNETMODEINFO, LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 카테고리 생성
DECLARE_LOG_CATEGORY_EXTERN(LogSSNetwork, Log, All);