#!/usr/bin/env python

"""@package dynamic_graph_manager

@file robot_pose_publisher.py
@author Maximilien Naveau (maximilien.naveau@gmail.com)
@license License BSD-3-Clause
@copyright Copyright (c) 2019, New York University and Max Planck Gesellshaft.
@date 2019-05-22

@brief Listens to TransformStamped messages and publish them to tf

"""

import rospy

import tf
import sensor_msgs.msg

frame = ''
childFrame = ''

#DEPRECATED. Robot Pose is already being published
def pose_broadcaster(msg):
    translation = msg.position[0:3];
    rotation = tf.transformations.quaternion_from_euler(msg.position[3], msg.position[4], msg.position[5])
    tfbr = tf.TransformBroadcaster()
    tfbr.sendTransform(translation, rotation,
                       rospy.Time.now(), childFrame, frame)

if __name__ == '__main__':
    rospy.init_node('robot_pose_publisher', anonymous=True)

    frame = rospy.get_param('~frame', 'odom')
    childFrame = rospy.get_param('~child_frame', 'base_link')
    topic = rospy.get_param('~topic', 'joint_states')
    
    rospy.Subscriber(topic, sensor_msgs.msg.JointState, pose_broadcaster)
    rospy.spin()
