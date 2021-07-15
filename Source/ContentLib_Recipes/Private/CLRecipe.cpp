


#include "CLRecipe.h"
#include "FGSchematic.h"
#include "FGSchematicManager.h"
#include "FGUnlockSubsystem.h"
#include "FGWorkBench.h"
#include "ContentLib_RecipesSubsystem.h"
#include "FGItemCategory.h"
#include "Buildables/FGBuildableFactory.h"
#include "Registry/ModContentRegistry.h"
#include "Unlocks/FGUnlockRecipe.h"

void UCLRecipe::InitFromStruct(UContentLib_RecipesSubsystem* Subsystem ,FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe,bool ClearIngredients,bool ClearProducts,bool ClearBuilders)
{
	if (!Recipe)
		return;
	UFGRecipe *  CDO = Recipe.GetDefaultObject();

	if (RecipeStruct.Name != "")
	{
		CDO->mDisplayName = FText::FromString(RecipeStruct.Name);
		CDO->mDisplayNameOverride = true;

	}

	if (RecipeStruct.OverrideName == 1)
	{
		CDO->mDisplayNameOverride = true;
	}
	else if (RecipeStruct.OverrideName == 0)
	{
		CDO->mDisplayNameOverride = false;
	}

	if (RecipeStruct.Category != "")
	{
		if (RecipeStruct.Category.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *RecipeStruct.Category);
			if (Loaded && Loaded->IsChildOf(UFGItemCategory::StaticClass()))
				CDO->mOverriddenCategory = Loaded;
			else
			{
				FString Left;
				FString Right;
				RecipeStruct.Category.Split("/", &Left, &Right, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
				if (Right != "")
				{
					FString TempName = "Cat_";
					TempName.Append(Right);
					UFGItemCategory* Cat = Cast<UFGItemCategory>(
						Subsystem->CreateContentLibRecipe(TempName, UFGItemCategory::StaticClass()));
					if (Cat)
					{
						Cat->mDisplayName = FText::FromString(Right);
						CDO->mOverriddenCategory = Cat->GetClass();
					}
					else
					{
						UClass* Find = FindObject<UClass>(ANY_PACKAGE, *TempName, false);
						if (!Find)
						{
							Find = FindObject<UClass>(ANY_PACKAGE, *TempName.Append("_C"), false);
						}
						if (Find && Find->IsChildOf(UFGItemCategory::StaticClass()))
						{
							Cat->mDisplayName = FText::FromString(Right);
							CDO->mOverriddenCategory = Find;
						}
					}
				}
			}
		}
		else
		{
			bool Added = false;
			for (auto e : Subsystem->ItemCategories)
			{
				FString S = e->GetName();
				FString A = RecipeStruct.Category;
				FString DescPre = FString("Cat_").Append(RecipeStruct.Category);
				if (!DescPre.EndsWith("_C"))
				{
					DescPre.Append("_C");
				}
				if (S.Equals(RecipeStruct.Category, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
				{
					CDO->mOverriddenCategory = e;
					Added = true;

					break;
				}
			}
			if (!Added)
			{
				UFGItemCategory* Cat = Cast<UFGItemCategory>(
					Subsystem->CreateContentLibRecipe(RecipeStruct.Category, UFGItemCategory::StaticClass()));
				if (Cat)
				{
					Cat->mDisplayName = FText::FromString(RecipeStruct.Category);
					CDO->mOverriddenCategory = Cat->GetClass();
				}
				else
				{
					UClass* Find = FindObject<UClass>(ANY_PACKAGE, *RecipeStruct.Category, false);
					if (!Find)
					{
						FString TempS = RecipeStruct.Category;
						Find = FindObject<UClass>(ANY_PACKAGE, *TempS.Append("_C"), false);
					}
					if (Find && Find->IsChildOf(UFGItemCategory::StaticClass()))
					{
						Cat->mDisplayName = FText::FromString(RecipeStruct.Category);
						CDO->mOverriddenCategory = Find;
					}
				}
			}
		}
	}
	AddProductOrIngredient(Recipe,RecipeStruct,Subsystem->Items,true, ClearIngredients);
	AddProductOrIngredient(Recipe,RecipeStruct,Subsystem->Items,false, ClearProducts);
	AddBuilders(Recipe,RecipeStruct,Subsystem->Builders,Subsystem->CraftingComps, ClearBuilders);
	AddToSchematicUnlock(Recipe,RecipeStruct,Subsystem);
	if(RecipeStruct.ManufacturingDuration != -1)
		CDO->mManufactoringDuration = RecipeStruct.ManufacturingDuration;
	if (RecipeStruct.ManualManufacturingMultiplier != -1)
		CDO->mManualManufacturingMultiplier = RecipeStruct.ManualManufacturingMultiplier;
	if (RecipeStruct.VariablePowerConsumptionConstant != -1)
		CDO->mVariablePowerConsumptionConstant = RecipeStruct.VariablePowerConsumptionConstant;
	if (RecipeStruct.VariablePowerConsumptionFactor != -1)
		CDO->mVariablePowerConsumptionFactor = RecipeStruct.VariablePowerConsumptionFactor;
	if (RecipeStruct.ManufacturingMenuPriority != -1)
		CDO->mManufacturingMenuPriority = RecipeStruct.ManufacturingMenuPriority;

}

void UCLRecipe::AddProductOrIngredient(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Items, bool Ingredient, bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
	{
		if (Ingredient)
			Recipe.GetDefaultObject()->mIngredients.Empty();
		else
			Recipe.GetDefaultObject()->mProduct.Empty();
	}
	
	for(auto i : Ingredient ? RecipeStruct.Ingredients : RecipeStruct.Products)
	{
		bool Found = false;
		if (i.Key.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *i.Key);
			if (Loaded && Loaded->IsChildOf(UFGItemDescriptor::StaticClass()))
			{
				FItemAmount Amount;
				Amount.Amount = i.Value;
				Amount.ItemClass = Loaded;
				auto & Target = Ingredient ? Recipe.GetDefaultObject()->mIngredients : Recipe.GetDefaultObject()->mProduct;
				bool Exists = false;
				for(auto & l : Target)
				{
					if(l.ItemClass == Loaded)
					{
						Exists = true;
						if(l.Amount != Amount.Amount)
						{
							l.Amount = Amount.Amount;
						}
						break;
					}
				}
				if(!Exists)
					Target.Add(Amount);
				Found = true;
			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Finding Item by Path %s failed"), *i.Key);
			}
		}
		else
		{
			for(auto e : Items)
			{	
				TSubclassOf<class UFGItemDescriptor> Desc = e;
				FString S = Desc->GetName();
				FString Tp = i.Key;
				FString DescPre = FString("Desc_").Append(i.Key);
				if (!i.Key.EndsWith("_C"))
				{
					DescPre.Append("_C");
				}
				if(S.Equals(i.Key,ESearchCase::IgnoreCase) || S.Equals(Tp.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
				{
					FItemAmount Amount;
					Amount.Amount = i.Value;
					Amount.ItemClass = Desc;
					auto & Target = Ingredient ? Recipe.GetDefaultObject()->mIngredients : Recipe.GetDefaultObject()->mProduct;
					bool Exists = false;
					for(auto & l : Target)
					{
						if(l.ItemClass == Desc)
						{
							Exists = true;
							if(l.Amount != Amount.Amount)
							{
								l.Amount = Amount.Amount;
							}
							break;
						}
					}
					if(!Exists)
						Target.Add(Amount);
					break;
				}
			}
		}
	}
}

void UCLRecipe::AddBuilders(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, const bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
		Recipe.GetDefaultObject()->mProducedIn.Empty();

	for(auto i : RecipeStruct.BuildIn)
	{

		if(i.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *RecipeStruct.Category);
			if (Loaded && Loaded->IsChildOf(UFGSchematic::StaticClass()) && !Recipe.GetDefaultObject()->mProducedIn.Contains((Loaded)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Loaded);
			else if(!Recipe.GetDefaultObject()->mProducedIn.Contains(Loaded))
			{
				UE_LOG(LogTemp,Error,TEXT("Finding Builder by Path %s failed"), *i);
			}	
		}
		else
		{
			for(auto e : Builders)
			{
				FString S = e->GetName();
				FString A = i;
				FString DescPre = FString("Build_").Append(i);
				if (!DescPre.EndsWith("_C"))
				{
					DescPre.Append("_C");
				}
				if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase) || S.Equals(DescPre, ESearchCase::IgnoreCase))
				{
					TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
					if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
						Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
					break;
				}
			}
			for(auto e : CraftingComps)
			{
				TSubclassOf<class UFGWorkBench> Desc = e;
				if(Desc.GetDefaultObject()->GetName() == i || i == "manual")
				{
					TSoftClassPtr< UObject > Insert = TSoftClassPtr< UObject > (e);
					if(!Recipe.GetDefaultObject()->mProducedIn.Contains((Insert)))
						Recipe.GetDefaultObject()->mProducedIn.Add(Insert);
					break;
				}
			}
			if (i.Contains("BP_BuildGun") || i.Contains("BuildGun") || i.Contains("FGBuildGun"))
			{
				UClass* Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/Equipment/BuildGun/BP_BuildGun.BP_BuildGun_C"));
				if (!Recipe.GetDefaultObject()->mProducedIn.Contains((Class)))
					Recipe.GetDefaultObject()->mProducedIn.Add(Class);
			}
		}
	}
}

void UCLRecipe::AddToSchematicUnlock(const TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct, UContentLib_RecipesSubsystem* Subsystem)
{
	if (!Recipe)
		return;
	for (auto i : RecipeStruct.UnlockedBy)
	{

		if(i.Contains("/"))
		{
			UClass* Loaded = LoadObject<UClass>(nullptr, *RecipeStruct.Category);
			if (Loaded && Loaded->IsChildOf(UFGSchematic::StaticClass()))
				Cast<UCLRecipe>(Recipe.GetDefaultObject())->AddToUnlock(Loaded,Subsystem);
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Finding Schematic by Path %s failed"), *i);
			}	
		}
		else
		{
			for(auto e : Subsystem->Schematics)
			{
				FString S = e->GetName();
				FString A = i;
				if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase))
				{
					Cast<UCLRecipe>(Recipe.GetDefaultObject())->AddToUnlock(e,Subsystem);
				}
			}
		}
	}
};


void UCLRecipe::AddToUnlock(TSubclassOf<UFGSchematic> Schematic , UContentLib_RecipesSubsystem* Subsystem)
{
	AFGSchematicManager * Manager = AFGSchematicManager::Get(Subsystem->GetWorld());
	AFGUnlockSubsystem * Unlock = AFGUnlockSubsystem::Get(Subsystem->GetWorld());
	bool Added = false;
	for(auto f : Schematic.GetDefaultObject()->mUnlocks)
	{
		if(Cast<UFGUnlockRecipe>(f))
		{
			if (!Cast<UFGUnlockRecipe>(f)->mRecipes.Contains(this->GetClass()))
			{
				Cast<UFGUnlockRecipe>(f)->mRecipes.Add(this->GetClass());
				Added = true;
			}
		}
	}
	if (!Added)
	{
		UClass* Class = FindObject<UClass>(ANY_PACKAGE, TEXT("BP_UnlockRecipe_C"), false);
		if (!Class)
		{
			Class = LoadClass<UClass>(nullptr, TEXT("/Game/FactoryGame/Unlocks/BP_UnlockRecipe.BP_UnlockRecipe_C"));
			if (!Class)
			{
				UE_LOG(LogTemp,Fatal,TEXT("CONTENTLIBRECIPES: Couldnt find BP_UnlockRecipe_C wanting to Add to %s"), *Schematic->GetName())
			}
		}
		UFGUnlockRecipe* Object = NewObject<UFGUnlockRecipe>(Schematic.GetDefaultObject(),Class);
		Object->mRecipes.Add(this->GetClass());
		Schematic.GetDefaultObject()->mUnlocks.Add(Object);
	}
	if (Manager && Unlock && Unlock->HasAuthority())
	{
		if(Manager->IsSchematicPurchased(Schematic))
		{
			Unlock->UnlockRecipe(this->GetClass());
		}
	}
}
