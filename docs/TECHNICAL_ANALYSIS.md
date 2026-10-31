# Technická analýza kompatibility

**Kontrolní datum:** 30. 10. 2026  
**Cíl:** Satisfactory 1.2, Satisfactory Mod Loader 3.12.x

## Ověřený základ

- Vydání SML 3.12.0 výslovně přidalo podporu Satisfactory 1.2 a uvádí aktualizaci hlaviček i stubů na CL#491125. Manifest proto deklaruje minimální `GameVersion` `>=491125` a závislost na SML `^3.12.0`.
- Aktuální modding dokumentace popisuje Satisfactory jako hru postavenou na upraveném Unreal Engine 5.6.1. Běžná instalace Unreal Engine z Epic Games Launcheru není pro kompilaci modu dostačující; je potřeba komunitní/Coffee Stain custom engine a odpovídající Starter Project.
- Dokumentace Satisfactory Save System vyžaduje `IFGSaveInterface`, `ShouldSave() == true` a `UPROPERTY(SaveGame)` pro data, která se mají ukládat. Samotné označení vlastnosti `SaveGame` bez save interface nestačí.
- SML poskytuje `AModSubsystem` s politikou `SpawnOnServer_Replicate`; `UGameWorldModule` umí automaticky registrovat mod subsystémy a chat commands.

Zdroje:

- [SML 3.12.0 release notes](https://github.com/satisfactorymodding/SatisfactoryModLoader/releases/tag/v3.12.0)
- [Required Software](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/dependencies.html)
- [Starter Project Structure](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/StarterProjectStructure.html)
- [Satisfactory Savegame](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/Savegame.html)
- [SML source: ModSubsystem.h](https://github.com/satisfactorymodding/SatisfactoryModLoader/blob/master/Mods/SML/Source/SML/Public/Subsystem/ModSubsystem.h)
- [SML source: GameWorldModule.h](https://github.com/satisfactorymodding/SatisfactoryModLoader/blob/master/Mods/SML/Source/SML/Public/Module/GameWorldModule.h)

## Zvolená architektura 0.1.2

1. **Portable domain engine** (`Source/FICSITContractsCore`) obsahuje generování nabídek, přijímání, částečné dodávky, dokončení, expiraci, penalizované zrušení, reputaci a omezenou historii. Nepoužívá Unreal typy, takže jde sestavit a testovat samostatně.
2. **SML runtime plugin** (`Mods/GameFeatures/FICSITContracts`) kompiluje tentýž engine jako součást Unreal modulu a převádí jeho data na `USTRUCT`.
3. **World module** nastaví `bRootModule`, zaregistruje `AFICSITContractsSubsystem` a příkaz `/contracts`.
4. **Server-authoritative subsystem** používá `SpawnOnServer_Replicate`. Autoritativní instance mění board; vlastnosti s `Replicated` se posílají klientům.
5. **Persistence** používá `IFGSaveInterface` a SaveGame vlastnosti pro reputaci, pořadové ID, nabídky, aktivní smlouvy a historii, včetně příznaku zrušení, fází a indexu aktuální fáze.
6. **Serverové příkazy:** SML `AChatCommandSubsystem` je ve zdrojovém kódu SML nastaven na `SpawnOnServer`, takže `/contracts` příkazy jsou vykonávány na autoritativní straně. SML chat-command dokumentace potvrzuje registraci přes `UGameWorldModule::mChatCommands`. Dodávka nejdříve ověří autoritu a hlavní inventář; protože herní `UFGInventoryComponent::Remove()` vrací `void`, adaptér po odečtení znovu načte počet předmětů a do kontraktu připíše pouze pozorovaný rozdíl.
7. **Interakce v 0.1.x je záměrně přes příkazy `/contracts`, `/contracts accept <ID>`, `/contracts deliver <ID>`, `/contracts cancel <ID>` a `/contracts history`. Zrušení aktivního kontraktu odečte 25 % odměny reputace (zaokrouhleno nahoru, minimálně 5 u kladné odměny, nejvýše však aktuální reputace) a zachová dílčí průběh v historii jako `CANCELLED`. Dodávka používá hlavní inventář hráče. Před odečtením se kontroluje autorita, dostupné množství a načtení třídy předmětu. Odečítá se nejvýše chybějící množství.
8. **Testy** kompilují stejný portable C++ engine jako Unreal adapter, ale záměrně neimitují Unreal runtime.

## Rozsah 0.2.0 a vědomá omezení

Verze 0.2.0 přidává nativní `AFICSITDispatchTerminal`, interakční widget, descriptor, build-gun recepturu, Tier 5 Logistics schematic a owner-bound RCO pro serverově ověřené akce. Legacy zakázky bez pole `Stages` se nadále vyhodnocují jako jednofázové a ukládají do původního `Requirements` pole. Plugin je nyní content-bearing (`CanContainContent=true`) a je umístěn v `Mods/GameFeatures/FICSITContracts`. Zdrojový OBJ/MTL model a editorový skript jsou v repozitáři; skutečné `.uasset` soubory včetně `UFGGameFeatureData` se musí vytvořit/importovat v cílovém SML 3.12 Starter Projectu. Dokumentace [Buildable tutorial](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/SimpleMod/buildable.html) potvrzuje nutnost propojit descriptor, buildable actor a hologram.

Factory Backbone postupuje mezi fázemi Structural Base (2 000 Iron Plates), Reinforcement (500 Reinforced Iron Plates) a Powertrain (150 Motors). Každá fáze přijímá jen své výrobky; celý reputační reward se připíše až po dokončení poslední fáze. Procento postupu je průměrem dokončení fází.

Kontrakty jsou nyní sdílené pro session, nikoli per-player. To je záměr pro první multiplayerový model: jeden společný FICSIT board, jedna autoritativní sada zakázek a replikovaný stav. Samostatné osobní smlouvy lze doplnit později pomocí PlayerState datového komponentu.

Časové limity používají UTC Unix čas. Odpočítávání tedy pokračuje i po vypnutí hry. Pokud bude při testování působit příliš tvrdě, pozdější konfigurační volba přepne na herní čas.

## Co nebylo možné ověřit v tomto prostředí

- Kompilaci pluginu pomocí custom UE 5.6.1 / FactoryGameEditor.
- UHT (Unreal Header Tool) validaci všech UCLASS/USTRUCT deklarací.
- Načtení pluginu v běžící hře se SML 3.12.x.
- Správnost všech vanilla item class path v cílovém asset registry.
- Chování ukládání/replikace v reálném multiplayeru.
- Alpakit staging a instalaci balíčku.

Tyto body jsou záměrně vedené jako herní/build testy; úspěch portable CMake testů se za ně nevydává.


## Dispatch Terminal source implementation (0.2.0)

The plugin now includes `AFICSITDispatchTerminal : AFGBuildable`, a `UFGInteractWidget` subclass, a native `UFGBuildingDescriptor` subclass, a build-gun `UFGRecipe`, a `UFGUnlockRecipe` and a Tier 5 `UFGSchematic` registered by the root `UGameWorldModule`. The actor opens the widget through `AFGHUD::OpenInteractUI`; the widget reads the existing replicated `AFICSITContractsSubsystem` and does not mutate it from the client. This avoids an unsafe client-side write path while a proper owner-valid RCO action layer is pending.

The source mesh is OBJ/MTL, not a cooked `.uasset`. SML 3.12 requires a Game Feature Data asset for content-bearing mods; the Unreal Editor utility imports the mesh/icon and creates that asset. The plugin is marked `CanContainContent=true` and is located in `Mods/GameFeatures/FICSITContracts`. Custom UE compilation, Alpakit packaging, build-gun placement, save/load and multiplayer interaction remain unverified.


### Terminal action security

The terminal widget obtains the RCO from its owning `AFGPlayerController`. `UFICSITContractsGameInstanceModule` registers `UFICSITContractsRemoteCallObject` through SML's `UGameInstanceModule::RemoteCallObjects`. The three server RPCs resolve `GetOwnerPlayerCharacter()` and `GetOwnerPlayerController()`, verify that the character is controlled by that exact controller and has server authority, then call the existing authoritative `AFICSITContractsSubsystem` methods. Delivery passes only the owning server-side character to `DeliverFromInventory`, so the host validates and removes items from that player's main inventory. The UI subscribes to `OnContractBoardChanged`; the subsystem's replication notify broadcasts the delegate on clients to refresh the board. This is source-level integration and still needs UHT/UE and multiplayer validation.

## Endgame progression core (0.3.0 foundation)

The portable domain layer now also contains `EndgameProgression` and `EndgameProgressionSnapshot`. The state machine models the ordered sequence from vanilla Space Elevator Phase V through Terminal I–VI and the three custom Space Elevator extensions VI–VIII. Its public API separates read-only gate checks from state-changing completion calls, maps catalog unlock IDs to their terminal chapter, and normalizes malformed snapshots so impossible later-phase flags are cleared.

The state engine is compiled into the standalone CMake test library and is also included into the SML runtime module through `FICSITContractsCore.cpp`. The wrapper lives under the module's `Source/FICSITContracts/Private` tree so Unreal Build Tool discovers the translation unit. CMake compiles the same wrapper as an object-only target to verify the relative source includes on Linux and Windows without requiring the proprietary UE SDK.

**Boundary of this milestone:** the state machine is not yet connected to `AFICSITContractsSubsystem` SaveGame/replication properties, the Space Elevator completion events, real schematic purchases, material costs or terminal research widgets. Those adapters must call this state machine only after server-side validation and verified item consumption. The new JSON catalog remains design data until real Satisfactory assets and unlock objects are registered.

