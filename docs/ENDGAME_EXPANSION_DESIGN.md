# FICSIT Endgame Expansion – technický návrh

**Stav:** návrhový katalog připravený pro implementaci (není zatím aktivní herní obsah)  
**Cíl kompatibility:** Satisfactory 1.2 / SML 3.12.x / custom UE 5.6.1  
**Katalog:** `Data/Progression/EndgameProgression.json`

## Rozsah rozšíření

Toto rozšíření navazuje na původní pátou fázi Space Elevatoru. Zachovává původní vanilla milníky a přidává šest výzkumných kapitol Dispatch Terminalu a tři další projekty Space Elevatoru.

- 6 nových kapitol Dispatch Terminalu.
- 3 nové Space Elevator projekty (VI–VIII).
- 4 nové suroviny.
- 96 plánovaných odemykatelných technologií.
- 54 návrhových výrobních receptů.
- Samostatné kategorie pro budovy, těžbu, logistiku, energii, automatizaci, zbraně, munici, osobní vybavení, vozidla, blueprinty a obranu.

**Důležité:** položky v tomto dokumentu a JSON katalogu jsou návrhy. Zatím nejde o hotové zbraně, blueprinty, budovy, recepty, rudy ani funkce ve hře.

## Postup odemykání

| Dokončený projekt | Zpřístupněné výzkumné kapitoly |
|---|---|
| Vanilla Space Elevator Phase V | Terminal I a II |
| Nový Space Elevator Phase VI | Terminal III a IV |
| Nový Space Elevator Phase VII | Terminal V |
| Nový Space Elevator Phase VIII | Terminal VI |

Každá kapitola má vlastní odemykatelné položky a recepty. Odemčený obsah se má přidávat přes skutečné Satisfactory schematics a jejich unlock objekty; samotné položky v JSON souboru zatím nic ve hře neodemknou.

## Blueprinty – oprava návaznosti

Blueprint Designer Mk1, Mk2 a Mk3 jsou součástí základní hry. Proto je nebudeme znovu přidávat. Rozšíření navazuje:

| Nový návrh | Kapitola | Návrhový pracovní prostor | Účel |
|---|---:|---:|---|
| Blueprint Designer Mk4 | I | 64 × 64 × 64 m | Velké výrobní moduly |
| Blueprint Designer Mk5 | IV | 80 × 80 × 80 m | Kompletní tovární bloky a pokročilé sestavy |

Rozměry jsou návrhové cíle, nikoli ověřené limity Blueprint systému. Před implementací je nutný prototyp, který ověří hranice stavění, ukládání, načítání, vkládání nižších blueprintů, multiplayer a kompatibilitu se save soubory.

## Nové suroviny

| Surovina | Vzhled | Výrobní řetězec | Hlavní využití |
|---|---|---|---|
| **Ferravium Ore** | Černomodrá kovová hornina s oranžovými žilami a jemným kovovým leskem. | Ferravium Concentrate → Ferravium Ingot → Titan Alloy | Těžba, konstrukce, nové střelivo a těžké průmyslové sestavy |
| **Cryonite Ore** | Průsvitné tyrkysové krystaly s ledovým povrchem a světelným jádrem. | Cryonite Powder → Cryogenic Slurry → Refined Cryonite Crystal → Superconductive Coil | Kryogenní chlazení, supravodiče, kryo munice a energetické regulátory |
| **Asterite Ore** | Tmavá fialová geoda s magentovými a tyrkysovými krystalickými ploškami. | Asterite Powder → Asterite Wafer → Photonic Fiber → Photonic Processor | Optická elektronika, fotonická komunikace a kvantové řídicí obvody |
| **Gravionite Ore** | Černofialový nerost s pulzujícím jádrem a drobnými levitujícími úlomky. | Gravionite Shard → Stabilized Gravionite → Gravitic Core → Quantum Logistics Engine | Gravitační logistika, stabilizátory, energetika a orbitální systémy |

### Pravidla pro ložiska

Nové rudy potřebují skutečné resource node descriptory, vizuální modely, těžební interakci, skener a správné save/load chování. Veřejná modding dokumentace upozorňuje, že práce s World Partitionem základní mapy není aktuálně podporována běžným modding toolkit workflow. Dynamicky vytvořená ložiska se navíc nezobrazí ve skeneru automaticky, dokud se neobnoví interní shluky skeneru.

Prototypování proto musí začít ověřením podporované cesty pro resource nodes. Pokud nebude spolehlivá, první vydání má použít podporované deposit/extraction point objekty nebo jiné ověřené umístění – ne slibovat globální world generation.

## Šest kapitol Dispatch Terminalu

### 1. Frontier Survey – Průzkum hranic

**Podmínka:** Vanilla Space Elevator Phase V complete

Ferravium, průzkum ložisek, první endgame vybavení, zbraň a Blueprint Designer Mk4.

**Odemykatelné technologie:**

- **Building:** Geological Survey Station, Deep-Core Extractor, Ore Washing Plant, Ferravium Smelter, Field Workshop, Survey Beacon Mk2, Deep-Core Storage
- **Weapon:** FICSIT Precision Rebar Rifle
- **Ammo:** Ferravium Darts
- **Equipment:** Pathfinder Visor, Portable Miner Mk2
- **Vehicle:** Survey Drone
- **Blueprint:** Blueprint Designer Mk4
- **Scanner:** Ferravium Scanner Upgrade

**Výrobní recepty:** `RCP_FERRAVIUM_CONCENTRATE`, `RCP_FERRAVIUM_INGOT`, `RCP_FERRAVIUM_PLATE`, `RCP_SURVEY_COMPUTER`, `RCP_FERRAVIUM_DARTS`, `RCP_PRECISION_REBAR_RIFLE`, `RCP_BLUEPRINT_MK4`

### 2. Advanced Metallurgy – Pokročilá metalurgie

**Podmínka:** Terminal Phase I complete

Titanové slitiny, těžká výroba, Mk7 logistika, automatická obrana a Exosuit.

**Implementovaný Phase II source slice:** nativní Titan Alloy Ingot a Titan Alloy Beam descriptory s oddělenými OBJ meshi a ikonami. Schéma Advanced Metallurgy je registrované a po potvrzeném nákupu označí Terminal II jako dokončený. Zatím jsou přidány dva bootstrap recepty ve vanilla Foundry Mk1: `RCP_TITAN_ALLOY` (60 Ferravium Ingots + 60 Steel Ingots → 60 Titan Alloy Ingots / 8 s) a `RCP_TITAN_BEAM_BOOTSTRAP` (30 Titan Alloy Ingots + 60 Steel Pipes → 30 Titan Alloy Beams / 12 s). Bootstrap je oddělen od návrhového receptu `RCP_TITAN_BEAM`, který v cílovém řetězci zůstává receptem Industrial Press (30 Titan Alloy Ingots → 15 Titan Alloy Beams / 8 s). Dedicated Advanced Alloy Foundry, Industrial Press a ostatní fáze výrobního řetězce zůstávají plánované.

**Odemykatelné technologie:**

- **Building:** Advanced Alloy Foundry, Industrial Press, Heavy-Duty Assembler, Industrial Storage Mk2, Industrial Fluid Buffer Mk2, Reinforced Build Set
- **Miner:** Miner Mk4
- **Logistics:** Conveyor Belt Mk7, Conveyor Lift Mk7, Smart Splitter Mk2, Pipeline Mk3
- **Power:** High-Voltage Substation, Power Storage Mk2
- **Weapon:** Heavy Rebar Repeater
- **Ammo:** Hardened Rebar Ammunition
- **Equipment:** FICSIT Exosuit Mk1
- **Vehicle:** Industrial Garage

**Výrobní recepty:** `RCP_TITAN_ALLOY`, `RCP_TITAN_BEAM`, `RCP_REINFORCED_TITAN_FRAME`, `RCP_HEAVY_ROTOR`, `RCP_HARDENED_AMMO`, `RCP_FERRAVIUM_STRUCTURAL_MATRIX`, `RCP_DEEP_CORE_EXTRACTOR_MODULE`, `RCP_ADVANCED_SURVEY_COMPUTER`, `RCP_HEAVY_REBAR_REPEATER`, `RCP_EXOSUIT_MK1`

### 3. Cryogenic Systems – Kryogenní systémy

**Podmínka:** Space Elevator Phase VI complete

Cryonite, chlazení, supravodiče, kryo výbava, nová zbraň a palivové systémy.

**Odemykatelné technologie:**

- **Building:** Cryogenic Processing Facility, Cryogenic Refinery, Gas Liquefaction Unit, Superconductive Assembler, Industrial Cooling Tower, Fluid Buffer Mk3
- **Logistics:** Cryogenic Pump Mk3
- **Power:** Thermal Energy Storage
- **Automation:** Cooling Loop Controller
- **Weapon:** Cryo Projector
- **Ammo:** Cryogenic Charge, Cryo Grenade
- **Equipment:** Cryo Suit Mk1, Jetpack Mk2

**Výrobní recepty:** `RCP_CRYONITE_POWDER`, `RCP_CRYOGENIC_SLURRY`, `RCP_REFINED_CRYONITE`, `RCP_SUPERCONDUCTIVE_COIL`, `RCP_CRYO_ENGINE`, `RCP_CRYO_AMMO`, `RCP_CRYOGENIC_POWER_REGULATOR`, `RCP_CRYO_PROJECTOR`, `RCP_CRYO_GRENADE`, `RCP_CRYO_SUIT`

### 4. Photonic Manufacturing – Fotonická výroba

**Podmínka:** Terminal Phase III complete and Space Elevator Phase VI complete

Asterite, optická elektronika, Blueprint Designer Mk5, fotonická puška a automatizované řízení.

**Odemykatelné technologie:**

- **Building:** Photonic Fabricator, Optical Glass Furnace, Quantum Circuit Assembler, Photonic Communication Tower, Holographic Display
- **Miner:** Miner Mk5
- **Logistics:** Conveyor Belt Mk8, Conveyor Lift Mk8, Smart Splitter Mk3, Drone Port Mk2
- **Automation:** Factory Monitoring Terminal, Automated Factory Alarm, Programmable Logistics Controller
- **Weapon:** Photonic Rifle
- **Ammo:** Photonic Energy Cells
- **Equipment:** Hoverpack Mk2
- **Vehicle:** Survey Drone Mk2
- **Blueprint:** Blueprint Designer Mk5

**Výrobní recepty:** `RCP_ASTERITE_WAFER`, `RCP_OPTICAL_GLASS`, `RCP_PHOTONIC_FIBER`, `RCP_PHOTONIC_PROCESSOR`, `RCP_QUANTUM_ROUTING_ARRAY`, `RCP_PHOTONIC_CELL`, `RCP_PHOTONIC_ROUTING_ARRAY`, `RCP_QUANTUM_COMMUNICATION_CORE`, `RCP_PHOTONIC_RIFLE`, `RCP_BLUEPRINT_MK5`

### 5. Gravitic Engineering – Gravitační technologie

**Podmínka:** Space Elevator Phase VII complete

Gravionite, gravitační doprava, energetika, osobní štít a gravitační zbraň.

**Odemykatelné technologie:**

- **Building:** Gravitic Stabilization Reactor, Gravitic Assembler, Mass Compression Unit
- **Miner:** Miner Mk6
- **Logistics:** Conveyor Belt Mk9, Conveyor Lift Mk9, Gravitic Freight Elevator, Quantum Merger
- **Power:** Quantum Accumulator, Gravitic Power Plant
- **Weapon:** Gravitic Launcher
- **Ammo:** Gravity Grenade, Repulsion Grenade
- **Equipment:** Gravity Boots, Personal Shield Generator
- **Defense:** Automated Sentry Mk2

**Výrobní recepty:** `RCP_GRAVIONITE_SHARD`, `RCP_STABILIZED_GRAVIONITE`, `RCP_GRAVITIC_CORE`, `RCP_GRAVITIC_STABILIZER`, `RCP_QUANTUM_LOGISTICS_ENGINE`, `RCP_GRAVITIC_TRANSIT_CORE`, `RCP_ORBITAL_LOGISTICS_MODULE`, `RCP_GRAVITIC_LAUNCHER`, `RCP_GRAVITY_GRENADE`, `RCP_REPULSION_GRENADE`, `RCP_GRAVITY_BOOTS`, `RCP_PERSONAL_SHIELD`, `RCP_ORBITAL_PREASSEMBLY_PROCESSOR`

### 6. Interplanetary Logistics – Meziplanetární logistika

**Podmínka:** Space Elevator Phase VIII complete

Orbitální logistika, kvantové relé, vzdálená správa továren, pokročilý blueprint management a podpůrný systém.

**Odemykatelné technologie:**

- **Building:** Orbital Freight Hub, Automated Loading Dock, Quantum Storage Container, Orbital Assembly Plant
- **Logistics:** Quantum Cargo Relay, Drone Port Mk3
- **Power:** Quantum Power Relay, Orbital Power Distributor
- **Automation:** Logistics Control Center, FICSIT Global Dispatch, Train Logistics Controller, Remote Factory Control
- **Weapon:** FICSIT Orbital Support Beacon
- **Equipment:** FICSIT Field Fabricator Mk2, Personal Logistics Suit
- **Blueprint:** Blueprint Management Terminal
- **Defense:** FICSIT Security Drone

**Výrobní recepty:** `RCP_QUANTUM_CARGO_CORE`, `RCP_QUANTUM_STORAGE_MATRIX`, `RCP_COMMAND_PROCESSOR`, `RCP_ORBITAL_ASSEMBLY_FRAME`

## Nové zbraně a munice

| Zbraň / vybavení | Kapitola | Munice nebo zdroj | Zamýšlená role |
|---|---:|---|---|
| FICSIT Precision Rebar Rifle | I | Ferravium Darts | Přesná jednoranová zbraň s delším dostřelem |
| Heavy Rebar Repeater | II | Hardened Rebar Ammunition | Rychlá průmyslová zbraň na kratší vzdálenost |
| Cryo Projector | III | Cryogenic Charge | Zpomalení nepřátel, nikoli okamžité zmrazení na neurčito |
| Photonic Rifle | IV | Photonic Energy Cells | Přesná energetická zbraň, standardní a nabitý výstřel |
| Gravitic Launcher | V | Gravitic Core / energie | Omezené gravitační přitažení nebo odpuzující impuls |
| FICSIT Orbital Support Beacon | VI | Kvantové a orbitální komponenty | Označí cíl pro omezenou podporu dronu |

Granáty: Cryo Grenade, Gravity Grenade a Repulsion Grenade. Zbraně musí mít samostatně ověřené poškození, kadenci, animace, inventář, munici, cooldowny a síťovou replikaci. Obranné věže a drony mají navíc závislost na spolehlivém target AI systému.

## Nové osobní vybavení

- Pathfinder Visor a FICSIT Scanner Mk2.
- FICSIT Exosuit Mk1.
- Cryo Suit Mk1.
- Jetpack Mk2 a Hoverpack Mk2.
- Gravity Boots.
- Personal Shield Generator.
- FICSIT Field Fabricator Mk2.
- Personal Logistics Suit.

Vybavení musí respektovat existující equipment sloty. Pokud hra nepodporuje nový samostatný slot, bude nutné zvolit kompatibilní variantu existujícího slotu nebo modulární upgrade – ne přepisovat inventář hráče nekompatibilním způsobem.

## Nové tovární a logistické systémy

### Výroba a zpracování

- Geological Survey Station, Deep-Core Extractor, Ore Washing Plant a Ferravium Smelter.
- Advanced Alloy Foundry, Industrial Press a Heavy-Duty Assembler.
- Cryogenic Processing Facility, Cryogenic Refinery a Gas Liquefaction Unit.
- Photonic Fabricator, Optical Glass Furnace a Quantum Circuit Assembler.
- Gravitic Stabilization Reactor, Gravitic Assembler a Mass Compression Unit.
- Orbital Assembly Plant a Orbital Freight Hub.

### Logistika a skladování

- Conveyor Mk7/Mk8/Mk9 a odpovídající lifts.
- Smart Splitter Mk2 a Mk3.
- Industrial Storage Mk2 a Fluid Buffer Mk2/Mk3.
- Pipeline Mk3 a Cryogenic Pump Mk3.
- Drone Port Mk2/Mk3, Train Logistics Controller a Gravitic Freight Elevator.
- Quantum Merger, Quantum Cargo Relay a Quantum Storage Container.

### Energie a automatizace

- High-Voltage Substation, Power Storage Mk2, Thermal Energy Storage.
- Quantum Accumulator, Gravitic Power Plant, Quantum Power Relay.
- Factory Monitoring Terminal, Automated Factory Alarm, Cooling Loop Controller.
- Logistics Control Center, FICSIT Global Dispatch a Remote Factory Control.

## Návrhové hodnoty a recepty

Všechny hodnoty jsou zatím cílové hodnoty pro balancování. Základní hra má standardní Conveyor Belt Mk6 s kapacitou 1 200 předmětů/min; nový výkon se musí testovat v herním enginu.

| Technologie | Návrhová kapacita / hodnota |
|---|---:|
| Conveyor Mk7 | 1 800 předmětů/min |
| Conveyor Mk8 | 2 700 předmětů/min |
| Conveyor Mk9 | 3 600 předmětů/min |
| Miner Mk4 | 1 200/min na normálním uzlu |
| Miner Mk5 | 1 800/min na normálním uzlu |
| Miner Mk6 | 2 400/min na normálním uzlu |

### Space Elevator VI – Frontier Expansion

### Space Elevator VII – Frontier Expansion

Odemkne terminalové kapitoly: 3 a 4.

| Požadovaný díl | Počet |
|---|---:|
| Ferravium Structural Matrix | 1,500 |
| Deep Core Extractor Module | 800 |
| Titan Alloy Beam | 1,200 |
| Advanced Survey Computer | 400 |

### Space Elevator VIII – Planetary Network

Odemkne terminalové kapitoly: 5.

| Požadovaný díl | Počet |
|---|---:|
| Cryogenic Power Regulator | 1,200 |
| Photonic Routing Array | 800 |
| Superconductive Coil | 1,500 |
| Quantum Communication Core | 400 |

### Space Elevator undefined – Orbital Assembly

Odemkne terminalové kapitoly: 6.

| Požadovaný díl | Počet |
|---|---:|
| Gravitic Transit Core | 1,000 |
| Orbital Logistics Module | 600 |
| Quantum Logistics Engine | 300 |
| Orbital Preassembly Processor | 100 |

## Recepty – výrobní řetězce

Všechny recepty mají jednoznačné ID v JSON katalogu. Počty jsou na jeden výrobní cyklus; časy cyklu jsou návrhové sekundy. Vanilla identifikátory jsou logické aliasy a nesmí se použít jako skutečné class path, dokud se neověří v asset registry konkrétní verze hry.

### Kapitola 1 – Frontier Survey

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_FERRAVIUM_CONCENTRATE` | Refinery (prototypové praní) | Ferravium Ore × 60<br>Vanilla Water × 30 | Ferravium Concentrate × 45 | 12 s |
| `RCP_FERRAVIUM_INGOT` | Ferravium Smelter | Ferravium Concentrate × 45<br>Vanilla Coal × 30 | Ferravium Ingot × 30 | 10 s |
| `RCP_FERRAVIUM_RECOVERY` | Vanilla Oil Refinery | Bauxite Ore × 120<br>Coal × 30<br>Water × 60 | Ferravium Ore × 60 | 18 s |
| `RCP_FERRAVIUM_SMELTER_BUILD` | Build Gun | Encased Industrial Beam × 30<br>Steel Pipe × 80<br>Motor × 20<br>Computer × 10<br>Cable × 100 | Ferravium Smelter × 1 | 0 s |
| `RCP_FERRAVIUM_PLATE` | Constructor | Ferravium Ingot × 30 | Ferravium Plate × 20 | 6 s |
| `RCP_SURVEY_COMPUTER` | Assembler | Vanilla Computer × 2<br>Ferravium Plate × 4 | Survey Computer × 1 | 12 s |
| `RCP_FERRAVIUM_DARTS` | Field Workshop | Ferravium Ingot × 2<br>Vanilla Iron Rod × 2 | Ferravium Dart × 8 | 5 s |
| `RCP_PRECISION_REBAR_RIFLE` | Field Workshop | Ferravium Plate × 12<br>Vanilla Rebar Gun × 1<br>Survey Computer × 1 | Precision Rebar Rifle × 1 | 30 s |
| `RCP_BLUEPRINT_MK4` | Equipment Workshop | Ferravium Plate × 20<br>Vanilla Computer × 10<br>Vanilla Heavy Modular Frame × 8 | Blueprint Designer Mk4 × 1 | 45 s |

### Kapitola 2 – Advanced Metallurgy

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_TITAN_ALLOY` | Advanced Alloy Foundry | Ferravium Ingot × 60<br>Vanilla Steel Ingot × 60 | Titan Alloy Ingot × 60 | 8 s |
| `RCP_TITAN_BEAM` | Industrial Press | Titan Alloy Ingot × 30 | Titan Alloy Beam × 15 | 8 s |
| `RCP_REINFORCED_TITAN_FRAME` | Heavy-Duty Assembler | Titan Alloy Beam × 8<br>Vanilla Heavy Modular Frame × 2 | Reinforced Titan Frame × 2 | 24 s |
| `RCP_HEAVY_ROTOR` | Heavy-Duty Assembler | Titan Alloy Ingot × 12<br>Vanilla Rotor × 6<br>Vanilla Motor × 2 | Heavy Industrial Rotor × 2 | 18 s |
| `RCP_HARDENED_AMMO` | Assembler | Ferravium Ingot × 4<br>Vanilla Iron Rod × 4 | Hardened Rebar × 12 | 6 s |
| `RCP_FERRAVIUM_STRUCTURAL_MATRIX` | Heavy-Duty Assembler | Ferravium Plate × 12<br>Titan Alloy Beam × 6<br>Reinforced Titan Frame × 2 | Ferravium Structural Matrix × 2 | 24 s |
| `RCP_DEEP_CORE_EXTRACTOR_MODULE` | Heavy-Duty Assembler | Deep Core Module × 4<br>Heavy Industrial Rotor × 2<br>Titan Alloy Beam × 4 | Deep Core Extractor Module × 1 | 24 s |
| `RCP_ADVANCED_SURVEY_COMPUTER` | Assembler | Survey Computer × 2<br>Titan Alloy Ingot × 4<br>Vanilla Circuit Board × 2 | Advanced Survey Computer × 1 | 24 s |
| `RCP_HEAVY_REBAR_REPEATER` | Heavy-Duty Assembler | Precision Rebar Rifle × 1<br>Heavy Industrial Rotor × 2<br>Titan Alloy Beam × 4 | Heavy Rebar Repeater × 1 | 35 s |
| `RCP_EXOSUIT_MK1` | Equipment Workshop | Reinforced Titan Frame × 2<br>Titan Alloy Plate × 8<br>Vanilla Motor × 4 | Exosuit Mk1 × 1 | 30 s |

### Kapitola 3 – Cryogenic Systems

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_CRYONITE_POWDER` | Crusher | Cryonite Ore × 60 | Cryonite Powder × 45 | 6 s |
| `RCP_CRYOGENIC_SLURRY` | Cryogenic Refinery | Cryonite Powder × 45<br>Vanilla Nitrogen Gas × 30<br>Vanilla Water × 30 | Cryogenic Slurry × 30 | 10 s |
| `RCP_REFINED_CRYONITE` | Cryogenic Refinery | Cryogenic Slurry × 30 | Refined Cryonite Crystal × 20 | 10 s |
| `RCP_SUPERCONDUCTIVE_COIL` | Superconductive Assembler | Refined Cryonite Crystal × 6<br>Vanilla Copper Ingot × 12 | Superconductive Coil × 4 | 12 s |
| `RCP_CRYO_ENGINE` | Superconductive Assembler | Superconductive Coil × 2<br>Vanilla Motor × 2<br>Cooling Cartridge × 4 | Cryogenic Engine × 1 | 20 s |
| `RCP_CRYO_AMMO` | Assembler | Cryogenic Slurry × 2<br>Vanilla Iron Plate × 2 | Cryo Charge × 4 | 8 s |
| `RCP_CRYOGENIC_POWER_REGULATOR` | Superconductive Assembler | Cryogenic Engine × 2<br>Superconductive Coil × 4<br>Photonic Processor × 1 | Cryogenic Power Regulator × 1 | 30 s |
| `RCP_CRYO_PROJECTOR` | Equipment Workshop | Cryogenic Engine × 2<br>Superconductive Coil × 2<br>Titan Alloy Plate × 4 | Cryo Projector × 1 | 35 s |
| `RCP_CRYO_GRENADE` | Assembler | Cryo Charge × 2<br>Vanilla Aluminum Casing × 1 | Cryo Grenade × 1 | 8 s |
| `RCP_CRYO_SUIT` | Equipment Workshop | Cryogenic Engine × 1<br>Superconductive Coil × 2<br>Titan Alloy Plate × 6 | Cryo Suit Mk1 × 1 | 30 s |

### Kapitola 4 – Photonic Manufacturing

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_ASTERITE_WAFER` | Photonic Fabricator | Asterite Ore × 60<br>Vanilla Excited Photonic Matter × 30 | Asterite Wafer × 30 | 10 s |
| `RCP_OPTICAL_GLASS` | Optical Glass Furnace | Vanilla Silica × 30<br>Asterite Powder × 15 | Optical Glass × 20 | 8 s |
| `RCP_PHOTONIC_FIBER` | Photonic Fabricator | Optical Glass × 10<br>Vanilla Copper Ingot × 10 | Photonic Fiber × 8 | 8 s |
| `RCP_PHOTONIC_PROCESSOR` | Quantum Circuit Assembler | Asterite Wafer × 6<br>Vanilla Circuit Board × 4 | Photonic Processor × 2 | 16 s |
| `RCP_QUANTUM_ROUTING_ARRAY` | Quantum Circuit Assembler | Photonic Processor × 2<br>Vanilla Time Crystal × 2 | Quantum Routing Array × 1 | 20 s |
| `RCP_PHOTONIC_CELL` | Photonic Fabricator | Photonic Processor × 1<br>Vanilla Power Shard × 2 | Photonic Energy Cell × 4 | 8 s |
| `RCP_PHOTONIC_ROUTING_ARRAY` | Quantum Circuit Assembler | Quantum Routing Array × 2<br>Photonic Fiber × 8<br>Optical Glass × 10 | Photonic Routing Array × 1 | 24 s |
| `RCP_QUANTUM_COMMUNICATION_CORE` | Quantum Circuit Assembler | Photonic Processor × 4<br>Quantum Routing Array × 2<br>Superconductive Coil × 1 | Quantum Communication Core × 1 | 30 s |
| `RCP_PHOTONIC_RIFLE` | Equipment Workshop | Photonic Processor × 4<br>Asterite Wafer × 8<br>Titan Alloy Beam × 4 | Photonic Rifle × 1 | 40 s |
| `RCP_BLUEPRINT_MK5` | Equipment Workshop | Titan Alloy Beam × 20<br>Photonic Processor × 10<br>Quantum Routing Array × 4 | Blueprint Designer Mk5 × 1 | 60 s |

### Kapitola 5 – Gravitic Engineering

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_GRAVIONITE_SHARD` | Mass Compression Unit | Gravionite Ore × 30 | Gravionite Shard × 20 | 8 s |
| `RCP_STABILIZED_GRAVIONITE` | Gravitic Stabilization Reactor | Gravionite Shard × 20<br>Vanilla Dark Matter Crystal × 10<br>Vanilla Reanimated Sam × 20 | Stabilized Gravionite × 8 | 15 s |
| `RCP_GRAVITIC_CORE` | Gravitic Assembler | Stabilized Gravionite × 8<br>Vanilla Superposition Oscillator × 2 | Gravitic Core × 2 | 20 s |
| `RCP_GRAVITIC_STABILIZER` | Gravitic Assembler | Gravitic Core × 1<br>Titan Alloy Ingot × 10 | Gravitic Stabilizer × 1 | 15 s |
| `RCP_QUANTUM_LOGISTICS_ENGINE` | Gravitic Assembler | Gravitic Stabilizer × 2<br>Photonic Processor × 2<br>Vanilla Time Crystal × 4 | Quantum Logistics Engine × 1 | 30 s |
| `RCP_GRAVITIC_TRANSIT_CORE` | Gravitic Assembler | Gravitic Core × 2<br>Gravitic Stabilizer × 2<br>Quantum Logistics Engine × 1 | Gravitic Transit Core × 1 | 45 s |
| `RCP_ORBITAL_LOGISTICS_MODULE` | Gravitic Assembler | Titan Alloy Beam × 8<br>Superconductive Coil × 4<br>Photonic Fiber × 10 | Orbital Logistics Module × 1 | 30 s |
| `RCP_GRAVITIC_LAUNCHER` | Equipment Workshop | Gravitic Core × 2<br>Quantum Routing Array × 2<br>Heavy Rebar Repeater × 1 | Gravitic Launcher × 1 | 50 s |
| `RCP_GRAVITY_GRENADE` | Assembler | Stabilized Gravionite × 2<br>Vanilla Nobelisk × 1 | Gravity Grenade × 1 | 12 s |
| `RCP_REPULSION_GRENADE` | Assembler | Stabilized Gravionite × 2<br>Vanilla Nobelisk × 1<br>Photonic Processor × 1 | Repulsion Grenade × 1 | 15 s |
| `RCP_GRAVITY_BOOTS` | Equipment Workshop | Gravitic Stabilizer × 2<br>Titan Alloy Plate × 6<br>Vanilla Motor × 2 | Gravity Boots × 1 | 35 s |
| `RCP_PERSONAL_SHIELD` | Equipment Workshop | Gravitic Core × 1<br>Superconductive Coil × 2<br>Photonic Processor × 2 | Personal Shield Generator × 1 | 35 s |
| `RCP_ORBITAL_PREASSEMBLY_PROCESSOR` | Gravitic Assembler | Quantum Routing Array × 2<br>Photonic Processor × 2<br>Gravitic Core × 1 | Orbital Preassembly Processor × 1 | 35 s |

### Kapitola 6 – Interplanetary Logistics

| ID receptu | Stroj | Vstupy | Výstupy | Čas |
|---|---|---|---|---:|
| `RCP_QUANTUM_CARGO_CORE` | Orbital Assembly Plant | Gravitic Core × 2<br>Photonic Processor × 4 | Quantum Cargo Core × 1 | 30 s |
| `RCP_QUANTUM_STORAGE_MATRIX` | Orbital Assembly Plant | Asterite Wafer × 12<br>Stabilized Gravionite × 4 | Quantum Storage Matrix × 1 | 30 s |
| `RCP_COMMAND_PROCESSOR` | Orbital Assembly Plant | Quantum Routing Array × 2<br>Quantum Cargo Core × 1 | Interplanetary Command Processor × 1 | 45 s |
| `RCP_ORBITAL_ASSEMBLY_FRAME` | Orbital Assembly Plant | Titan Alloy Beam × 12<br>Quantum Logistics Engine × 1 | Orbital Assembly Frame × 1 | 45 s |

## Tři nové Space Elevator projekty

Tyto projekty navazují na původní Space Elevator a jsou samostatné od šesti výzkumných kapitol. Finální množství se musí vybalancovat v dlouhodobém hraní.

| Projekt | Název | Odemkne |
|---|---|---|
| VI | Frontier Expansion | Terminal III–IV |
| VII | Planetary Network | Terminal V |
| VIII | Orbital Assembly | Terminal VI |

## Technická architektura

1. **JSON katalog** je jediný návrhový zdroj ID, kapitol, odemčení, receptů a gate podmínek.
2. **Schematic layer** musí převést katalog do skutečných `UFGSchematic` a konkrétních `UFGUnlock` instancí.
3. **Asset layer** musí dodat item descriptors, recepty, stroje, buildable descriptors, hologramy, ikony, zvuky a materiálové instance.
4. **Equipment / combat layer** potřebuje weapon/equipment actor classes, ammo descriptors, damage data, cooldowns, animations and server-authoritative replication.
5. **Progression layer** musí číst dokončení Space Elevatoru, zpřístupnit správné kapitoly a ukládat nákupy schematics.
6. **Validation layer** ověřuje unikátní ID, vazby mezi kapitolami, recepty a Elevator požadavky; nikdy nesmí prohlásit JSON design za hotový herní obsah.

## Ferravium – první nativní obsahový prototyp

První endgame obsahový řez má nyní nativní C++ zdrojové třídy, výrobní recepty, vlastní manufacturer a výzkumnou schematiku. Stav této části je **source prototype**: je připravená v repozitáři, ale zatím nebyla zkompilována v cílovém Unreal Engine 5.6.1 ani ověřena v běžící hře.

### Výrobní řetězec

| Recept | Stroj | Vstupy za cyklus | Výstup za cyklus | Čas |
|---|---|---|---|---:|
| Ferravium Recovery (dočasný zdroj) | Vanilla Oil Refinery | 120 Bauxite Ore + 30 Coal + 60 Water | 60 Ferravium Ore | 18 s |
| Ferravium Washing | Vanilla Oil Refinery (prototyp, později Ore Washing Plant) | 60 Ferravium Ore + 30 Water | 45 Ferravium Concentrate | 12 s |
| Ferravium Ingot | Ferravium Smelter | 45 Ferravium Concentrate + 30 Coal | 30 Ferravium Ingots | 10 s |

**Dočasný zdroj rudy:** Ferravium Recovery z bauxitu, uhlí a vody je pouze bootstrap recept pro testování item descriptorů a výrobní linky. Nepředstavuje přirozené naleziště Ferravia. Přidání skutečných uzlů do vanilla mapy a jejich registrace do skeneru zůstává odděleným technickým úkolem kvůli omezením World Partition.

### Nový stroj: Ferravium Smelter

- Native actor `AFICSITFerraviumSmelter : AFGBuildableManufacturer`.
- Vstupní a výstupní `UFGFactoryConnectionComponent` pro pásy.
- Základní odběr nastavený na 90 MW pomocí reflektovaného `mPowerConsumption` pole base třídy.
- Nativní buildable descriptor, Build Gun recipe a production recipe.
- Vlastní OBJ/MTL průmyslový model a ikona; import zajišťuje `Tools/Unreal/BuildFerraviumAssets.py`.

### Výzkum

Schematika **Frontier Survey // Ferravium Processing** odemyká:
- Ferravium Recovery, Washing a Smelting recipes.
- Ferravium Smelter buildable recipe.
- Všechny tři Ferravium materiálové descriptory.

Přístup k schematice vyžaduje zakoupený FICSIT Dispatch Terminal a stav, kdy je v uloženém endgame postupu dostupná Terminal Phase I. To znamená, že skutečné odemčení stále čeká na propojení s událostí dokončení vanilla Space Elevator Phase V; dokud tato událost nebude napojena, schematika zůstane skrytá.

### Import assetů v cílovém Starter Projectu

1. Zkopíruj `Mods/GameFeatures/FICSITContracts`, `Mods/FICSITContracts/ContentSource` a `Tools/Unreal` do Satisfactory Modding Starter Projectu.
2. V Unreal Editoru spusť `Tools/Unreal/BuildFICSITContractsAssets.py` pro vytvoření původního Dispatch Terminal mesh/icon a Game Feature Data assetu.
3. Poté spusť `Tools/Unreal/BuildFerraviumAssets.py` pro import Ferravium/Titan Alloy položek, smelteru a pěti ikon a pěti meshů.
4. Zkontroluj v Content Browseru cesty pod `/FICSITContracts/Resources/Ferravium` a `/FICSITContracts/Buildables/FerraviumSmelter`, včetně `SM_TitanAlloyIngot`, `SM_TitanAlloyBeam`, `TX_TitanAlloyIngot_256` a `TX_TitanAlloyBeam_256`.
5. Zkompiluj `FactoryGameEditor`, spusť Alpakit a otestuj schematiku, build gun, pásové porty, power draw, celý výrobní řetězec a save/load.

## Stav prvních obsahových prototypů: Ferravium a Titan Alloy

První native source slice obsahuje:
- `UFICSITFerraviumOreDescriptor` jako `UFGResourceDescriptor`, plus Concentrate a Ingot item descriptory.
- Nativní `AFICSITFerraviumSmelter : AFGBuildableManufacturer` se vstupním a výstupním conveyor connection.
- Buildable descriptor, Build Gun recept a `Frontier Survey` schematic.
- Recipe unlock, který zpřístupňuje recovery, washing, smelting i konstrukci pece.
- Zdrojové OBJ/MTL modely a Editor importér pro modely a čtyři ikony.

**Omezení první iterace:** přirozené Ferravium resource nodes ještě nejsou umístěné v herním světě. Prozatím se ruda získává přes recept Ferravium Recovery ve vanilla Oil Refinery. Je to pouze testovací bootstrap cesta, nikoli náhrada finální hlubinné těžby. Frontier Survey je navíc záměrně skrytý, dokud nebude skutečná vanilla Space Elevator Phase V událost volat serverovou metodu, která zpřístupní Terminal I.

## Stav druhého obsahového prototypu: Advanced Metallurgy

Druhý native source slice obsahuje:
- `UFICSITTitanAlloyIngotDescriptor` s vlastním conveyor mesh a generovanou UI ikonou.
- `UFICSITTitanAlloyRecipe`, odemykaný schématem Advanced Metallurgy.
- Výrobní cyklus 60 Ferravium Ingots + 60 Steel Ingots → 60 Titan Alloy Ingots za 8 sekund ve vanilla Foundry Mk1.
- `UFICSITAdvancedMetallurgySchematic`, skrytou do zakoupení Frontier Survey a dostupnosti Terminal II.
- Serverové potvrzení nákupu, které ukládá Terminal II jako dokončený a reconciliuje existující save v pořadí Terminal I → Terminal II.
- OBJ/MTL mesh, pětici generovaných UI ikon a editorový import assetů.

**Omezení druhé iterace:** vanilla Foundry Mk1 je dočasný kompatibilní bootstrap pro ingot i beam. Dedicated Advanced Alloy Foundry a Industrial Press, Reinforced Titan Frame a ostatní recepty Phase II ještě nejsou implementované. Titan Alloy Beam má vlastní native descriptor, mesh, ikonu a Foundry bootstrap, ale konečný návrhový Industrial Press řetězec zůstává samostatně plánovaný. Nativní zdrojové třídy nejsou dosud UHT/UE zkompilované ani otestované ve hře.

V cílovém SML Starter Projectu je nutné spustit nejprve `BuildFICSITContractsAssets.py` a potom `BuildFerraviumAssets.py`, zkompilovat `FactoryGameEditor`, zkontrolovat asset registry a provést in-game test obou výrobních linek i obou schémat. Portable CI ani statická kontrola tuto herní validaci nenahrazují.

## Rizika a pořadí prototypování

| Priorita | Oblast | Důvod |
|---|---|---|
| P0 | Data-driven Schematic/Unlock registry | Základ všech odemčení |
| P0 | Custom item descriptor + recipe | Nutné pro jakýkoli nový výrobní řetězec |
| P0 | Jedna custom výrobní budova | Ověření buildable a výrobního cyklu |
| P1 | Blueprint Designer Mk4 | Ověřit rozsah a save compatibility |
| P1 | Nová zbraň + munice | Ověřit damage, equipment, animace a multiplayer |
| P1 | Nový resource deposit | World Partition a ore scanner představují riziko |
| P2 | Drony a obranná AI | Cíl, navigace, síť, výkon |
| P2 | Quantum Cargo Relay | Potřebuje transakční transfer bez ztráty a duplikace |
| P2 | Remote Factory Control | Bezpečné serverové řízení a oprávnění |

## Implementační milníky

- **0.3.0-A – Progression foundation:** JSON schema, loader, unlock registry, saved research state, UI categories.
- **0.3.0-B – First content slice:** Ferravium resource path, one new machine, first custom recipes, Blueprint Designer Mk4 prototype.
- **0.3.0-C – Player gear:** Precision Rebar Rifle, ammo, Pathfinder Visor and Exosuit.
- **0.4.0 – Cryogenic / Photonic:** Cryonite/Asterite chains, new machines, Cryo Projector, Photonic Rifle, Smart Logistics.
- **0.5.0 – Gravitic:** Gravionite, gravitic machine, launcher, gravity equipment and power storage.
- **0.6.0 – Orbital Logistics:** Space Elevator VI–VIII integration, Orbital Freight Hub, Quantum Cargo Relay and remote management.
- **1.0.0 – Release candidate:** UE build, packaging, save/load, multiplayer, performance, balance and asset validation.

## Autoritativní technické odkazy

- [Blueprint Designer – Official Satisfactory Wiki](https://satisfactory.wiki.gg/wiki/Blueprint_Designer)
- [Satisfactory Modding – Schematics](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/Schematic.html)
- [Satisfactory Modding – Ore Scanner](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/OreScanner.html)
- [Satisfactory Modding – Custom Levels / World Partition limitation](https://docs.ficsit.app/satisfactory-modding/latest/Development/Satisfactory/CustomLevels.html)
