# In-game validation checklist for 0.2.0-alpha.2

This checklist intentionally remains unchecked until tested in the target Satisfactory build.

## Packaging and loading

- [ ] Build `FactoryGameEditor` with SML 3.12.x Starter Project and custom UE 5.6.1.
- [ ] Confirm UHT completes without errors.
- [ ] Run the Unreal Editor asset import utility and verify the imported terminal mesh, icon and Game Feature Data.
- [ ] Set the Game Feature Data Initial State to Active.
- [ ] Package the plugin with Alpakit.
- [ ] Confirm SMM loads FICSIT Contracts without dependency or version warnings.
- [ ] Confirm no missing module / missing item class errors appear in the game log.

## Dispatch Terminal building

- [ ] Confirm the terminal appears in the Trading Post / Progression build category after purchasing FICSIT Dispatch Network.
- [ ] Verify the Oil Processing dependency hides the Dispatch Network schematic beforehand.
- [ ] Build the terminal and verify its hologram, collision, clearance and dismantle refund.
- [ ] Verify the full industrial mesh appears, including canopy, side service towers, operator deck, three screens, keypad, vents and beacon.
- [ ] Verify imported materials retain separate Gunmetal, Steel, Black, Orange, Screen and Indicator slots.
- [ ] Verify Gunmetal and Steel respond to the Customizer while orange/black/screen parts retain their intended appearance.
- [ ] Verify the cyan screen light and orange beacon illuminate without causing excessive bloom or performance cost.
- [ ] Confirm fallback geometry appears if the imported mesh is unavailable and does not overlap the imported model.

## Contract board and single player

- [ ] Start a new or copied test save and run `/contracts`.
- [ ] Confirm five distinct offers appear when five eligible templates exist.
- [ ] Confirm reputation starts at 0 and the entry rank is FICSIT Trainee.
- [ ] Accept up to four orders and confirm the fifth active order is rejected.
- [ ] Confirm reputation gates prevent higher-tier orders from appearing too early.
- [ ] Complete a basic one-item order and verify only the outstanding amount is removed.
- [ ] Complete a multi-material standard order and verify no reward is paid until every manifest line is supplied.
- [ ] Attempt an unrequested item and verify it is neither removed nor credited.
- [ ] Attempt to deliver more than the outstanding amount and verify the excess stays in inventory.
- [ ] Confirm timed Emergency Grid Recovery expires at its deadline and grants no reputation.
- [ ] Cancel an active order and confirm the reputation fee and retained cancellation history.

## Strategic program validation

- [ ] Accept Factory Backbone Program and verify all four phases are shown in the terminal.
- [ ] Deliver only one material from a multi-item phase and verify the next phase stays locked.
- [ ] Deliver a future-phase product early and verify it is not removed from inventory.
- [ ] Complete phase one and verify phase two becomes current.
- [ ] Save/reload during a partially completed phase and confirm every material line and phase index persist.
- [ ] Complete Factory Backbone and confirm the 1,100 REP reward is granted exactly once.
- [ ] Validate the four-phase Foundry Expansion Initiative.
- [ ] Validate the five-phase Automated Assembly Rollout and Planetary Power Grid Program.
- [ ] Validate all six phases of Planetary Logistics Network.
- [ ] Validate all seven phases of Orbital Logistics Authority Program.
- [ ] Confirm full phase routes in the terminal and `/contracts` output show COMPLETE, CURRENT and LOCKED phases correctly.
- [ ] Verify completed, expired and cancelled entries are clearly distinguished in the archive.

## Terminal interaction and multiplayer

- [ ] Look at the terminal and press E; confirm the interaction widget opens and closes correctly.
- [ ] Confirm reputation, rank, five available offers, four active slots and full multi-phase routes are visible.
- [ ] Confirm Accept, Deliver and Cancel buttons work and refresh the board after a state change.
- [ ] Join a host session with a second client and confirm the board/reputation is replicated.
- [ ] Use terminal actions from a client and verify the server revalidates the owner/controller pair and terminal distance.
- [ ] Confirm Deliver only consumes the requesting player's main inventory.
- [ ] Save/reload with one or more terminals placed and confirm all remain usable.
- [ ] Confirm multiple terminals access the same board rather than generating duplicate contract state.

## Endgame Expansion — pending implementation and in-game validation

These are acceptance tests for the planned 0.3.x+ content. They must remain unchecked until the content is implemented and tested in Satisfactory.

### Progression and Space Elevator

- [ ] Verify vanilla Space Elevator Phase V remains unchanged.
- [ ] Complete vanilla Phase V and confirm Terminal Chapters I and II become available.
- [ ] Complete custom Space Elevator Phase VI and confirm Terminal Chapters III and IV become available.
- [ ] Complete custom Space Elevator Phase VII and confirm Terminal Chapter V becomes available.
- [ ] Complete custom Space Elevator Phase VIII and confirm Terminal Chapter VI becomes available.
- [ ] Verify no chapter or Elevator project can be purchased before its prerequisites.
- [ ] Save/reload and verify completed schematics and unlock state persist in multiplayer.

### Custom resources and production

- [ ] Spawn and harvest Ferravium in a supported vanilla-world resource/deposit path.
- [ ] Confirm Ferravium appears in the resource scanner and map markers.
- [ ] Validate Ferravium mining, washing, smelting and all downstream recipes.
- [ ] Repeat resource-node, scanner and production tests for Cryonite, Asterite and Gravionite.
- [ ] Confirm every Space Elevator VI–VIII requirement has a reachable production chain before its project is offered.
- [ ] Confirm recipe throughput, byproducts, fluid consumption, power draw and machine input/output buffers.

### Blueprints and building

- [ ] Verify Blueprint Designer Mk4 builds, saves, reloads and places blueprints correctly.
- [ ] Verify Blueprint Designer Mk5 can open Mk4 and lower-level blueprints without corrupting save data.
- [ ] Verify boundary dimensions and placement restrictions.
- [ ] Test blueprint sharing and dedicated-server save/load.
- [ ] Verify all new building descriptors, build recipes, holograms, categories, icons and dismantle refunds.

### Weapons, equipment and vehicles

- [ ] Verify each custom weapon can be equipped, fired, reloaded and repaired where applicable.
- [ ] Verify ammo consumption, damage, fire rate, recoil, effects and cooldowns.
- [ ] Test Cryo/Gravitic status effects against all supported creature classes.
- [ ] Verify Exosuit, Cryo Suit, Jetpack Mk2, Hoverpack Mk2, Gravity Boots and Personal Shield fit valid equipment slots.
- [ ] Verify equipment state and effects replicate and persist in multiplayer.
- [ ] Verify Survey Drones and Security Drones navigate, return, despawn and recover safely.
- [ ] Verify Industrial Garage and all vehicle interactions do not break vanilla vehicles.

### Logistics, power and automation

- [ ] Measure real Conveyor Mk7/Mk8/Mk9 throughput under load; verify splitters and lifts preserve rates.
- [ ] Measure miners at 100%, overclocked and underclocked settings across all node purities.
- [ ] Validate Pipeline Mk3 and cryogenic fluid stability.
- [ ] Verify Quantum Cargo Relay transfer is atomic: no item loss, duplication or cross-session desync.
- [ ] Test relay capacity, energy consumption, pairing, filters, cooldown and network failure recovery.
- [ ] Verify Remote Factory Control checks authority and only operates permitted devices.
- [ ] Test all new storage, alarms, power distribution and monitoring at factory scale.

### Performance and compatibility

- [ ] Profile large-scale drone, turret, monitor and relay deployments.
- [ ] Test dedicated-server CPU/network usage and multiple concurrent players.
- [ ] Confirm all content loads with SML 3.12.x and custom UE 5.6.1.
- [ ] Run Alpakit packaging and a clean-install test.
- [ ] Confirm no missing assets, invalid vanilla descriptor paths, save migration warnings or runtime log spam.

