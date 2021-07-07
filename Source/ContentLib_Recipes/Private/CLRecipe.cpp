


#include "CLRecipe.h"



#include "FGSchematic.h"
#include "FGWorkBench.h"
#include "Buildables/FGBuildableFactory.h"
#include "Registry/ModContentRegistry.h"
#include "Unlocks/FGUnlockRecipe.h"

void UCLRecipe::InitFromStruct(FContentLib_Recipe RecipeStruct, TSubclassOf<class UFGRecipe> Recipe,bool ClearIngredients,bool ClearProducts,bool ClearBuilders)
{
	if (!Recipe)
		return;
	UFGRecipe *  CDO = Recipe.GetDefaultObject();
	if (!CDO)
		return;
	TArray<UClass*> Arr;
	TArray<UClass*> Builders;
	TArray<UClass*> CraftingComps;
	TArray<UClass*> Schematics;
	GetDerivedClasses(UFGItemDescriptor::StaticClass(), Arr, true);
	GetDerivedClasses(AFGBuildableFactory::StaticClass(), Builders, true);
	GetDerivedClasses(UFGWorkBench::StaticClass(), CraftingComps, true);
	GetDerivedClasses(UFGSchematic::StaticClass(), Schematics, true);
	if (RecipeStruct.Name != "")
	{
		CDO->mDisplayName = FText::FromString(RecipeStruct.Name);
		CDO->mDisplayNameOverride = true;
	}
	AddProductOrIngredient(Recipe,RecipeStruct,Arr,true, ClearIngredients);
	AddProductOrIngredient(Recipe,RecipeStruct,Arr,false, ClearProducts);
	AddBuilders(Recipe,RecipeStruct,Builders,CraftingComps, ClearBuilders);
	AddToSchematicUnlock(Recipe,RecipeStruct,Schematics);
	if(RecipeStruct.ManufacturingDuration != 1)
		CDO->mManufactoringDuration = RecipeStruct.ManufacturingDuration;
	if (RecipeStruct.mVariablePowerConsumptionConstant != 0)
		CDO->mVariablePowerConsumptionConstant = RecipeStruct.mVariablePowerConsumptionConstant;
	if (RecipeStruct.mVariablePowerConsumptionFactor != 1)
		CDO->mVariablePowerConsumptionFactor = RecipeStruct.mVariablePowerConsumptionFactor;
}

void UCLRecipe::AddProductOrIngredient(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Items, const bool Ingredient, bool ClearFirst)
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

void UCLRecipe::AddBuilders(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Builders,TArray<UClass*> CraftingComps, bool ClearFirst)
{
	if (!Recipe)
		return;
	if (ClearFirst)
		Recipe.GetDefaultObject()->mProducedIn.Empty();

	for(auto i : RecipeStruct.BuildIn)
	{
		for(auto e : Builders)
		{
			FString S = e->GetName();
			FString A = i;
			if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase))
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
			if(Desc.GetDefaultObject()->GetName() == i)
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

void UCLRecipe::AddToSchematicUnlock(TSubclassOf<class UFGRecipe> Recipe,FContentLib_Recipe RecipeStruct,TArray<UClass*> Schematics)
{
	if (!Recipe)
		return;
	for (auto i : RecipeStruct.UnlockedBy)
	{
		for(auto e : Schematics)
		{
			FString S = e->GetName();
			FString A = i;
			if (S.Equals(i, ESearchCase::IgnoreCase) || S.Equals(A.Append("_C"), ESearchCase::IgnoreCase))
			{
				TSubclassOf<UFGSchematic> Schematic = e;
				for(auto f : Schematic.GetDefaultObject()->mUnlocks)
				{
					if(Cast<UFGUnlockRecipe>(f))
					{
						Cast<UFGUnlockRecipe>(f)->mRecipes.Add(Recipe);
					}
				}
			}
		}
	}
};

TSubclassOf<UCLRecipe> UCLRecipe::CreateDynRecipe(FString Name)
{
	if (Name == "" || FindObject<UClass>(ANY_PACKAGE, *Name, false) || FindObject<UClass>(ANY_PACKAGE, *Name.Append("_C"), false))
		return nullptr;
    const EClassFlags ParamsClassFlags = CLASS_Native | CLASS_MatchedSerializers;
	UClass * ParentClass = UCLRecipe::StaticClass();
    //Code below is taken from GetPrivateStaticClassBody
    //Allocate memory from ObjectAllocator for class object and call class constructor directly
    UClass* ConstructedClassObject = (UClass*)GUObjectAllocator.AllocateUObject(sizeof(UDynamicClass), alignof(UDynamicClass), true);
    ::new (ConstructedClassObject)UDynamicClass(
    	EC_StaticConstructor,
    	*Name,
    	ParentClass->GetStructureSize(),
    	ParentClass->GetMinAlignment(),
    	CLASS_Intrinsic,
    	CASTCLASS_None,
    	UObject::StaticConfigName(),
    	EObjectFlags(RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet),
    	ParentClass->ClassConstructor,
    	ParentClass->ClassVTableHelperCtorCaller,
    	ParentClass->ClassAddReferencedObjects, nullptr);
    
    //Set super structure and ClassWithin (they are required prior to registering)
    FCppClassTypeInfoStatic TypeInfoStatic = { false };
    ConstructedClassObject->SetSuperStruct(ParentClass);
    ConstructedClassObject->ClassWithin = UObject::StaticClass();
    ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#if WITH_EDITOR
    //Field with cpp type info only exists in editor, in shipping SetCppTypeInfoStatic is empty
    ConstructedClassObject->SetCppTypeInfoStatic(&TypeInfoStatic);
#endif
    //Register pending object, apply class flags, set static type info and link it
    ConstructedClassObject->RegisterDependencies();

    ConstructedClassObject->DeferredRegister(UDynamicClass::StaticClass(), TEXT("/ContentLib_Recipes/"), *Name);

    //Mark class as Constructed and perform linking
    ConstructedClassObject->ClassFlags |= (EClassFlags)(ParamsClassFlags | CLASS_Constructed);
    ConstructedClassObject->AssembleReferenceTokenStream(true);
    ConstructedClassObject->StaticLink();

    //Make sure default class object is initialized
    ConstructedClassObject->GetDefaultObject();
	return ConstructedClassObject;
}
