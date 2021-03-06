# dynamic_graph_manager

## What is it

Dynamic graph "glue code" responsible for the instanciation of the graph and the
python interpreter. Provide a ROS server in order to create distant ROS clients.

## Authors

- Maximilien Naveau
- Julian Viereck
- Andrea Delprete

## Copyrights

Copyright (c) 2019, New York University and Max Planck Gesellschaft.

## License

License BSD-3-Clause

## Installation:

This is a ros package so it should be in a ROS environment.
One can still test its compilation by using the following instruction:

`cd dynamic_graph_manager
mkdir _build
cd _build
cmake ..
make -j8`

## Usage:

Inherite from the class DynamicGraphManager and overload the three functions
responsible for the hardware:

	 `virtual void initialize_hardware_communication_process()`

	 `virtual void get_sensors_to_map(VectorDGMap&)`

and

	`virtual void set_motor_controls_from_map(const VectorDGMap&)`

## Documentation:

See demo and unit tests for more information on the API.
Doxygen informations are available by calling:

	`catkin_make -DBUILD_DOCUMENTATION=ON`
