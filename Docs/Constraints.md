# Known Constraints

- Guard Behavior Trees assume the blackboard keys listed in [GuardSetup.md](GuardSetup.md)
- Guard aiming and BT chasing now use the same target source
- `UPlayerInventoryComponent` weapon slots require both a valid slot item and a runtime weapon actor
- New gameplay code should use `UPlayerWeaponComponent` for equip/fire/reload and active-weapon state; inventory weapon methods remain as deprecated compatibility wrappers
- Character death callbacks are latched and should fire once
- Radar updates are throttled instead of rebuilt every frame

# Suggested Next Work

- Add editor validation for invalid guard BT / blackboard setups
- Move more guard query APIs behind guard components
- Reorganize `Weapon/` into a clearer `Combat/` slice
- Move runtime weapon state out of `UPlayerInventoryComponent` once Blueprint/test compatibility allows it
- Improve AI aiming points to use sockets / eyes instead of actor center offsets
- Add editor validation checks for inventory, guard setup, and BT assumptions
