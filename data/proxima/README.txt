# Proxima Centauri - Our new home

A [4X](https://en.wikipedia.org/wiki/4X) game.

## General information

Turn by turn 4X game which tells the colonisation of Proxima Centauri b by humans.
The players set orders for their unit during their turn and the resolution happens at the same time for all units.

Turn resolution:
- Units move, any number of units can be on the same tile at this point
- Units fire (long range fire)
- Resolve multi unit on the same tile, if enemy resolve fight
- build actions (TODO)

### Unique Reference

Each object is defined by a unique reference, also called ID.
It looks like this: AAA00000.
The 3 first characters of an ID (also called ID string) are letters and define the unit type.
The 5 next characters are random numbers (also called ID number).
In case there is not enough digit we can use an other set of letters for the same unit type.

List of used ID string:
- DUN: Definition of a UNit.
- DTI: Definition of a map TIle.
- PLY: PLaYer.
- UNI: UNIt on the map.
- ATT: ATTack definition.
- DEF: DEFense definition.

### Units

A unit is anything which can do an action and/or is owned by a player.

A unit has a set of actions it can do, each action has a target.
A unit can have no action.

List of currently available actions:
- FIRE: ranged attack on a target tile
- MOVE: move to the target tile
- BUILD: build the target object (TODO)

### Unit attack and defense

A unit can have multiple attack and only one defense.
The attack that deals the most damage is used.

A ranged attack do not trigger a response, only the firing unit deals damage.
If two enemy units are on the same tile they engage in combat where they both attack at the same time.
The endurance used for damage calculation of both unit is the endurance they have at the begining of the fight.
The unit which lost the most endurance retreats, if they both lost the same amount of endurance then they both retreats.
Retreat to the closest player rally point.

Attack and defense are divided in soft and hard. Soft represent light weapons like guns and hard heavier weapons like cannon.
Tile cover multiplies the defense depending of the cover usage. Typical cover and cover usage is between 1 and 3.
The cover bonus applies only to unit which have at least one action point left to represent the time they need to get to cover.

Function to calculate damage:
```
endurance = endurance - (softAttack - (softDefense * tileCover * coverUsage)) - (hardAttack - (hardDefense * tileCover * coverUsage))
```

Unit damage is in function of the unit endurance. Above 80% it does full damage and bellow 20% it does no damage, in between it is linear.
```
if (endurance < 20)
  return 0
if (endurance > 80)
  return attack
return attack * endurance / 100
```

### Unit movement

Infrastructure is an indication of the mouvement cost to move in the tile.
When a unit moves we deduct the mouvement cost from its action points. A unit cannot go bellow zero action point.
```
mouvementCost = 1 / infrastructure
```

### Attack range and cost

Attack range is defined by a string.
If "X" then it can fire at any tile X spaces away, if X = 0 then only on the same tile and no ranged attack.
If "X-Y" then it can fire between X and Y.
If "X:Y" then it can fire at X or Y.

For example if an attack has a range of "0-1:4" it can fire on the same tile, and on any tile which is 1 or 4 spaces away from the unit tile.

Attack cost is the cost in action points for the unit. A cost 0 means it can attack as much as the player wants.
A cost of -1 uses all the remaining action points of the unit but the unit can still move before as long as it has at least one point remaining to fire.

### Building

Worker, engineer and factory can build objects.
TODO.

## Attack definition - def_attack.json

List of all possible units attacks.

## Defense definition - def_defense.json

List of all possible units defenses.

## Map definition - def_map.json

List of tiles type present on the map.

## Unit definition - def_unit.json

List of the different units that can be present on the map.

## Map tiles collection - map.json

List all the tiles present in the map.

## Order collection - order_PLY00001_0.json

List all the order set by a player.
There is one file per player and per turn.
PLY00001 is the player ID and 0 is the turn number.

## Player collection - player.json

List of all the player in the game with their name and ID.
The team is used to know who their units will attack.

## Unit collection - unit.json

List all units present on the map.
