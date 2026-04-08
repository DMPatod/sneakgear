# C4 Architecture — SneakGear Target State

Diagrams showing the **intended architecture** derived from the goal checklist, technical debt backlog, and suggested next work. Annotations mark what is **new**, **enhanced**, or **refactored** vs the current state.

Legend:
- `[NEW]` — does not exist yet
- `[ENHANCED]` — exists but needs significant rework
- `[REFACTORED]` — exists but needs restructuring/renaming
- _(no annotation)_ — structurally sound, carries forward as-is

---

## Level 1 — System Context

```mermaid
C4Context
  title System Context — SneakGear (Target)

  Person(player, "Player", "Controls the player character via keyboard / gamepad")

  System_Boundary(sg, "SneakGear (Unreal Engine 5.7)") {
    System(game, "SneakGear Game", "Metal Gear-inspired stealth-action with deep AI, loadout, missions, and world systems")
  }

  System_Ext(save, "Save Slot", "Persistent save file on disk — checkpoints, loadout, mission progress")

  Rel(player, game, "Plays")
  Rel(game, save, "Reads / writes")
```

---

## Level 2 — Containers (Target Module Split)

The main `SneakGear` module is split at the seams where responsibility has grown large enough to warrant isolation. New modules are annotated.

```mermaid
C4Container
  title Container Diagram — Target Module Architecture

  Person(player, "Player")

  System_Boundary(sg, "SneakGear") {

    Container(sneakgear, "SneakGear", "UE C++ Runtime Module", "Core gameplay: player movement, combat, stealth, inventory, NPC, mission")

    Container(sneakgearai, "SneakGearAI [NEW]", "UE C++ Runtime Module", "Guard AI: perception, investigation, search, alert propagation, reinforcements")

    Container(sneakgearcore, "SneakGearCore", "UE C++ Runtime Module [ENHANCED]", "Shared types, enums, structs, UI interfaces, audio event types, save types")

    Container(sneakgearui, "SneakGearUI", "UE C++ Plugin [ENHANCED]", "Event-driven HUD: radar, crosshair, vitals, stealth rating, objective tracker")

    Container(sneakgeartests, "SneakGearTests", "UE C++ UncookedOnly Module [ENHANCED]", "Full coverage: inventory, AI states, combat, stealth, missions, save/load")

  }

  System_Ext(uegas, "GameplayAbilitySystem", "Health, stamina, ammo, armor attributes + gameplay effects for damage/healing")
  System_Ext(uebt, "BehaviorTree / AIModule", "Guard AI execution — enhanced BT with investigation and search sequences")
  System_Ext(ueinput, "EnhancedInput", "Player input action mapping")
  System_Ext(uemetasound, "MetaSound / Audio [NEW]", "Surface-aware footsteps, weapon audio, guard barks, ambient audio")
  System_Ext(uechaos, "Chaos Physics / Streaming", "World simulation, level streaming for large maps")

  Rel(player, sneakgear, "Plays via")
  Rel(sneakgear, sneakgearai, "Queries guard state from / drives guard targets in")
  Rel(sneakgear, sneakgearcore, "Imports shared types from")
  Rel(sneakgearai, sneakgearcore, "Imports shared AI types from")
  Rel(sneakgear, sneakgearui, "Pushes gameplay events to")
  Rel(sneakgearui, sneakgearcore, "Uses IPlayerUIDataSource from")
  Rel(sneakgear, uegas, "Health / stamina / ammo / armor attributes")
  Rel(sneakgearai, uebt, "Guard AI execution")
  Rel(sneakgear, ueinput, "Player input")
  Rel(sneakgear, uemetasound, "Fires audio events via UAudioEventSubsystem")
  Rel(sneakgear, uechaos, "World simulation and streaming")
  Rel(sneakgeartests, sneakgear, "Tests")
  Rel(sneakgeartests, sneakgearai, "Tests")
```

---

## Level 3 — Components: SneakGear (Target Feature Slices)

```mermaid
C4Component
  title Component Diagram — SneakGear: Target Feature Slices

  Container_Boundary(sg, "SneakGear Runtime Module") {

    Component(characters, "Characters", "ABaseCharacter [ENHANCED]", "Base GAS character: lifecycle, damage, death, armor attribute")

    Component(player, "Player", "APlayerCharacterBase [ENHANCED]\nASneakGearPlayerCharacter [ENHANCED]", "Walk/run/sprint/crouch/prone/vault/climb\nShoulder swap, obstruction camera, cinematic takedowns\nContextual cover: snap, lean, blind-fire, corner transitions")

    Component(playercomps, "Player Components", "UPlayerInventoryComponent [REFACTORED]\nUPlayerLoadoutComponent [NEW]\nUPlayerWeaponComponent [ENHANCED]\nUPlayerHUDComponent\nUPlayerInventoryInteractionComponent", "Inventory split from loadout/progression\nWeight/encumbrance, pre-mission loadout screen\nWeapon spawning moved out of inventory")

    Component(combat, "Combat [REFACTORED from Weapon/]", "AWeaponBase [ENHANCED]\nAHitscanWeaponBase [ENHANCED]\nUWeaponStateMachine [NEW]\nUDamageSystem [NEW]\nUNonLethalSystem [NEW]", "Weapon state machine: idle → firing → reloading → jammed\nDamage falloff, armor penetration, cover penetration\nNon-lethal: chokeout, stun, tranquilizer, restraints\nAttachment and upgrade system")

    Component(stealth, "Stealth [NEW]", "UStealthSystem [ENHANCED from StealthSignatureComponent]\nUNoiseEmitterComponent [NEW]\nUDisturbanceSubsystem [NEW]\nUBodyDiscoverySubsystem [NEW]", "Noise events: footsteps, item throws, gunshots\nDisturbance propagation to nearby guards\nBody discovery → alert raise\nStealth tools: silencers, distractions, decoys, smoke, EMP")

    Component(inventory, "Inventory / Loadout [ENHANCED]", "UPlayerLoadoutComponent [NEW]\nUWeaponAttachmentComponent [NEW]\nUPlayerItemDefinition [ENHANCED]", "Richer item data model with loadout validation rules\nPre-mission slot selection, weight/encumbrance budget\nAttachment slots per weapon (suppressor, scope, grip)")

    Component(mission, "Mission [REFACTORED from Game/]", "AMissionManager [NEW]\nAMissionObjective [NEW]\nUStealthRatingComponent [NEW]\nUSaveLoadSubsystem [NEW]", "Objective tracking (eliminate, extract, reach, surveil)\nStealth rating: alert count, kill type, time, detection events\nAlert penalties and time bonuses\nCheckpoint save/load")

    Component(npc, "NPC [NEW]", "ANPCCharacter [NEW]\nUNPCInteractionComponent [NEW]", "Hostage, escort target, informant archetypes\nInteractions: rescue, escort, intel extraction")

    Component(radar, "Radar [ENHANCED]", "URadarRegistrySubsystem [ENHANCED]\nURadarEmitterComponent", "Event-driven contact updates (no interval rebuild)\nContact types: guard, hostage, objective, item")

    Component(audio, "Audio [NEW]", "UAudioEventSubsystem [NEW]", "Game-side event bus feeding MetaSound\nFootstep surface mapping, suppressed vs unsuppressed weapon audio\nGuard bark triggers, ambient state audio")

    Component(world, "World [NEW]", "UDayNightCycleSubsystem [NEW]\nUWeatherSubsystem [NEW]", "Day/night cycle with dynamic lighting influence\nWeather states affecting guard perception and player stealth signature")

    Component(game, "Game", "ASandboxGameState [ENHANCED]\nGAS AttributeSets [ENHANCED]", "Mission lifecycle\nAdds armor attribute set")

  }

  Rel(player, characters, "Extends")
  Rel(player, playercomps, "Owns")
  Rel(playercomps, combat, "Fires and reloads via")
  Rel(playercomps, inventory, "Stores loadout in")
  Rel(player, stealth, "Emits noise and stealth signature via")
  Rel(player, radar, "Contacts displayed on HUD via")
  Rel(player, npc, "Interacts with")
  Rel(player, mission, "Advances objectives via")

  Rel(combat, stealth, "Gunshot noise events to")
  Rel(combat, audio, "Fires weapon audio events to")

  Rel(stealth, audio, "Fires footstep and disturbance events to")
  Rel(stealth, radar, "Body discovery updates contacts in")

  Rel(mission, game, "Reads / writes mission state in")
  Rel(mission, stealth, "Tracks stealth events from")

  Rel(world, stealth, "Modulates stealth signature via weather")
  Rel(world, audio, "Drives ambient audio state in")
```

---

## Level 3 — Components: SneakGearAI (Target)

The AI grows into its own module to support investigation, search memory, alert propagation, and reinforcements — all currently absent.

```mermaid
C4Component
  title Component Diagram — SneakGearAI: Target

  Container_Boundary(ai, "SneakGearAI Module [NEW]") {

    Component(awareness, "Awareness [ENHANCED]", "UGuardAwarenessComponent", "Vision cone, hearing range, peripheral vision\nAwareness accumulation / decay per stimulus type\nStimulus types: visual, audio, disturbance, body discovery")

    Component(perception, "Perception [NEW]", "UGuardPerceptionRouter [NEW]", "Routes stimuli from UDisturbanceSubsystem and UAIPerceptionComponent\nPrioritises stimuli: direct sight > last-known sound > reported alert")

    Component(memory, "Guard Memory [NEW]", "UGuardMemoryComponent [NEW]", "Last-known position (LKP) per target\nLast-heard noise location\nSearched area history (prevents re-patrolling cleared zones)")

    Component(stateMachine, "Guard State Machine [NEW]", "UGuardBehaviorStateComponent [NEW]", "States: Calm → Suspicious → Investigating → Searching → Alerted → Combat → Reporting\nClean transitions owned here, not spread across BT tasks")

    Component(alertprop, "Alert Propagation [NEW]", "UAlertPropagationSubsystem [NEW]", "Guard-to-guard radio/shout alert sharing\nAlert radius and time-to-respond\nReinforcement request routing")

    Component(patrol, "Patrol [ENHANCED]", "UGuardPatrolComponent [ENHANCED]", "Scheduled waypoints\nWaypoint action tags (look around, crouch, wait)\nDynamic re-route after investigation")

    Component(btlayer, "Behavior Tree Layer [ENHANCED]", "AGuardAIController [ENHANCED]\nBT Tasks / Services / Decorators", "BT reads guard state from UGuardBehaviorStateComponent\nInvestigation sequence: move-to-LKP → search grid → resume patrol\nReinforcement sequence: report → wait for backup → converge\nEditor validation via UGuardSetupValidator [NEW]")

    Component(targeting, "Targeting [ENHANCED]", "UGuardTargetingComponent [NEW]", "Socket/eye-point aim rather than actor-centre offset\nMultiple target prioritisation (primary / secondary)\nLOS tracking per aim point")

  }

  Container_Ext(sneakgear, "SneakGear", "Disturbance events, body discovery, player actor reference")
  Container_Ext(sneakgearcore, "SneakGearCore", "Shared AI types and awareness enums")
  System_Ext(uebt, "BehaviorTree / AIModule", "UE perception, navigation, BT execution")

  Rel(awareness, perception, "Receives stimuli from")
  Rel(perception, memory, "Updates LKP and noise location in")
  Rel(memory, stateMachine, "State transition triggers from")
  Rel(stateMachine, btlayer, "Blackboard state written to")
  Rel(stateMachine, alertprop, "Sends alerts via")
  Rel(alertprop, stateMachine, "Raises alert state in other guards via")
  Rel(patrol, btlayer, "Patrol waypoints consumed by")
  Rel(targeting, btlayer, "Aim points consumed by")
  Rel(btlayer, uebt, "Executes via")
  Rel(sneakgear, perception, "Pushes disturbance and body events to")
  Rel(awareness, sneakgearcore, "Uses GuardAwarenessTypes from")
```

---

## Level 3 — Components: SneakGearUI (Target)

```mermaid
C4Component
  title Component Diagram — SneakGearUI: Target (Event-Driven)

  Container_Boundary(ui, "SneakGearUI Plugin [ENHANCED]") {

    Component(hud, "PlayerHUDWidget", "Root HUD container", "Composes all sub-widgets; subscribes to gameplay event bus")

    Component(crosshair, "CrosshairWidget", "Aim feedback", "Spread, hit marker, non-lethal indicator")
    Component(radar, "RadarWidget [ENHANCED]", "Minimap radar", "Event-driven — reacts to RadarRegistry change events\nContact types: guard, hostage, objective, item")
    Component(vitals, "PlayerVitalsWidget [ENHANCED]", "Health / stamina / armor", "Event-driven via GAS attribute delegates")
    Component(weapon, "WeaponHUD [ENHANCED]", "Clip / ammo / attachments\nHold-to-open menu", "Driven by weapon state machine events")
    Component(stealth, "StealthRatingWidget [NEW]", "Alert count, kill type, detection events", "Live stealth rating feed from UStealthRatingComponent")
    Component(objectives, "ObjectiveTrackerWidget [NEW]", "Active mission objectives", "Driven by AMissionManager objective-changed events")
    Component(prompt, "ContextPromptWidget [ENHANCED]", "Pickup, NPC interaction, cover prompts", "Driven by proximity events; shows action bindings")
    Component(eventfeed, "EventFeedWidget [ENHANCED]", "On-screen event log", "Body discovered, alert raised, objective completed")
    Component(debug, "PlayerDebugWidget", "Dev-only AI/stealth debug overlay", "Awareness values, LKP marker, noise radius")

    Component(eventbus, "UIEventBus [NEW]", "UGameplayEventRouter", "Central event router — all gameplay state changes flow through here, no tick polling")
  }

  Container_Ext(core, "SneakGearCore", "IPlayerUIDataSource, event type definitions")
  Container_Ext(sneakgear, "SneakGear", "Gameplay events: weapon state, stealth, mission, NPC")
  Container_Ext(sneakgearai, "SneakGearAI", "Guard state changes (alert raised, body found)")

  Rel(hud, crosshair, "Contains")
  Rel(hud, radar, "Contains")
  Rel(hud, vitals, "Contains")
  Rel(hud, weapon, "Contains")
  Rel(hud, stealth, "Contains")
  Rel(hud, objectives, "Contains")
  Rel(hud, prompt, "Shows contextually")
  Rel(hud, eventfeed, "Contains")
  Rel(hud, debug, "Contains in dev builds")

  Rel(crosshair, eventbus, "Subscribes to weapon fire / hit events")
  Rel(radar, eventbus, "Subscribes to radar contact events")
  Rel(vitals, eventbus, "Subscribes to GAS attribute change events")
  Rel(weapon, eventbus, "Subscribes to weapon state machine events")
  Rel(stealth, eventbus, "Subscribes to stealth rating events")
  Rel(objectives, eventbus, "Subscribes to mission objective events")
  Rel(prompt, eventbus, "Subscribes to proximity and interaction events")
  Rel(eventfeed, eventbus, "Subscribes to all named game events")

  Rel(sneakgear, eventbus, "Publishes gameplay events to")
  Rel(sneakgearai, eventbus, "Publishes AI state events to")
  Rel(eventbus, core, "Uses event type definitions from")
```

---

## Summary of New Systems by Goal Checklist Item

| Goal | Target System |
|------|--------------|
| Walk / run / sprint / crouch / prone / vault / climb | `Player [ENHANCED]` — movement stance machine |
| Contextual cover: snap, lean, blind-fire, corner | `Player [ENHANCED]` — cover state component |
| Advanced camera | `Player [ENHANCED]` — shoulder swap, obstruction, cinematic |
| AI perception: hearing, peripheral, LKP | `SneakGearAI` — `UGuardPerceptionRouter`, `UGuardMemoryComponent` |
| AI behavior: investigate, search, reinforce | `SneakGearAI` — `UGuardBehaviorStateComponent`, `UAlertPropagationSubsystem` |
| Stealth feedback: awareness meter, body discovery | `Stealth [NEW]` — `UBodyDiscoverySubsystem`; `SneakGearUI` — stealth rating widget |
| Stealth tools: silencers, distractions, decoys, EMP | `Stealth [NEW]` — `UNoiseEmitterComponent`, tool item definitions |
| Non-lethal: chokeout, stun, tranq, restraints | `Combat [REFACTORED]` — `UNonLethalSystem` |
| Combat depth: damage falloff, armor, penetration | `Combat [REFACTORED]` — `UDamageSystem`, armor GAS attribute |
| Weapons: upgrades, attachments, ammo types | `Inventory / Loadout [ENHANCED]` — `UWeaponAttachmentComponent` |
| Inventory + loadout: weight, encumbrance | `Inventory / Loadout [ENHANCED]` — `UPlayerLoadoutComponent` |
| Missions: objectives, stealth ratings, penalties | `Mission [REFACTORED]` — `AMissionManager`, `UStealthRatingComponent` |
| NPC: hostage, escort, intel | `NPC [NEW]` — `ANPCCharacter`, `UNPCInteractionComponent` |
| World: day/night, weather | `World [NEW]` — `UDayNightCycleSubsystem`, `UWeatherSubsystem` |
| Audio: footsteps, barks, suppressed shots | `Audio [NEW]` — `UAudioEventSubsystem` + MetaSound |
| Save / load and checkpointing | `Mission [REFACTORED]` — `USaveLoadSubsystem` |
| UI: event-driven, interaction prompts | `SneakGearUI [ENHANCED]` — `UIEventBus`, context prompt widget |
| Optimization: AI, lighting, streaming | `Radar [ENHANCED]` event-driven; `World [NEW]` streaming; AI LOD budgets |
