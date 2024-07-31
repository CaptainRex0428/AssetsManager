#pragma once

#include "Widgets/Notifications/SNotificationList.h"
#include "FrameWork/Notifications/NotificationManager.h"

void MsgLog(const FString& Message, bool isWarning = true)
{
	if (isWarning)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
}

// Screen Debug

void ScreenMsg(const FString& Message, const FColor& Color = FColor::Cyan)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

void ScreenMsgLog(const FString& Message, const FColor& Color = FColor::Cyan)
{
	ScreenMsg(Message, Color);
	MsgLog(Message);
}

// Dialog Debug

EAppReturnType::Type DlgMsgLog(EAppMsgType::Type MsgType, const FString& Message, bool isWarning = true, bool isLog = true)
{
	if (isWarning)
	{
		FString title = TEXT("WARNING");
		if (isLog)
		{
			MsgLog(Message, isWarning);
		}
		return FMessageDialog::Open(MsgType, FText::FromString(Message), FText::FromString(title));
	}

	FString title = TEXT("TIP");
	if (isLog)
	{
		MsgLog(Message, isWarning);
	}
	return FMessageDialog::Open(MsgType, FText::FromString(Message), FText::FromString(title));
}

EAppReturnType::Type DlgMsg(EAppMsgType::Type MsgType, const FString& Message, bool isWarning = true)
{
	return DlgMsgLog(MsgType, Message, isWarning, false);
}

// Notification Debug

void NtfyMsg(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}

void NtfMsgLog(const FString& Message, bool isWarning = false)
{
	NtfyMsg(Message);
	MsgLog(Message, isWarning);
}
