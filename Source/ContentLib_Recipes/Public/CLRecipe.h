

#pragma once

#include "CoreMinimal.h"

#include "FGRecipe.h"

#include "CLRecipe.generated.h"


class UFGSchematic;
USTRUCT(BlueprintType)
struct  CONTENTLIB_RECIPES_API  FContentLib_Recipe
{
	GENERATED_BODY()
	FContentLib_Recipe(): ManufacturingDuration(1), mVariablePowerConsumptionFactor(1),
	                      mVariablePowerConsumptionConstant(0), ClearIngredients(true), ClearProducts(true),
	                      ClearBuilders(true)
	{
	} ;
	UPROPERTY(BlueprintReadWrite)
		FString Name;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Ingredients;
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, int32> Products;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> BuildIn;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> UnlockedBy;
	UPROPERTY(BlueprintReadWrite)
		float ManufacturingDuration;
	UPROPERTY(BlueprintReadWrite)
		float mVariablePowerConsumptionFactor;
	UPROPERTY(BlueprintReadWrite)
		float mVariablePowerConsumptionConstant;

	UPROPERTY(BlueprintReadWrite)
		bool ClearIngredients;
	UPROPERTY(BlueprintReadWrite)
		bool ClearProducts;
	UPROPERTY(BlueprintReadWrite)
		bool ClearBuilders;

};

/**
 * 
 */
UCLASS()
class CONTENTLIB_RECIPES_API UCLRecipe : public UFGRecipe
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static void InitFromStruct(FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe, bool ClearIngredients = true, bool ClearProducts = true, bool ClearBuilders = true);

	UFUNCTION(BlueprintCallable)
	static TSubclassOf<UCLRecipe> CreateDynRecipe(FString Name);
	UFUNCTION(BlueprintCallable)
	static void AddToSchematicUnlock(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Schematics);
	UFUNCTION(BlueprintCallable)
		static void AddBuilders(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, bool ClearFirst = false);
	UFUNCTION(BlueprintCallable)
		static void AddProductOrIngredient(TSubclassOf<class UFGRecipe> Recipe, FContentLib_Recipe RecipeStruct, TArray<UClass*> Items, bool Ingredient, bool ClearFirst = false);

};
