// Copyright (c) mon-game. All rights reserved.

#include "Misc/AutomationTest.h"

#include "EntityId.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEntityIdValidSyntaxTest, "MonGame.Data.EntityId.ValidSyntax",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FEntityIdValidSyntaxTest::RunTest(const FString& Parameters)
{
	// Namespaced slugs: at least "<namespace>.<name>", lowercase, digits and underscores.
	TestTrue(TEXT("two segments"), FEntityId::IsValidSyntax(TEXT("type.fire")));
	TestTrue(TEXT("three segments"), FEntityId::IsValidSyntax(TEXT("a.b.c")));
	TestTrue(TEXT("digits and underscores"), FEntityId::IsValidSyntax(TEXT("zone.route01_grass")));

	TestFalse(TEXT("empty"), FEntityId::IsValidSyntax(TEXT("")));
	TestFalse(TEXT("no dot"), FEntityId::IsValidSyntax(TEXT("species")));
	TestFalse(TEXT("leading dot"), FEntityId::IsValidSyntax(TEXT(".fire")));
	TestFalse(TEXT("trailing dot"), FEntityId::IsValidSyntax(TEXT("type.")));
	TestFalse(TEXT("double dot"), FEntityId::IsValidSyntax(TEXT("type..fire")));
	TestFalse(TEXT("uppercase"), FEntityId::IsValidSyntax(TEXT("Type.Fire")));
	TestFalse(TEXT("dash not allowed"), FEntityId::IsValidSyntax(TEXT("species.bulba-saur")));
	TestFalse(TEXT("space not allowed"), FEntityId::IsValidSyntax(TEXT("species.bulba saur")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEntityIdParseTest, "MonGame.Data.EntityId.Parse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FEntityIdParseTest::RunTest(const FString& Parameters)
{
	const TOptional<FEntityId> Parsed = FEntityId::Parse(TEXT("species.bulbasaur"));
	TestTrue(TEXT("valid id parses"), Parsed.IsSet());
	if (Parsed.IsSet())
	{
		TestEqual(TEXT("round trip"), Parsed->ToString(), FString(TEXT("species.bulbasaur")));
		TestTrue(TEXT("valid flag"), Parsed->IsValid());
	}

	TestFalse(TEXT("invalid id rejected"), FEntityId::Parse(TEXT("Species")).IsSet());

	FEntityId DefaultId;
	TestFalse(TEXT("default id is invalid"), DefaultId.IsValid());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
