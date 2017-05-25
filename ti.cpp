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

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}

int main(int argc, char **argv)

{
  
  std::cout << "Sono nel main" << std::endl;
  
//definisco velocità v=1m/s e ho una frequenza di 10 Hz.T=0.1
  
  /*int v=1;  
  float T=0.1;
  std::vector<float> pf={0,0,0};
  std::vector<float> ps={0,0,5};
  std::vector<float> pt={2,0,5};
  float d1= std::sqrt(pow((ps[0]-pf[0]),2)+pow((ps[1]-pf[1]),2)+pow((ps[2]-pf[2]),2));
  float d2= std::sqrt(pow((pt[0]-ps[0]),2)+pow((pt[1]-ps[1]),2)+pow((pt[2]-ps[2]),2));
  int n1=d1/v*T;
  int n2=d2/v*T;;
  int n = n1+n2;*/ 
  
  //Definisco i 3 waypoints e le distanze tra i 3 punti
  
   
   std::vector<float> pf={0,0,0};
   std::vector<float> ps={0,0,5};
   std::vector<float> pt={2,0,5};
   
    point[0].position=pf;
    point[50].position=ps;
    point[70].position=pt;
    
   std::cout << "Sono dopo le posizioni" << std::endl;
  
  
    ros::init(argc, argv, "ti_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
    
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("mavros/state", 10, state_cb);
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

    geometry_msgs::PoseStamped pose;

    //pose.pose.position.x = 0;
    //pose.pose.position.y = 0;
    //pose.pose.position.z = 2;

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

   /* for(int i=0;i<71;i++) { 

     pose.pose.position.x = point[i].position[0];
     pose.pose.position.y = point[i].position[1];
     pose.pose.position.z = point[i].position[2];

     ros::Time last_request = ros::Time::now();
     
     double secs =last_request.toSec();
    
       if(i==0)

         point[i].time= secs;
	
       else point[i].time=secs -point[i-1].time;*/

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
	
	
    for(int i=0;i<71;i++) { 

     pose.pose.position.x = point[i].position[0];
     pose.pose.position.y = point[i].position[1];
     pose.pose.position.z = point[i].position[2];

     ros::Time last_request = ros::Time::now();
     
     double secs =last_request.toSec();
    
       if(i==0)

         point[i].time= secs;
	
       else point[i].time=secs -point[i-1].time;
	
	
        local_pos_pub.publish(pose);

        ros::spinOnce();
        rate.sleep();
    }
}

    return 0;
}
