

#pragma once

#include "CoreMinimal.h"

#include "CLRecipe.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CLRecipeBPFLib.generated.h"





/**
 * 
 */
UCLASS()
class CONTENTLIB_RECIPES_API UCLRecipeBPFLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	// IO Functions originally by Rama!
	template <class FunctorType>
    class PlatformFileFunctor : public IPlatformFile::FDirectoryVisitor	//GenericPlatformFile.h
    {
	public:

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			return Functor(FilenameOrDirectory, bIsDirectory);
		}

		PlatformFileFunctor(FunctorType&& FunctorInstance)
            : Functor(MoveTemp(FunctorInstance))
		{
		}

	private:
		FunctorType Functor;
    };

	template <class Functor>
    static PlatformFileFunctor<Functor> MakeDirectoryVisitor(Functor&& FunctorInstance)
	{
		return PlatformFileFunctor<Functor>(MoveTemp(FunctorInstance));
	}

	UFUNCTION(BlueprintCallable)
	static void WriteStringToFile(FString Path, FString resultString, bool Relative);
	UFUNCTION(BlueprintCallable)
	static bool LoadStringFromFile(FString& String, FString Path, bool Relative);
	UFUNCTION(BlueprintCallable)
	static bool GetDirectoriesInPath(const FString& FullPathOfBaseDir, TArray<FString>& DirsOut, const FString& NotContainsStr , bool Recursive, const FString& ContainsStr);
	UFUNCTION(BlueprintCallable)
	static void String_Sort(UPARAM(ref) TArray <FString>& Array_To_Sort, bool Descending, bool FilterToUnique,  TArray <FString>& Sorted_Array);
	UFUNCTION(BlueprintCallable)
	static bool GetFilesInPath(const FString& FullPathOfBaseDir, TArray<FString>& FilenamesOut, bool Recursive, const FString& FilterByExtension);

};
