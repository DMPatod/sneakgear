# Sneak Gear

Unreal Engine 5.7 stealth-action project inspired by the Metal Gear Solid series.

## Current State

- Third-person player with aim, cover, inventory-backed weapon slots, and weapon selection UI
- Guard AI with awareness, patrol paths, Behavior Tree-driven movement, and combat response
- Shared GAS-based health, stamina, and ammo attributes
- Radar, crosshair, weapon HUD, vitals, and debug UI

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
