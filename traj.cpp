#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <vector>

#define n 70

//definisco una struttura che definisce i punti della mia traiettoria

struct trajectory{

std::vector<float> position;
double time;

} point[n];

//Callback che salva lo stato attuale del mio autopilota


 
geometry_msgs::PoseStamped pose;
mavros_msgs::State current_state;

void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

void pose_cb(const geometry_msgs::PoseStamped::ConstPtr& msg){
    pose = *msg;
}

int main(int argc, char **argv)

{
  
  std::cout << "Sono nel main" << std::endl;
  
  
  //Definisco i 3 waypoints e le distanze tra i 3 punti
   
   int T=0.1;
   int v=1;
   
   std::vector<float> pf={0,0,0};
   std::vector<float> ps={0,0,5};
   std::vector<float> pt={2,0,5};
   
   point[0].position=pf;
   point[49].position=ps;
   point[69].position=pt;
   
   
   std::cout << "Sono dopo le posizioni" << std::endl;
  
  
    ros::init(argc, argv, "traj_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
    
    
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
    ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10, pose_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz

    ros::Rate rate(20.0);
    

    //wait for FCU connection

    while(ros::ok() && current_state.connected){
        ros::spinOnce();
        rate.sleep();
	ROS_INFO("State Connected");

    }

    //geometry_msgs::PoseStamped pose;
    

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
    
      std::cout << "Prima for" << std::endl;
    
  

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
  
    double secs =last_request.toSec();
	
   for(int i=1;i<n;i++) { 
      
       point[i].position[0]=point[i-1].position[0]+v*T;
       point[i].position[1]=point[i-1].position[1]+v*T;
       point[i].position[2]=point[i-1].position[2]+v*T;
       
       std::cout << " Stampo la mia posizione " << std::endl;
       
       pose.pose.position.x =point[i].position[0];
       pose.pose.position.y =point[i].position[1];
       pose.pose.position.z =point[i].position[2];
      
      std::cout << "pos x"<<"\n"<< point[i].position[0] << "\n" << " pos y"<< "\n"<< point[i].position[1]<<"\n"<< "pos z"<<"\n"<< point[i].position[2] << std::endl;
      
      std::cout << "Sono prima del while " << std::endl;
  
     local_pos_pub.publish(pose);
     
      std::cout << "Sono dopo aver pubblicato le cose" << std::endl;
  

        ros::spinOnce();
        rate.sleep();
	
	 std::cout << "Sono dopo lo sleep" << std::endl;
  
    }
}

  
    return 0;
}
