// RPGSaveGame.cpp

#include "RPGSaveGame.h"

const FRPGSavedObject* URPGSaveGame::FindObject(const FString& Key) const
{
	return Objects.FindByPredicate([&Key](const FRPGSavedObject& Object) { return Object.Key == Key; });
}
