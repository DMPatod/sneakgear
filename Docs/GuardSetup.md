# Guard Setup

Guards are no longer supported through legacy patrol fallback logic. A valid Behavior Tree is required.

## Required Setup

1. Place a `GuardCharacter` or spawn one through `AGuardSpawner`
2. Assign a `BehaviorTreeAsset`
3. Ensure the BT has a blackboard
4. Assign a `PatrolPath` if the guard should patrol

## Recommended Blackboard Keys

| Key | Type |
|-----|------|
| `TargetActor` | Object |
| `Awareness` | Float |
| `AwarenessState` | Int |
| `HasLineOfSight` | Bool |
| `PatrolLocation` | Vector |
| `PatrolIndex` | Int |

## Recommended Behavior Tree Shape

```text
Root
  Selector

    Sequence
      Decorator: AwarenessState == Alerted
      Move To(TargetActor)

    Sequence
      Decorator: Awareness > 0.0
      Wait(0.2)

    Sequence
      Decorator: Awareness <= 0.0
      Loop
        Set Next Patrol Point
        Move To(PatrolLocation)
        Wait(0.2)
```

## Notes

- `AGuardAIController` updates guard blackboard values continuously
- Patrol path data now comes only from `APatrolPath.Waypoints`
- Legacy `GuardPatrolData` and `WaypointAction` systems were removed
