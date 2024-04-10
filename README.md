# Raylib Test

This is a simple gravitational field simulation
that does not take general relativity into account.
This simulation is capable of tracking _**3,000
rigib bodies**_ with their own gravitational field
at _**130~ FPS**_.

## Keybindings

- `T`: Slow motion. This decreases the error term O(dt*dt)
by 2,500 times (since I use verlet integration.)
- `S`: Spawn a rigid body with stochastic mass and radius.
