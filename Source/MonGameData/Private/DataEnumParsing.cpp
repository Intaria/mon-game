// Copyright (c) mon-game. All rights reserved.

#include "DataEnumParsing.h"

namespace MonGame::DataEnums
{
	TOptional<EAbilityKind> ParseAbilityKind(const FString& InValue)
	{
		const FString Value = InValue.ToLower();
		if (Value == TEXT("physical"))
		{
			return EAbilityKind::Physical;
		}
		if (Value == TEXT("energy"))
		{
			return EAbilityKind::Energy;
		}
		return {};
	}

	FString AbilityKindToString(EAbilityKind InValue)
	{
		return InValue == EAbilityKind::Physical ? TEXT("physical") : TEXT("energy");
	}

	TOptional<ETargetScope> ParseTargetScope(const FString& InValue)
	{
		const FString Value = InValue.ToLower();
		if (Value == TEXT("self"))
		{
			return ETargetScope::Self;
		}
		if (Value == TEXT("enemy"))
		{
			return ETargetScope::Enemy;
		}
		if (Value == TEXT("ally"))
		{
			return ETargetScope::Ally;
		}
		if (Value == TEXT("any"))
		{
			return ETargetScope::Any;
		}
		if (Value == TEXT("all"))
		{
			return ETargetScope::All;
		}
		if (Value == TEXT("none"))
		{
			return ETargetScope::None;
		}
		if (Value == TEXT("random"))
		{
			return ETargetScope::Random;
		}
		return {};
	}

	FString TargetScopeToString(ETargetScope InValue)
	{
		switch (InValue)
		{
		case ETargetScope::Self: return TEXT("self");
		case ETargetScope::Enemy: return TEXT("enemy");
		case ETargetScope::Ally: return TEXT("ally");
		case ETargetScope::Any: return TEXT("any");
		case ETargetScope::All: return TEXT("all");
		case ETargetScope::None: return TEXT("none");
		case ETargetScope::Random: return TEXT("random");
		default: return FString();
		}
	}

	TOptional<EAnimProfile> ParseAnimProfile(const FString& InValue)
	{
		const FString Value = InValue.ToLower();
		if (Value == TEXT("default"))
		{
			return EAnimProfile::Default;
		}
		if (Value == TEXT("angry"))
		{
			return EAnimProfile::Angry;
		}
		if (Value == TEXT("happy"))
		{
			return EAnimProfile::Happy;
		}
		if (Value == TEXT("lazy"))
		{
			return EAnimProfile::Lazy;
		}
		return {};
	}

	FString AnimProfileToString(EAnimProfile InValue)
	{
		switch (InValue)
		{
		case EAnimProfile::Default: return TEXT("default");
		case EAnimProfile::Angry: return TEXT("angry");
		case EAnimProfile::Happy: return TEXT("happy");
		case EAnimProfile::Lazy: return TEXT("lazy");
		default: return FString();
		}
	}
}
