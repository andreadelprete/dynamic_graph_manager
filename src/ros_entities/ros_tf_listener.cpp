/**
 * @file ros_tf_listener.cpp
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
 * @date 2019-05-22
 */

#include "dynamic_graph_manager/ros_init.hh"
#include "ros_entities/ros_tf_listener.hh"

#include <dynamic-graph/factory.h>

namespace dynamic_graph
{
  DYNAMICGRAPH_FACTORY_ENTITY_PLUGIN(RosTfListener, "RosTfListener");
}
