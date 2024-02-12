


# this repo is out of date, use this repo instead: https://github.com/Nogg-aholic/ContentLib



# ContentLib Recipes

[Mod Page](https://ficsit.app/mod/5ak7eHymSNw4YN)

## This mod allows Users (and Developers) to easily define custom recipes for any machine, or overwrite recipes from other mods!

Creating your own recipes is as simple as creating a JSON file (schema provided below) and placing it in the `FactoryGame/Configs/Recipes` directory and launching the game.

You can also overwrite any recipe in a very similar manner.

If it doesn't exist, and you want to use it, create it!

This mod's recipe format supports basically everything a recipe Blueprint created in Unreal Engine does because it turns the JSON into such a blueprint at runtime. 

### Questions? Suggestions? Join the [Nog's Mods Discord server](https://discord.gg/kcRmFxn89d) or leave an issue on the [GitHub issues page](https://github.com/Nogg-aholic/ContentLib_Recipes/issues) (slower response time)

## Example:
 Below is an example recipe and a screenshot of it working in game.

```json
{
	"Name": "BioFuel",
	"Ingredients": [
		{
			"Item": "Coal",
			"Amount": 1
		}
	],
	"Products": [
		{
			"Item": "Biofuel",
			"Amount": 10
		}
	],
	"ProducedIn":["ConstructorMk1"],
	"UnlockedBy":["Schematic_1-1"]
}
```

</details>

![Recipe](https://i.imgur.com/ZUl6Mc5.png "Recipe")

## JSON Schema

The format of recipe json files follows a [JSON Schema](https://json-schema.org/), a helpful template that both explains the structure of the file and allows your favorite editors to automatically check your syntax for you. The schema itself is human-readable, so you can also read it to learn the file format.

You can use [Visual Studio Code](https://youtu.be/m30JiCuW42U), [any JetBrains editor](https://www.jetbrains.com/help/idea/json.html#ws_json_schema_add_custom), or more, to get automatic suggestions and see the documentation as you write. Those links go to documentation on how to set it up.

You can view the schema below (may be out of date) or on GitHub [here](https://raw.githubusercontent.com/Nogg-aholic/ContentLib_Recipes/master/FContentLib_Recipe.json) (guaranteed to be up to date).

<details>
<summary> JSON Schema </summary>

```json
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "type": "object",
  "properties": {
    "Name": {
      "type": "string",
      "description": "Recipe Name. If not used first Product is used as Name"
    },
    "Ingredients": {
      "type": "array",
      "items": {
        "properties": {
          "Item": {
            "type": "string",
            "description": "Class Name -> 'Desc_Coal_C' while 'Desc_' & '_C' is not required"
          },
          "Amount": {
            "description": "Amount of Items",
            "type": "integer"
          }
        },
        "required": [ "Item", "Amount" ],
        "type": "object"
      },
      "minItems": 1
    },
    "Products": {
      "type": "array",
      "items": {
        "properties": {
          "Item": {
            "type": "string",
            "description": "Class Name -> 'Desc_Coal_C' while 'Desc_' & '_C' is not required"
          },
          "Amount": {
            "description": "Amount of Items",
            "type": "integer"
          }
        },
        "required": [ "Item", "Amount" ],
        "type": "object"
      },
      "minItems": 1
    },
    "ProducedIn": {
      "type": "array",
      "items": {
        "Item": {
          "type": "string",
          "description": "Class Name -> 'Build_ConstruktorMk1_C' while 'Build_' & '_C' is not required. Alternatively 'manual' can be used to Add this Recipe to all Manual Crafters"
        }
      },
      "minItems": 1
    },
    "UnlockedBy": {
      "type": "array",
      "Item": {
        "type": "string",
        "description": "Class Name -> 'Schematic_1-1_C' while '_C' is not required"
      },
      "minItems": 1
    },
    "ManufacturingDuration": {
      "type": "number",
      "description": "Duration for the Recipe Cycle in Seconds",
      "minimum": 0.0
    },
    "ManualManufacturingMultiplier": {
      "type": "number",
      "description": "Multiplier for the Recipe Cycle in Crafting Components",
      "minimum": 0.0
    },
    "VariablePowerConsumptionFactor": {
      "type": "number",
      "minimum": 0.0
    },
    "VariablePowerConsumptionConstant": {
      "type": "number",
      "minimum": 0.0
    },
    "ManufacturingMenuPriority": {
      "type": "number",
      "minimum": 0.0
    },
    "ClearIngredients": {
      "type": "boolean",
      "description": "Should the Recipe this is used on have its Ingredient Array Cleared before Adding to it? Default is true when Property Ingredients exists, false when not."
    },
    "ClearProducts": {
      "type": "boolean",
      "description": "Should the Recipe this is used on have its Products Array Cleared before Adding to it? Default is true when Property Products exists, false when not."

    },
    "ClearBuilders": {
      "type": "boolean",
      "description": "Should the Recipe this is used on have its Builders Array Cleared before Adding to it? Default is true when Property ProducedIn exists, false when not."
    }
  }
}
```

</details>

## File Patching (Overwrite other recipes)

In addition to creating new recipes, you can also use _ContentLib Recipes_ to modify existing recipes, both base-game recipes and those added by other mods.

The syntax for the recipe is the exact same, but you must put an additional line at the start of the file to specify what recipe to overwrite - the recipe's blueprint path.

You can find this blueprint path from a [reference list](https://github.com/Goz3rr/SatisfactorySaveEditor/blob/master/Reference%20Materials/Recipes.txt), via a tool like [UModel](https://www.gildor.org/en/projects/umodel), or by looking a mod's source code (either online or ingame with ContentInspector).

Below is an example modification of the base game's Biofuel recipe.

```json
//Game/FactoryGame/Recipes/Constructor/Recipe_Biofuel.Recipe_Biofuel_C
{
    "Name": "BioFuel Override",
    "Ingredients": [
        {
            "Item": "Coal",
            "Amount": 1
        }
    ],
    "Products": [
        {
            "Item": "Biofuel",
            "Amount": 10
        }
    ]
}
```

## ContentLib Recipes

This Recipe JSON is what most of your recipes will probably look like:

```json
{
    "$schema": "https://github.com/Nogg-aholic/ContentLib_Recipes/blob/master/FContentLib_Recipes.json",
    "Ingredients": [
        {
            "Item": "Coal",
            "Amount": 1 
        }
    ],
    "Products": [
        {
            "Item": "Coal",
            "Amount": 1
        }
    ],
    "ManufacturingDuration": 1,
    "ProducedIn": [
        "Build_ConstructorMk1"
    ],
    "UnlockedBy": [
        "Schematic_1-1"
    ],
}
```

You can check the JSON schema above to find out what fields are optional and which are required. There are a lot more fields on offer, such as craft time.

For example, you could leave out the `UnlockedBy` field, but the recipe would not be accessible without some other code to register it. An example of a minimal recipe is presented below.

<details>
<summary> Minimum valid recipe (still needs registration) </summary>

```json
{
    "Ingredients": [
        {
            "Item": "Coal",
            "Amount": 1 
        }
    ],
    "Products": [
        {
            "Item": "Coal",
            "Amount": 1
        }
    ],
    "ProducedIn": [
        "Build_ConstructorMk1"
    ],
}
```

</details>

## The inner Workings

Items are resolved by comparing the Name against existing Item ClassNames, the first Match is used.

Watch out for ambiguity with Class Names!

To Find a Name you can UModel or browse the SML Project, alternatively here are some Vanilla Names

* Constructor : Build_ConstructorMk1
* Assembler : Build_AssemblerMk1
* Foundry : Build_FoundryMk1
* Manufactuerer : Build_ManufacturerMk1

Classes additionally have a '_C' Tag when generated by a Blueprint, this as well as the Naming Convention related
"Build" or "Desc" for Items is optional.

### Why do recipe overwrites need the blueprint path?

The Reason is simple:

* This Recipe may not be Loaded when it isn't added by a Registered Schematic at this Point.
* There is no way to "Find" it by name without loading *everything*.

Therefore, this mod uses a Blueprint Path here to reliably load the Recipe to be Patched.

### Why is the blueprint path not part of the Json itself?

* The step of turning the raw text into JSON is skipped when the recipe class fails to load to improve performance.
* Putting the path inside of the JSON would have forced this Mod to do the conversion step earlier.
* Even if it were part of the JSON, it has no actual value for the Recipes themselves, just for the mod figuring out what to overwrite. The path resolves to either a useless nullptr or a valid pointer with no further usefulness.

## For Mod Developers

You can use  _ContentLib Recipes_ to define the recipes for your own mods.

This makes it easy for end users to configure recipes to their own balance preferences, and can help you quickly create recipes if you want to automatically generate them from another source.

You can follow the examples below to load recipes from your mod's own directories, or create and register recipes from strings alone, or whatever you'd like!

Make sure that you list `ContentLib_Recipes` as a required dependency of your own mod, and follow the directions [here](https://docs-dev.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/Adding_Ingame_Mod_Icon.html) to be sure that any json files you include in your mod files get packed into your final mod.

![Guide](https://i.imgur.com/p5TgndI.png "Guide")

