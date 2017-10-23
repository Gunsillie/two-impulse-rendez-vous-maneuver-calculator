# two-impulse-rendez-vous-maneuver-calculator

This repository was created for the mid-term assignment A2 of Professor Genya Ishigami's Space Exploration Engineering course of Fall 2017, at Keio University. However, you can use it as freely as the attached license allows.

## How to use the program

The program works very simply: it takes a set of arguments as inputs, and creates a set of files containing its outputs.
The arguments that can be passed to the program are, in order of input when launched as a command line:
- **T**: the desired time of rendez-vous between the chaser and the target (type: double, unit: seconds).
- **time_step**: the desired time step between two points of data in the output (type: double, unit: seconds).
- **altitude_target**: the altitude of the target wrt. the surface of the Earth (type: double, unit: meters).
- **w**: the angular speed of the target in its rotation around the Earth (type: double, unit: rad/s=).
- **x0**: x coordinate of the chaser at t=0 (type: double, unit: meters).
- **y0**: ibid.
- **z0**: ibid.

The different files are created in the same folder as the executable file. They are formatted specifically to be used in TiKz plots, like those of the report.pdf file.
- **output_tikz_x_t.txt**, **output_tikz_y_t.txt** and **output_tikz_z_t.txt** give the values of all 3 coordinates of the chaser as a function of time (starting from t=0 and ending at t=T).
- **output_tikz_x_z.txt** gives the values of all (x,z) points of data (useful when the problem is planar and y is constant).
- **output_tikz_x_y_z.txt** gives the values of all (x,y,z) points of data.


## Notes

- Because **altitude_target** and **w** can be derived from each other, you must set one of them to 0. If both parameters are given a non-zero value, **w** will be calculated again based on the **altitude_target** input by the user.
- The movement of the target around the Earth is assumed to be circular.
- The frame of reference for all coordinates is target-centered and target-fixed.
- The resolution of the problem is done by means of the Clohessy-Wiltshire equations; be aware of the possible imprecisions due to linearization.
- For additional information, please refer to the report.pdf file.
