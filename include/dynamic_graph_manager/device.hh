/**
 * \file device.hh
 * \brief The robot entity
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file declares the input and output of the DynamicGraph
 */

#ifndef DEVICE_HH
#define DEVICE_HH

#include <yaml-cpp/yaml.h>

#include <dynamic-graph/linear-algebra.h>
#include <dynamic-graph/entity.h>
#include <dynamic-graph/all-signals.h>

/* short cut of the namespace */
namespace dg = dynamicgraph;

#include "dynamic_graph_manager/periodic-call.hh"
#include <dynamic_graph_manager/matrix-geometry.hh>

namespace dynamic_graph {
  typedef dynamicgraph::Signal<dg::Vector,int>  OutSignal;
  typedef dynamicgraph::SignalPtr<dg::Vector,int>  InSignal;
  typedef std::map<std::string, OutSignal* > DeviceOutSignalMap;
  typedef std::map<std::string, InSignal* > DeviceInSignalMap;
  typedef std::map<std::string, std::vector<double> > VectorDoubleMap;
  typedef std::map<std::string, dg::Vector > VectorDGMap;

  class Device: public dynamicgraph::Entity
  {
  public:

    static const std::string CLASS_NAME;

    /**
     * @brief getClassName is an overloaded function from the class Entity.
     * It is used to access the class name and do
     * @return the name of the device class
     */
    virtual const std::string& getClassName(void) const {
      return CLASS_NAME;
    }

    /**
     * @brief Device is the constructor. The name allow the DynamicGraph to
     * identify the entity
     * @param input_name is the entity name
     * @param params is the yaml file used to initialize the device
     */
    Device(const std::string& input_name, const YAML::Node& params);

    /**
     * @brief ~Device is a default destructor that might overloaded
     */
    virtual ~Device();

    /**
     * @brief parse_yaml_file fill in the internal maps for sensors and controls.
     */
    void parse_yaml_node(const YAML::Node& sensors_and_controls);

    /**
     * @brief get_sensor_from_map
     * @param sensors
     */
    virtual void set_sensors_from_map(const VectorDoubleMap& sensors);

    /**
     * @brief execute_graph is a fonction that execute the graph.
     *
     * In order it does:
     *  - Execute a first set of synchrounous commands.
     *  - Execute the graph.
     *  - Execute a second set of synchronous commands.
     *
     */
    virtual void execute_graph();

    /**
     * @brief set_control_to_map is a parser that feed the map "controls" with
     * the output of the DynamicGraph.
     * @param controls is the the map containing the controls.
     */
    virtual void get_controls_to_map(VectorDoubleMap& motor_controls);

    /****************************************************************
     * DEVICE OUPUT SIGNALS // INPUT OF THE GRAPH <=> SENSOR VALUES *
     ****************************************************************/

    /**
     * @brief sensors_out_ is a map of device output signals. They represent
     * all the sensors belonging to the robot.
     */
    DeviceOutSignalMap sensors_out_;

    /**
      * @brief sensors_map_ is a map of dg::Vector. They represent
      * all the sensors data measured on the robot. It is a internal copy of the
      * sensor data that is used for type conversion from std::vector<double>
      * to dg::Vector
      */
    VectorDGMap sensors_map_;

    /*******************************************************
     * DEVICE INPUT SIGNALS // OUTPUT OF THE CONTROL GRAPH *
     *******************************************************/

    /**
     * @brief motor_control_in_ is the output motor control for each joint.
     * Feeding this signal *IS MANDATORY* otherwise accessing this data will
     * make the process crash.
     */
    DeviceInSignalMap motor_controls_in_;

    /**
     * @brief motor_controls_map_ is a map of dg::Vector. They represent
      * all the controls to be sent to the robot. It is a internal copy of the
      * controls data that is used for type conversion from dg::Vector
      * to std::vector<double>
     */
    VectorDGMap motor_controls_map_;

  protected:
    /**
     * @brief periodic_call_before_ handle the *synchronous* command call on the
     * device between getting the sensor data and sending the commands.
     * Typically used when one wants to evaluate a signal that is not plugged.
     */
    PeriodicCall periodic_call_before_;

    /**
     * @brief periodic_call_after_ handle the *synchronous* command call on the
     * device between getting the sensor data and sending the commands.
     * Typically used when one wants to evaluate a signal that is not plugged.
     */
    PeriodicCall periodic_call_after_;

    /**
     * @brief params is a YAML node that allow the creation of a modular device
     */
    YAML::Node params_;
  };

} // namespace dynamic_graph


#endif /* #ifndef DEVICE_HH */



