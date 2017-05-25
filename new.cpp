#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#ifndef ROS_DURATION_H
#define ROS_DURATION_H
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <climits>
#include <stdint.h>
#include <duration.h>
#endif
#include <iostream>
#include <fstream>



std::vector<double> pos_in={0,0,0};
std::vector<double> pos_f={0,0,0};
std::vector<double> pos_0={0,0,0};
std::vector<double>pos_1={0,0,5};
//std::vector<double>pos_2={0,0,6};
std::vector<double> pos_trajectory={0,0,0};
int i=0;
int tf=10;
double t_0=0;
int count=0;



mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

int main(int argc, char **argv)
{
    
  
    ros::init(argc, argv, "new_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
   
    
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 5, state_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 5);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    // wait for FCU connection
    while(ros::ok() && current_state.connected){
        ros::spinOnce();
        rate.sleep();
	ROS_INFO("State Not Connected");

    }

    geometry_msgs::PoseStamped pose;

    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
	ROS_INFO("%d...",i);
    }
    
    ROS_INFO("Position Selected");

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();
    double time_secs =ros::Time::now().toSec();
    ros::Duration d ;
    double d_secs  = d.toSec();
    //d_secs= time_secs -t_0;
    

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
	
	/*for(count=0; count < 2; count++){
	  
	  if (count==0){
	    pos_f[0]=pos_0[0];
	    pos_f[1]=pos_0[1];
	    pos_f[2]=pos_0[2];
	  }
      
	  if(count ==1){
	    pos_f[0]=pos_1[0];
	    pos_f[1]=pos_1[1];
	    pos_f[2]=pos_1[2];
	  }
      
	  /*if (count ==2) {
	    pos_f[0]=pos_2[0];
	    pos_f[1]=pos_2[1];
	    pos_f[2]=pos_2[2];
	  }*/
      

      
      
      d_secs= time_secs -t_0;
	
	if (d_secs> tf)
	  d_secs = tf;
	
    float a3=0,a4=0,a5=0;

    for(i=0;i < 3;i++){
        a3=  10*(pos_f[i]-pos_in[i])/(pow(tf,3));
        a4= -15*(pos_f[i]-pos_in[i])/(pow(tf,4));
        a5=  6* (pos_f[i]-pos_in[i])/(pow(tf,5));    
        pos_trajectory[i]=pos_in[i]+a3*pow(d_secs,3)+a4*pow(d_secs,4)+a5*pow(d_secs,5);       
	
	
	std::cout << pos_trajectory[i] <<std::endl;
    
    }
    
	pose.pose.position.x = pos_trajectory[0];
   	pose.pose.position.y = pos_trajectory[1];
    	pose.pose.position.z = pos_trajectory[2];
	
	
	local_pos_pub.publish(pose);
	
      
	

	pos_in[0]=pos_f[0];
	pos_in[1]=pos_f[1];
	pos_in[2]=pos_f[2];
	
        ros::spinOnce();
        rate.sleep();
     // } // chiusura for posizioni
    
    }  // chiusura while ros
    
  
    
    return 0;
    
}   // chiusura main