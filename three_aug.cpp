#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>


struct point{
  
std::vector<float> position;

} waypoint[3];

geometry_msgs::PoseStamped current_pose;

void pose_cb(const geometry_msgs::PoseStamped::ConstPtr& msg){
    current_pose = *msg;
}

geometry_msgs::TwistStamped current_vel;

void vel_cb(const geometry_msgs::TwistStamped::ConstPtr& msg){
    current_vel = *msg;
}


mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

int main(int argc, char **argv)
{

  float soglia=0.5;
  float soglia_vel=0.1;
  
  std::vector<float> p0={0,0,5}; 
  std::vector<float> p1={3,0,5}; 
  std::vector<float> p2={0,0,2};
  
  waypoint[0].position=p0;
  waypoint[1].position=p1;
  waypoint[2].position=p2;
  
  /*std::cout << "p0x:"<<"\n"<< waypoint[0].position[0] << "\n" << "p0y:"<< "\n"<<waypoint[0].position[1] <<"\n"<< "p0z:"<<"\n"<< waypoint[0].position[2]  << std::endl;
  std::cout << "p1x:"<<"\n"<< waypoint[1].position[0] << "\n" << "p1y:"<< "\n"<<waypoint[1].position[1] <<"\n"<< "p1z:"<<"\n"<< waypoint[1].position[2]  << std::endl;
  std::cout << "p2x:"<<"\n"<< waypoint[2].position[0] << "\n" << "p2y:"<< "\n"<<waypoint[2].position[1] <<"\n"<< "p2z:"<<"\n"<< waypoint[2].position[2]  << std::endl;*/
  
  
  
    ros::init(argc, argv, "july_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
   
    
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
            ("mavros/local_position/pose", 10, pose_cb);
    ros::Subscriber vel_sub = nh.subscribe<geometry_msgs::TwistStamped>
            ("mavros/local_position/velocity", 10, vel_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    ros::Publisher set_vel_pub = nh.advertise<geometry_msgs::TwistStamped>
            ("mavros/setpoint_velocity/cmd_vel", 10);	
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
    
    geometry_msgs::PoseStamped set_pose;
    
    std::cout << "Definisco il mio primo obiettivo" <<std::endl;

   set_pose.pose.position.x=p0[0];
   set_pose.pose.position.y=p0[1];
   set_pose.pose.position.z=p0[2];
   
   geometry_msgs::TwistStamped set_vel;
   
   set_vel.twist.linear.x=0.5;
   set_vel.twist.linear.y=0.5;
   set_vel.twist.linear.z=0.5;
   
   
   int waypoint_index = 0;
   
   std::cout << "pos x"<<"\n"<< set_pose.pose.position.x << "\n" << " pos y"<< "\n"<<set_pose.pose.position.y<<"\n"<< "pos z"<<"\n"<< set_pose.pose.position.z << std::endl;
   
    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(set_pose);
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
        
        
        local_pos_pub.publish(set_pose);
	set_vel_pub.publish(set_vel);
	
	
        
	if((fabs(set_pose.pose.position.x-current_pose.pose.position.x) < soglia) && 
	  ( fabs(set_pose.pose.position.y-current_pose.pose.position.y) <soglia) && 
	  ( fabs(set_pose.pose.position.z-current_pose.pose.position.z) < soglia))
      
	{
	    waypoint_index++;
	    set_pose.pose.position.x=waypoint[waypoint_index%3].position[0];
	    set_pose.pose.position.y=waypoint[waypoint_index%3].position[1];
	    set_pose.pose.position.z=waypoint[waypoint_index%3].position[2];
	    
	    
	    if((fabs(set_vel.twist.linear.x - current_vel.twist.linear.x) > soglia_vel)&&
	   (fabs(set_vel.twist.linear.y - current_vel.twist.linear.y) >soglia_vel) &&
	   (fabs(set_vel.twist.linear.z - current_vel.twist.linear.z) >soglia_vel))
	      
	    {  
	     
	     current_vel.twist.linear.x=set_vel.twist.linear.x;
	     current_vel.twist.linear.y=set_vel.twist.linear.y;
	     current_vel.twist.linear.z=set_vel.twist.linear.z;
	     
	     std::cout <<  "- new set vel (x,y,z):" << 
	      current_vel.twist.linear.x << ", " 
	    <<current_vel.twist.linear.y<<", "
	    <<current_vel.twist.linear.z << std::endl;
	      
	    std::cout << waypoint_index << "- new set pose (x,y,z):" << 
	    set_pose.pose.position.x << ", " 
	    <<set_pose.pose.position.y<<", "
	    <<set_pose.pose.position.z << std::endl;
	    
	    }
	}
	
	if((fabs(set_vel.twist.linear.x - current_vel.twist.linear.x) > soglia_vel)&&
	   (fabs(set_vel.twist.linear.y - current_vel.twist.linear.y) >soglia_vel) &&
	   (fabs(set_vel.twist.linear.z - current_vel.twist.linear.z) >soglia_vel)){
	     
	     
	     current_vel.twist.linear.x=set_vel.twist.linear.x;
	     current_vel.twist.linear.y=set_vel.twist.linear.y;
	     current_vel.twist.linear.z=set_vel.twist.linear.z;
	     
	     std::cout <<  "- current velocity (x,y,z):" << 
	      current_vel.twist.linear.x << ", " 
	    <<current_vel.twist.linear.y<<", "
	    <<current_vel.twist.linear.z << std::endl;
	
        std::cout << "ang x:"<<"\n"<< current_vel.twist.angular.x << "\n" << " ang y:"<< "\n"<<current_vel.twist.angular.y<<"\n"<< "ang z:"<<"\n"<< current_vel.twist.angular.z << std::endl;
	 
	}
	
         ros::spinOnce();
         rate.sleep();
          
    }
    
   
      
    return 0;   
    
    
}