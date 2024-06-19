# Raylib Test

## Target: gravity_sim

This is a simple gravitational field simulation
that does not take general relativity into account.
This simulation is capable of tracking _**3,000
rigib bodies**_ with their own gravitational field
at _**130~ FPS**_.

### Keybindings

- `T`: Slow motion. This decreases the error term O(dt\*dt)
  by 2,500 times (since I use verlet integration.)
- `S`: Spawn a rigid body with stochastic mass and radius.

## Target: graph

This is a simple graph visualizer built on top of Boost Graph
Library (BGL). Clicking on nodes enables "connection mode", when
you can click on another node to connect them together.

### Keybindings

- `T`: Zap the camera toward the first vertex.
