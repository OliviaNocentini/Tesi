#include <mav_trajectory_generation_ros/ros_visualization.h>

visualization_msgs::MarkerArray markers;
double distance = 1.0; // Distance by which to seperate additional markers. Set 0.0 to disable.
std::string frame_id = "world";

// From Trajectory class:
mav_trajectory_generation::drawMavTrajectory(trajectory, distance, frame_id, &markers);

// From mav_msgs::EigenTrajectoryPoint::Vector states:
mav_trajectory_generation::drawMavSampledTrajectory(states, distance, frame_id, &markers)
