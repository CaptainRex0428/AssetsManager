// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardAsset/StructLikeString.h"

FStructLikeString::FStructLikeString(
	FString InString)
	:FString(InString),isStructLike(false)
{
	this->Map.Empty();

	if(InString.StartsWith("(") && InString.EndsWith(")"))
	{
		InString.RemoveFromStart("(");
		InString.RemoveFromEnd(")");

		TArray<FString> StringLikePairs =
			SplitStringRecursive(InString,",");

		if (StringLikePairs.Num()!=0)
		{
			for (FString & StringLikePair : StringLikePairs)
			{
				if(StringLikePair.Contains("=") && 
					!StringLikePair.StartsWith("=") 
					&& !StringLikePair.EndsWith("="))
				{
					FString LeftS;
					FString RightS;
					StringLikePair.Split("=", &LeftS, &RightS);

					LeftS.RemoveFromStart("\"");
					LeftS.RemoveFromEnd("\"");

					RightS.RemoveFromStart("\"");
					RightS.RemoveFromEnd("\"");

					this->Map.Add(LeftS, RightS);
				}
			}
		}

		if(this->Map.Num()!=0)
		{
			this->isStructLike = true;
		}
	}


}

FStructLikeString::~FStructLikeString()
{
}

bool FStructLikeString::IsStructLike()
{
	return this->isStructLike;
}

TMap<FString, FString>& FStructLikeString::GetMap()
{
	return this->Map;
}

TArray<FString> FStructLikeString::SplitStringRecursive(
	const FString& InStr, 
	const FString& SpliteTag)
{
	TArray<FString> OutList;
	OutList.Empty();

	FString LStr;
	FString RStr;

	if (InStr.Split(SpliteTag, &LStr, &RStr))
	{
		OutList.Add(LStr);

		while (RStr.Split(SpliteTag, &LStr, &RStr))
		{
			OutList.Add(LStr);
		}

		OutList.Add(RStr);
	}
	else
	{
		OutList.Add(InStr);
	}

	return OutList;
}
