# In-game validation checklist for 0.1.2

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
- [ ] Cancel a partially completed order and confirm the reputation fee and retained cancellation history.
- [ ] Confirm cancellation cannot push reputation below zero.
- [ ] Confirm an expired order cannot be cancelled and is labelled EXPIRED, not CANCELLED.
- [ ] Save, exit, reload and confirm offers, active progress, reputation and history persist.
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
