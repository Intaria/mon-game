// Copyright (c) mon-game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataEnums.h"

/**
 * String <-> enum conversion for CSV enum values.
 * Shared by the CSV loader and the data validator so a value accepted at load time
 * is exactly a value the validator accepts (single source of truth for enum spelling).
 */
namespace MonGame::DataEnums
{
	MONGEDATA_API TOptional<EAbilityKind> ParseAbilityKind(const FString& InValue);
	MONGEDATA_API FString AbilityKindToString(EAbilityKind InValue);

	MONGEDATA_API TOptional<ETargetScope> ParseTargetScope(const FString& InValue);
	MONGEDATA_API FString TargetScopeToString(ETargetScope InValue);

	MONGEDATA_API TOptional<EAnimProfile> ParseAnimProfile(const FString& InValue);
	MONGEDATA_API FString AnimProfileToString(EAnimProfile InValue);
}
