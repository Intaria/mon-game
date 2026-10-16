// Copyright (c) mon-game. All rights reserved.

#include "EntityId.h"

TOptional<FEntityId> FEntityId::Parse(const FString& InValue)
{
	if (!IsValidSyntax(InValue))
	{
		return {};
	}
	return FEntityId(InValue);
}

bool FEntityId::IsValidSyntax(const FString& InValue)
{
	if (InValue.IsEmpty())
	{
		return false;
	}

	int32 SegmentLength = 0;
	for (const TCHAR Char : InValue)
	{
		if (Char == TEXT('.'))
		{
			// Both sides of every dot must be non-empty, including the trailing segment.
			if (SegmentLength == 0)
			{
				return false;
			}
			SegmentLength = 0;
			continue;
		}

		const bool bLowerAlpha = Char >= TEXT('a') && Char <= TEXT('z');
		const bool bDigit = Char >= TEXT('0') && Char <= TEXT('9');
		if (!bLowerAlpha && !bDigit && Char != TEXT('_'))
		{
			return false;
		}
		++SegmentLength;
	}

	// At least two segments: "<namespace>.<name>"; a trailing dot leaves SegmentLength == 0.
	return SegmentLength > 0 && InValue.ContainsChar(TEXT('.'));
}
