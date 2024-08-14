#pragma once

void MsgLog(const FString& Message, bool isWarning = true);

// Screen Debug

void ScreenMsg(const FString& Message, const FColor& Color = FColor::Cyan);

void ScreenMsgLog(const FString& Message, const FColor& Color = FColor::Cyan);

// Dialog Debug

EAppReturnType::Type DlgMsgLog(EAppMsgType::Type MsgType, const FString& Message, bool isWarning = true, bool isLog = true);

EAppReturnType::Type DlgMsg(EAppMsgType::Type MsgType, const FString& Message, bool isWarning = true);

// Notification Debug

void NtfyMsg(const FString& Message);

void NtfyMsgLog(const FString& Message, bool isWarning = false);
