

#pragma once

#include "CoreMinimal.h"

#include "CLRecipe.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ContentLib_RecipesSubsystem.generated.h"

USTRUCT(BlueprintType)
struct CONTENTLIB_RECIPES_API FJsonRecipe
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		TSubclassOf<UFGRecipe> Class;
	UPROPERTY(BlueprintReadWrite)
		FString Json;
};



/**
 * 
 */
UCLASS()
class CONTENTLIB_RECIPES_API UContentLib_RecipesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void FillLoadedClasses();
	UFUNCTION(BlueprintCallable)
	FContentLib_Recipe GenerateFromString(FString String);
	UFUNCTION(BlueprintCallable)
	FString SerializeRecipe(TSubclassOf<UFGRecipe> Recipe);
	UFUNCTION(BlueprintCallable)
	FString SerializeCLRecipe(FContentLib_Recipe Recipe);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UObject> CreateContentLibRecipe(FString Name, TSubclassOf<UObject> Class);

	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> ItemCategories;

	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Items;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Builders;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> CraftingComps;
	UPROPERTY(BlueprintReadOnly)
	TArray<UClass*> Schematics;
	
	UPROPERTY(BlueprintReadOnly)
		TMap<TSubclassOf<UCLRecipe>,FString> DynRecipes;
	
	UPROPERTY(BlueprintReadOnly)
		TArray<FJsonRecipe> RecipePatches;

};
