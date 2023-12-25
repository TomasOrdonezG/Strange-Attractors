# Strange-Attractors
3D Visualization of strange attractors

# Controls
| Key | Action |
|-----|--------|
|  S  | Open/close general attractor settings |
|  C  | Open/close trail colour settings |
| ESC | Close the window and end porgram |
| 1-6 | Switch attractor |

# Attractors
| Number key | Attractor name |
| ------- | ----------- |
| 1 | Lorenz  |
| 2 | Banlue  |
| 3 | Halvorsen  |
| 4 | Aizawa  |
| 5 | Lu Chen  |
| 6 | Genesio  |

# General Settings
Key `S` to open/close.

| Setting | Description |
| ------- | ----------- |
| a,b,c | Three parameters that describe each attractor. Some attractors are not defined in terms of all three |
| Zoom | Zoom level in the Z direction (forward) |
| X/Y/Zrot | Rotation speed in each axis |
| dt | Delta time, controls the speed of the simulation (will change the appearance of the trail length) |
| length | Number of segments the trail is made up of (higher trail length might cause animation speed to appear slower) |

# Colour settings
Key `C` to open/close.

Four sections controlling the red, green, blue, and alpha channels. Each section contains a left and right slider, the left sliders control the channels of the left end (tail) of the gradient, and the right sliders control the channels of the right end (head) of the gradient. On the top you can see a preview of what the gradient will look like on the trail. Press `C` again to close the colour settings.

