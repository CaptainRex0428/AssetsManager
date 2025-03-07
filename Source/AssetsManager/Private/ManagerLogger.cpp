#include "ManagerLogger.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "FrameWork/Notifications/NotificationManager.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY(LogAssetsManager);

void MsgLog(const FString& Message, bool isWarning)
{
	if (isWarning)
	{
		UE_LOG(LogAssetsManager, Warning, TEXT("%s"), *Message);
		return;
	}

	UE_LOG(LogAssetsManager, Log, TEXT("%s"), *Message);
}

// Screen Debug

void ScreenMsg(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

void ScreenMsgLog(const FString& Message, const FColor& Color)
{
	ScreenMsg(Message, Color);
	MsgLog(Message);
}

// Dialog Debug

EAppReturnType::Type DlgMsgLog(EAppMsgType::Type MsgType, const FString& Message, bool isWarning, bool isLog)
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

EAppReturnType::Type DlgMsg(EAppMsgType::Type MsgType, const FString& Message, bool isWarning)
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

void NtfyMsgLog(const FString& Message, bool isWarning)
{
	NtfyMsg(Message);
	MsgLog(Message, isWarning);
}
