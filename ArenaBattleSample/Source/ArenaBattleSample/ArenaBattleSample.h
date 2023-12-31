// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define LOG_LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define LOG_REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))

#define LOG_SUB_LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUB_REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))

#define LOG_NET_MODE_INFO (GetNetMode() == ENetMode::NM_Client ? *FString::Printf(TEXT("CLIENT_%d"), GPlayInEditorID) : GetNetMode() == ENetMode::NM_Standalone ? TEXT("STANDALONE") : TEXT("SERVER"))

#define AB_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NET_MODE_INFO, LOG_LOCAL_ROLE_INFO, LOG_REMOTE_ROLE_INFO, *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))
#define AB_SUB_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NET_MODE_INFO, LOG_SUB_LOCAL_ROLE_INFO, LOG_SUB_REMOTE_ROLE_INFO, *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);
