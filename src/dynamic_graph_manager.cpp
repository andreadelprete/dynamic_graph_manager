/**
 * \file dynamic_graph_manager.cpp
 * \brief The Control Manager
 * \author Maximilien Naveau
 * \date 2018
 *
 * Implementation of the DynamicGraphManager class
 *
 */

#include <dynamic_graph_manager/ros_init.hh>
#include <dynamic_graph_manager/dynamic_graph_manager.hh>

using namespace dynamic_graph;

DynamicGraphManager::DynamicGraphManager()
{
  // Upon construction the graph is inactive
  params_.reset();
  is_dynamic_graph_stopped_ = true;
  is_dynamic_graph_stopped_ = true;
  pid_dynamic_graph_process_ = 0;
  pid_hardware_communication_process_ = 0;
}

DynamicGraphManager::~DynamicGraphManager()
{
  ros_service_start_dg_.shutdown();
  ros_service_stop_dg_.shutdown();
  ros_shutdown();
  if(thread_dynamic_graph_ && thread_dynamic_graph_->joinable())
  {
    thread_dynamic_graph_->join();
  }
  if(thread_hardware_communication_ &&
     thread_hardware_communication_->joinable())
  {
    thread_hardware_communication_->join();
  }
}

void DynamicGraphManager::initialize(YAML::Node param){

  // Upon initialization the graph is inactive
  is_dynamic_graph_stopped_ = true;

  // copy the yaml node for further use
  params_ = param ;

  pid_dynamic_graph_process_ = 0;
  pid_hardware_communication_process_ = 0;
}

void DynamicGraphManager::run()
{
  pid_t child_pid = fork();
  if(child_pid == 0) // child process
  {
    pid_dynamic_graph_process_ = getpid();
    pid_hardware_communication_process_ = getppid();

    initialize_dynamic_graph_process();
    run_dynamic_graph_process();
    wait_stop_dynamic_graph();
    exit(0);
  }else if(child_pid > 0) // parent process
  {
    pid_dynamic_graph_process_ = child_pid;
    pid_hardware_communication_process_ = getpid();

    initialize_hardware_communication_process();
    run_hardware_communication_process();
  }else
  {
    throw(std::runtime_error("DynamicGraphManager::run(): the fork failed"));
  }
}

void DynamicGraphManager::wait_start_dynamic_graph()
{
  while(is_dynamic_graph_stopped_)
  {
    usleep(1000);
  }
}

void DynamicGraphManager::wait_stop_dynamic_graph()
{
  while(!is_dynamic_graph_stopped_)
  {
    usleep(1000);
  }
  if(thread_dynamic_graph_ && thread_dynamic_graph_->joinable())
  {
    thread_dynamic_graph_->join();
  }
}

void DynamicGraphManager::wait_stop_hardware_communication()
{
  while(!is_hardware_communication_stopped_)
  {
    usleep(1000);
  }
  if(thread_hardware_communication_ &&
     thread_hardware_communication_->joinable())
  {
    thread_hardware_communication_->join();
  }
}


bool DynamicGraphManager::has_dynamic_graph_process_died()
{
  bool is_dg_proc_dead = false;
  int status = 0;
  pid_t p = 0;
  p = waitpid(pid_dynamic_graph_process_, &status, WNOHANG);
  if(p == 0)
  {
    is_dg_proc_dead = false;
  }
  else if(p > 0)
  {
    is_dg_proc_dead = true;
  }
  else
  {
    throw(std::runtime_error(std::string("DynamicGraphManager::") +
      "has_dynamic_graph_process_died(): waitpid failed"));
  }
  return is_dg_proc_dead;
}

void DynamicGraphManager::initialize_dynamic_graph_process()
{
  ros::NodeHandle& ros_node_handle = ros_init("dynamic_graph");
  ros_python_interpreter_.reset(
        new dynamic_graph::RosPythonInterpreter(ros_node_handle));
  start_ros_service(ros_node_handle);
  device_.reset(new Device(params_["device"]["name"].as<std::string>(),
      params_["device"]));
}

void DynamicGraphManager::run_dynamic_graph_process()
{
  // launch the real time thread and ros spin
  thread_dynamic_graph_.reset(new std::thread(
        &DynamicGraphManager::dynamic_graph_real_time_loop, this));
}

void DynamicGraphManager::run_hardware_communication_process()
{
  ros_init("hardware_communication");
  // launch the real time thread
  thread_hardware_communication_.reset(new std::thread(
        &DynamicGraphManager::hardware_communication_real_time_loop, this));
}

void DynamicGraphManager::start_ros_service(ros::NodeHandle& ros_node_handle)
{
  // Advertize the service to start and stop the dynamic graph
  ros_service_start_dg_ = ros_node_handle.advertiseService(
                              "start_dynamic_graph",
                              &DynamicGraphManager::start_dynamic_graph,
                              this);
  ros_service_stop_dg_ = ros_node_handle.advertiseService(
                             "stop_dynamic_graph",
                             &DynamicGraphManager::stop_dynamic_graph,
                             this);
  // advertize the ros::services associated to the python interpreter
  ros_python_interpreter_->start_ros_service();
}

void DynamicGraphManager::dynamic_graph_real_time_loop()
{
  std::cout << "dynamic graph thread started" << std::endl;
  is_dynamic_graph_stopped_ = false;
  // local variables of the sensors map
  VectorDGMap sensors_map, motor_controls_map;
  sensors_map = device_->sensors_map_;
  motor_controls_map = device_->motor_controls_map_;
  wait_start_dynamic_graph();

  while(!is_dynamic_graph_stopped_ && ros::ok())
  {
    // read the sensor from the shared memory
    shared_memory::get("DynamicGraphManager", "sensors_map", sensors_map);

    // call the dynamic graph
    device_->set_sensors_from_map(sensors_map);
    device_->execute_graph();
    device_->get_controls_to_map(motor_controls_map);

    // write the command to the shared memory
    shared_memory::set("DynamicGraphManager", "motor_controls_map",
                       motor_controls_map);
  }
  is_dynamic_graph_stopped_ = true;
  std::cout << "dynamic graph thread stopped" << std::endl;
}

void DynamicGraphManager::hardware_communication_real_time_loop()
{
  std::cout << "hardware communication loop started" << std::endl;
  is_hardware_communication_stopped_ = false;
  // setup the maps from the yaml
  VectorDGMap sensors_map, motor_controls_map;
  parse_yaml_node(params_, sensors_map, motor_controls_map);
  assert(!is_hardware_communication_stopped_ && "The loop is started");
  assert(ros::ok() && "Ros has to be initialized");
  while(!is_hardware_communication_stopped_ && ros::ok())
  {
    // call the sensors
    get_sensors_to_map(sensors_map);
    // write the sensors to the shared memory
    shared_memory::set("DynamicGraphManager", "sensors_map", sensors_map);

    // sleep 1ms
    usleep(1000);

    // read the command from the shared memory
    shared_memory::get("DynamicGraphManager", "motor_controls_map",
                       motor_controls_map);

    // send the command to the motors
    set_motor_controls_from_map(motor_controls_map);
  }
  is_hardware_communication_stopped_ = true;
  std::cout << "hardware communication loop stopped" << std::endl;
}
