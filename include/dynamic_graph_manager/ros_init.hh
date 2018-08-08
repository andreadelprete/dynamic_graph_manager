/**
 * \file ros_init.hh
 * \brief a ROS singleton
 * \author Maximilien Naveau
 * \date 2018
 *
 * This file declares a ROS global singleton object. It is used to initialize
 * ROS and get a node handle specific to the DynamicGraphManager class.
 */

#ifndef ROS_INIT_HH
#define ROS_INIT_HH

# include <ros/ros.h>

namespace dynamic_graph
{
/**
 * @brief The GlobalRos struct is a structure that allows to gloabally handle
 * the ROS objects
 */
struct GlobalRos
{
  /** @brief ~GlobalRos is a specific destructor that stop the ROS activities */
  ~GlobalRos ()
  {
    if (async_spinner_)
      async_spinner_->stop();
    if (node_handle_)
      node_handle_->shutdown();
  }
  /**
   * @brief nodeHandle_ is the global node handle used by all ROS objects
   */
  boost::shared_ptr<ros::NodeHandle> node_handle_;
  /**
   * @brief ros_async_spinner_ is a ros object that handles in a global way all
   * the ros callbacks and interruption. Call ros_async_spinner_.start() in
   * order to start handling the callback in a separate thread.
   */
  boost::shared_ptr<ros::AsyncSpinner> async_spinner_;
};

/**
 * @brief rosInit is a global method that uses the structure GlobalRos.
 * Its role is to create the ros::nodeHandle and the ros::spinner once at the
 * first call. It always returns a reference to the node hanlde.
 * @return the reference of GLOBAL_ROS_VAR.node_handle_.
 */
ros::NodeHandle& ros_init ();

/**
 * @brief ros_spinner return the async_spinner_. Call dynamic_graph ros_init if
 * ros has not been initialized
 * @return the reference of GLOBAL_ROS_VAR.async_spinner_.
 */
ros::AsyncSpinner& ros_spinner ();

/**
 * @brief ros_shutdown shuts down ros and stop the ros spinner
 */
void ros_shutdown ();

} // end of namespace dynamic_graph.

#endif //! ROS_INIT_HH