


#include "ContentLib_RecipesSubsystem.h"



#include "FGSchematic.h"
#include "FGWorkBench.h"
#include "Buildables/FGBuildableFactory.h"

void UContentLib_RecipesSubsystem::FillLoadedClasses()
{
	GetDerivedClasses(UFGItemDescriptor::StaticClass(), Items, true);
	GetDerivedClasses(AFGBuildableFactory::StaticClass(), Builders, true);
	GetDerivedClasses(UFGWorkBench::StaticClass(), CraftingComps, true);
	GetDerivedClasses(UFGSchematic::StaticClass(), Schematics, true);
}
