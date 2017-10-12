#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/TwistStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <math.h>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/transform_listener.h>
#include "vel_cont.hpp"
#include "angles/angles.h"
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include "tf/transform_datatypes.h"

#define _KP_LINEAR .1
#define _KP_ANGULAR .1
#define alpha  1.29   //  angolo in radianti sarebbe 73,5 gradi scegliendo il field of view sopra orizzonte pari a 5 gradi.

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

nav_msgs::Odometry current_pose_iRobot;

void pose_cb_iRobot(const nav_msgs::Odometry::ConstPtr& msg){
    current_pose_iRobot = *msg;
}


//geometry_msgs::Pose desired_goal_pose;

geometry_msgs::TransformStamped transformStamped;

//costruttore della classe 

Movements::Movements(ros::NodeHandle& nh)
{
  

  set_vel_pub = nh.advertise<geometry_msgs::TwistStamped>
    ("mavros/setpoint_velocity/cmd_vel", 10);	 
    
  
 // goal_sub = nh.subscribe<geometry_msgs::Pose>
  //  ("/goal", 10, &Movements::goal_cb, this);
    
// this->set_desired_pos(0,0,1,0,0,0);
 
 
}

void Movements::evaluate_error(float linear_error[3],float angular_error[3])
{
  
  //definisco le posizioni del mio drone 
  
  float x_drone=transformStamped.transform.translation.x;
  float y_drone=transformStamped.transform.translation.y;
  float z_drone=transformStamped.transform.translation.z;
  
  float roll_drone(0),pitch_drone(0),yaw_drone;
  float q0_drone=transformStamped.transform.rotation.x;
  float q1_drone=transformStamped.transform.rotation.y;
  float q2_drone=transformStamped.transform.rotation.z;
  float q3_drone=transformStamped.transform.rotation.w;
  
  
  // definisco le posizioni del centro del fov della camera
  
  
  float fov_x=z_drone*tan(alpha);   // la funzione tan prende angoli in radianti
  float fov_y=y_drone;
  float fov_z=0;
  
  
  // per quanto riguarda la rotazione il fov segue esattamente il centro del drone
  
  float roll_fov(0),pitch_fov(0),yaw_fov;
  float q0_fov=q0_drone;
  float q1_fov=q1_drone;
  float q2_fov=q2_drone;
  float q3_fov=q3_drone;
  
  
    
  yaw_drone= tf::getYaw(transformStamped.transform.rotation);
  
  
  yaw_fov=yaw_drone;
  
  //la posizione desiderata è quella che assume il centro del mio roomba.
  
  float x_des = current_pose_iRobot.pose.pose.position.x;
  float y_des = current_pose_iRobot.pose.pose.position.y;
  float z_des = current_pose_iRobot.pose.pose.position.z;

  float roll_des = current_pose_iRobot.pose.pose.orientation.x;
  float pitch_des = current_pose_iRobot.pose.pose.orientation.y;
  float yaw_des = current_pose_iRobot.pose.pose.orientation.z;
  
  linear_error[0] = x_des - fov_x;
  linear_error[1] = y_des - fov_y;
  linear_error[2] = z_des - fov_z;

  angular_error[0] =angles::normalize_angle(roll_des - roll_fov);
  angular_error[1] =angles::normalize_angle(pitch_des - pitch_fov);
  angular_error[2] =angles::normalize_angle( yaw_des -yaw_fov);
}

float Movements::norma_linear_error()
{
  float linear_error[3];
  float angular_error[3];
  this->evaluate_error(linear_error, angular_error);
  float norma=0;
  for(int i=0; i<3 ; i++)
    norma += pow(linear_error[i],2);
 
  norma=sqrt(norma);
  
  return norma;
}



float Movements::norma_angular_error()
{
  float linear_error[3];
  float angular_error[3];
  this->evaluate_error(linear_error, angular_error);
  float norma=0;
  for(int i=0; i<3 ; i++)
    norma += pow(angular_error[i],2);
 
  norma=sqrt(norma);
  
  return norma;
}

/*void Movements::set_desired_pos(float x, float y, float z, float roll, float pitch, float yaw)
{
    desired_pos[0]=x;
    desired_pos[1]=y;
    desired_pos[2]=z;
    desired_att[0]=roll;
    desired_att[1]=pitch;
    desired_att[2]=yaw;
}*/


void Movements::set_command()
{
  
 
  float linear_error[3];
  float angular_error[3];
  
  this->evaluate_error(linear_error,angular_error);
  
  geometry_msgs::TwistStamped command;

  float Kp_linear=_KP_LINEAR;
  
  // set linear command
  
  command.twist.linear.x = Kp_linear * linear_error[0];
  command.twist.linear.y = Kp_linear * linear_error[1];
  command.twist.linear.z = Kp_linear * linear_error[2];
  
  float Kp_angular=_KP_ANGULAR;
  
  // set angular command
  command.twist.angular.x = 0;//Kp * angular_error[0];
  command.twist.angular.y = 0;//Kp * angular_error[1];
  command.twist.angular.z = Kp_angular * angular_error[2];

  // send the command
  
  std::cout << "Twist Command: " << std::endl; 
  std::cout << "linear = " << command.twist.linear.x << ", " << command.twist.linear.y << ", " << command.twist.linear.z << std::endl;
  std::cout << "angular = " << command.twist.angular.x << ", " << command.twist.angular.y << ", " << command.twist.angular.z << std::endl;
  set_vel_pub.publish(command);
  
  return ;  

}


/*void Movements::goal_cb(const geometry_msgs::Pose::ConstPtr& msg)
{
  
  double yaw = tf::getYaw(transformStamped.transform.rotation);
  
  // transform quaternion to euler
  //quat2eul(&roll, &pitch, &yaw, q0, q1, q2, q3);
//  this->set_desired_pos(transformStamped.transform.translation.x,transformStamped.transform.translation.y, transformStamped.transform.translation.z, 0, 0, yaw);
  
  std::cout << "desired pose: " << transformStamped.transform.translation.x << ", " << transformStamped.transform.translation.y << ", " << transformStamped.transform.translation.z << ", " <<  yaw << std::endl;
}*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

  float soglia_wp=0.5;
  
  
  std::vector<float> p0={0,0,1};
  
  
  
 
    ros::init(argc, argv, "follow");
    ros::NodeHandle nh;
    
    
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");
  
   // ros::Subscriber drone_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
      //     ("mavros/local_position/pose", 10, drone_pose_cb);
    ros::Subscriber iRobot_pose_sub = nh.subscribe<nav_msgs::Odometry>
           ("iRobot/odom", 10, pose_cb_iRobot);
    ros::Publisher set_pose_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);	

     //instanziare l'oggetto mov nella classe movements cioè alloco in memoria il mio oggetto.

    Movements mov(nh);

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);
    

    
    geometry_msgs::PoseStamped set_pose;
    
    
   std::cout << "Definisco fino a dove si deve alzare il mio drone" <<std::endl;

   set_pose.pose.position.x=p0[0];
   set_pose.pose.position.y=p0[1];
   set_pose.pose.position.z=p0[2];
    
   mavros_msgs::SetMode offb_set_mode;
  offb_set_mode.request.custom_mode = "OFFBOARD";

  mavros_msgs::CommandBool arm_cmd;
  arm_cmd.request.value = true;
   
   
  ros::Time last_request = ros::Time::now();
  
  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);
  
     int task=1;


    while(ros::ok()){
      
       if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.success){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }
       
      
    transformStamped.header.stamp = ros::Time::now();
  
  //set the name of the parent frame of the link we're creating and the name of the child
  
        transformStamped.header.frame_id = "ENU";
        transformStamped.child_frame_id = "base_link";
    try{
      transformStamped = tfBuffer.lookupTransform("ENU", "base_link",
                               ros::Time(0));
    }
    catch (tf2::TransformException &ex) {
      ROS_WARN("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }
    
 
   if(task==1)
     
   {

      set_pose_pub.publish(set_pose);
   	
   
        if((fabs(set_pose.pose.position.x-transformStamped.transform.translation.x) < soglia_wp) && 
	  ( fabs(set_pose.pose.position.y-transformStamped.transform.translation.y) < soglia_wp) && 
	  ( fabs(set_pose.pose.position.z-transformStamped.transform.translation.z) < soglia_wp))
      
	{
	    // faccio prendere al roomba la posizione del centro del field of view.
	  
	  std::cout << " finito primo task" <<std::endl;
	  task++;
	 
     
        }
        
   }
    
   if(task==2)
     
     
   {
      
     std::cout << " inizia secondo task" <<std::endl;
     mov.set_command();
     
     
   } 
         ros::spinOnce();
         rate.sleep();
	 
   }
          
    
      
    return 0;   
    
    
}