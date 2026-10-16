// Copyright (c) mon-game. All rights reserved.

#include "Misc/AutomationTest.h"

#include "DataEnumParsing.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataEnumParsingTest, "MonGame.Data.Enums.CsvSpellings",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FDataEnumParsingTest::RunTest(const FString& Parameters)
{
	// CSV spellings are lowercase (schema.md §3 notation); parsing is case-insensitive.
	const TOptional<EAbilityKind> Physical = MonGame::DataEnums::ParseAbilityKind(TEXT("physical"));
	TestTrue(TEXT("physical parses"), Physical.IsSet());
	TestEqual(TEXT("physical spelling"), MonGame::DataEnums::AbilityKindToString(EAbilityKind::Physical), FString(TEXT("physical")));
	TestEqual(TEXT("energy spelling"), MonGame::DataEnums::AbilityKindToString(EAbilityKind::Energy), FString(TEXT("energy")));
	TestFalse(TEXT("unknown kind rejected"), MonGame::DataEnums::ParseAbilityKind(TEXT("special")).IsSet());
	TestTrue(TEXT("case-insensitive"), MonGame::DataEnums::ParseAbilityKind(TEXT("PHYSICAL")).IsSet());

	const ETargetScope AllScopes[] = {
		ETargetScope::Self, ETargetScope::Enemy, ETargetScope::Ally, ETargetScope::Any,
		ETargetScope::All, ETargetScope::None, ETargetScope::Random
	};
	for (const ETargetScope Scope : AllScopes)
	{
		const FString Spelling = MonGame::DataEnums::TargetScopeToString(Scope);
		const TOptional<ETargetScope> Parsed = MonGame::DataEnums::ParseTargetScope(Spelling);
		TestTrue(*FString::Printf(TEXT("target scope round trip: %s"), *Spelling), Parsed.IsSet());
		if (Parsed.IsSet())
		{
			TestTrue(*FString::Printf(TEXT("target scope value: %s"), *Spelling), Parsed.GetValue() == Scope);
		}
	}
	TestFalse(TEXT("unknown scope rejected"), MonGame::DataEnums::ParseTargetScope(TEXT("everyone")).IsSet());

	const EAnimProfile AllProfiles[] = { EAnimProfile::Default, EAnimProfile::Angry, EAnimProfile::Happy, EAnimProfile::Lazy };
	for (const EAnimProfile Profile : AllProfiles)
	{
		const FString Spelling = MonGame::DataEnums::AnimProfileToString(Profile);
		const TOptional<EAnimProfile> Parsed = MonGame::DataEnums::ParseAnimProfile(Spelling);
		TestTrue(*FString::Printf(TEXT("anim profile round trip: %s"), *Spelling), Parsed.IsSet());
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
