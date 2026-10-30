# In-game validation checklist for 0.2.0

This checklist is intentionally unchecked until tested in the target game.

## Packaging and loading

- [ ] Build `FactoryGameEditor` with the SML 3.12.x Starter Project and custom UE 5.6.1.
- [ ] Confirm UHT completes without errors.
- [ ] Package the plugin with Alpakit.
- [ ] Confirm SMM loads FICSIT Contracts without dependency or version warnings.
- [ ] Confirm no missing module / missing item class errors appear in the game log.

## Single player

- [ ] Start a new or copied test save and run `/contracts`.
- [ ] Confirm three offers appear and reputation starts at 0.
- [ ] Accept an offer with `/contracts accept <ID>`.
- [ ] Confirm the same contract moves from available to active.
- [ ] Attempt to accept an unknown ID and confirm no state changes.
- [ ] Attempt to accept a fourth active contract and confirm the cap is enforced.
- [ ] Put the exact required vanilla items in the main inventory and run `/contracts deliver <ID>`.
- [ ] Confirm only the outstanding amount is removed.
- [ ] Confirm a partially supplied contract stays active and reports the correct percentage.
- [ ] Confirm a completed contract is moved to history and reputation is awarded once.
- [ ] Confirm a repeated delivery cannot award reputation again.
- [ ] Accept Factory Backbone Program and confirm only the Structural Base stage is shown.
- [ ] Deliver Reinforced Iron Plates or Motors early and confirm they are not removed from inventory.
- [ ] Complete Structural Base and confirm Reinforcement unlocks; confirm the command reports the stage transition.
- [ ] Save/reload during Reinforcement and confirm the stage index and previous deliveries persist.
- [ ] Complete Reinforcement and confirm Powertrain unlocks.
- [ ] Complete Powertrain and confirm the 750 REP reward is granted exactly once.
- [ ] Cancel a partially completed order and confirm the reputation fee and retained cancellation history.
- [ ] Confirm cancellation cannot push reputation below zero.
- [ ] Confirm an expired order cannot be cancelled and is labelled EXPIRED, not CANCELLED.
- [ ] Save, exit, reload and confirm offers, active progress, reputation, history and multi-stage progress persist.
- [ ] Verify a timed offer expires after its deadline and grants no reputation.

## Multiplayer / dedicated server

- [ ] Join a host session with a second client and confirm the board/reputation is replicated.
- [ ] Confirm only the server changes the board and inventory.
- [ ] Accept/deliver from a client and confirm exactly one state transition and one inventory deduction.
- [ ] Save and reload the session; confirm state remains consistent on host and clients.

## Edge cases

- [ ] Deliver with zero matching items.
- [ ] Deliver more than the remaining amount.
- [ ] Deliver an item not required by the selected contract.
- [ ] Expire an active timed contract.
- [ ] Cancel an active contract and confirm the cancellation penalty is applied only once.
- [ ] Test an item descriptor class that fails to resolve and confirm no items are removed.


## Dispatch Terminal — editor and in-game validation

- [ ] Run the asset import utility in the SML 3.12 Starter Project.
- [ ] Confirm the `FICSITContracts` Game Feature Data asset exists and Initial State is Active.
- [ ] Compile Development Editor with custom UE 5.6.1 and resolve all UHT/compiler warnings.
- [ ] Confirm FICSIT Dispatch Network stays hidden before Oil Processing and appears in the Tier 5 Logistics schematic list after Oil Processing is purchased.
- [ ] Purchase the schematic and confirm the build recipe appears in the build gun under Trading Post / Progression.
- [ ] Build the terminal and verify the hologram, placement clearance, dismantle refund and paint/customizer behavior.
- [ ] Look at the terminal and press E; confirm the Satisfactory interaction widget opens and closes correctly.
- [ ] Confirm the board shows current reputation, active contracts, available offers and staged progress.
- [ ] Confirm a second player sees the same board and no duplicate contract state is created by a second terminal.
- [ ] Save/reload with one or more terminals placed and confirm they remain usable.
- [ ] Confirm chat commands still perform accept/deliver/cancel on the authoritative host.

- [ ] Verify the terminal's gunmetal/steel surfaces respond to the Customizer while orange, black and screen details retain their materials.
- [ ] Verify the terminal's clearance box prevents overlapping placement and its solid collision blocks player movement.
- [ ] Use Accept from a client and confirm the server updates the shared board for all players.
- [ ] Use Deliver from a client and confirm only that player's main inventory is consumed, never another player's inventory or a client-side item count.
- [ ] Use Cancel from a client and confirm the server applies the configured reputation penalty exactly once.
- [ ] Confirm the UI refreshes on all clients after a contract state change.
