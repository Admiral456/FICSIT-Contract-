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
2. **SML runtime plugin** (`Mods/FICSITContracts`) kompiluje tentýž engine jako součást Unreal modulu a převádí jeho data na `USTRUCT`.
3. **World module** nastaví `bRootModule`, zaregistruje `AFICSITContractsSubsystem` a příkaz `/contracts`.
4. **Server-authoritative subsystem** používá `SpawnOnServer_Replicate`. Autoritativní instance mění board; vlastnosti s `Replicated` se posílají klientům.
5. **Persistence** používá `IFGSaveInterface` a SaveGame vlastnosti pro reputaci, pořadové ID, nabídky, aktivní smlouvy a historii, včetně příznaku zrušení, fází a indexu aktuální fáze.
6. **Serverové příkazy:** SML `AChatCommandSubsystem` je ve zdrojovém kódu SML nastaven na `SpawnOnServer`, takže `/contracts` příkazy jsou vykonávány na autoritativní straně. SML chat-command dokumentace potvrzuje registraci přes `UGameWorldModule::mChatCommands`. Dodávka nejdříve ověří autoritu a hlavní inventář; protože herní `UFGInventoryComponent::Remove()` vrací `void`, adaptér po odečtení znovu načte počet předmětů a do kontraktu připíše pouze pozorovaný rozdíl.
7. **Interakce v 0.1.x je záměrně přes příkazy `/contracts`, `/contracts accept <ID>`, `/contracts deliver <ID>`, `/contracts cancel <ID>` a `/contracts history`. Zrušení aktivního kontraktu odečte 25 % odměny reputace (zaokrouhleno nahoru, minimálně 5 u kladné odměny, nejvýše však aktuální reputace) a zachová dílčí průběh v historii jako `CANCELLED`. Dodávka používá hlavní inventář hráče. Před odečtením se kontroluje autorita, dostupné množství a načtení třídy předmětu. Odečítá se nejvýše chybějící množství.
8. **Testy** kompilují stejný portable C++ engine jako Unreal adapter, ale záměrně neimitují Unreal runtime.

## Rozsah 0.1.3 a vědomá omezení

Verze 0.1.3 rozšiřuje kontraktový systém o sekvenční třífázový Factory Backbone Program. Legacy zakázky bez pole `Stages` se nadále vyhodnocují jako jednofázové a ukládají do původního `Requirements` pole. Projekt stále není kompletním rozšířením s vlastní budovou. Neobsahuje nové assety, proto je `CanContainContent` v `.uplugin` nastaveno na `false`. Jakmile přidáme Dispatch Terminal, modely, hologram, UI widgety a další assety, bude třeba přidat content/Game Feature část podle aktuálního postupu SML 3.12 a vytvořit příslušný `FGGameFeatureData` asset v editoru. Dokumentace [Buildable tutorial](https://docs.ficsit.app/satisfactory-modding/latest/Development/BeginnersGuide/SimpleMod/buildable.html) popisuje, že buildable potřebuje descriptor, actor a hologram, nikoli pouze C++ třídu.

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
