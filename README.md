# Sneak Gear

Unreal Engine 5.7 stealth-action project inspired by the Metal Gear Solid series.

## Current State

**Player**
- Third-person movement with aim, crouch, and cover snap
- Inventory-backed weapon slots: runtime weapon actors, clip/reserve ammo, weapon selection menu
- Animation-driven fire system: hitscan (spread, recoil, penetration) and projectile fire modes
- GAS-based health, stamina, and ammo attributes with reserve-to-clip reload flow
- Player components: `UPlayerInventoryComponent`, `UPlayerInventoryInteractionComponent`, `UPlayerWeaponComponent`, `UPlayerHUDComponent`, `UPlayerAimComponent`, `UPlayerLocomotionComponent`

**AI**
- Guard awareness with vision-based perception, alert levels, and patrol path following
- Behavior Tree-driven movement with combat response and player tracking
- Custom BT nodes for patrol, firing, reload, and clip-state checks
- Guard spawner, guard manager subsystem, and guard archetype data assets for tuning
- `UGuardAwarenessComponent` and `UGuardPatrolComponent` as reusable guard components

**UI**
- Radar, crosshair, weapon HUD, vitals panel, pickup prompt, and info overlays
- Equipped item display, weapon selection menu, event log feed, and stealth debug widget
- UI layer isolated as `SneakGearUI` plugin; reads from `IPlayerUIDataSource`

## Docs

| Document | Description |
|----------|-------------|
| [Features](Docs/Features.md) | Current implemented features — player, combat, AI, UI, GAS |
| [Source Layout](Docs/ProjectStructure.md) | Module structure, feature slices, class ownership rules |
| [Guard Setup](Docs/GuardSetup.md) | BT requirements, blackboard keys, patrol configuration |
| [Inventory & UI Notes](Docs/InventoryNotes.md) | Player inventory and HUD component breakdown |
| [Known Constraints](Docs/Constraints.md) | Active constraints and suggested next work |
| [Technical Debt](Docs/TechnicalDebt.md) | Architecture, AI, combat, performance, and tooling gaps |
| [Goal Checklist](Docs/GoalChecklist.md) | Full feature wishlist |
| [Automated Tests](Docs/AutomationTests.md) | How to run tests and what each suite covers |
| [Test Results](Docs/AutomationReports/TestStatus.md) | Latest automation report |
| [Architecture — Current](Docs/C4Architecture.md) | C4 diagrams of the current module and component structure |
| [Architecture — Target](Docs/C4ArchitectureTarget.md) | C4 diagrams of the intended target architecture |
