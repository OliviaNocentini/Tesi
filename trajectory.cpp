//We first need to convert our optimization object into the Trajectory class

#include <mav_trajectory_generation/trajectory.h>

mav_trajectory_generation::Trajectory trajectory;
opt.getTrajectory(&trajectory);
