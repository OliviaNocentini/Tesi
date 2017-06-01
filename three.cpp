#include <iostream>
#include <numeric>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>
#include <algorithm>
#include <functional>

#define n 3

using namespace std;

struct waypoint{
  
std::vector<float> position;

} point[3];




int main(int argc, char **argv)
{
   
  int i;
  
 std::vector<float> p0={0,0,0}; 
 std::vector<float> p1={0,0,5}; 
 std::vector<float> p2={2,0,5};
 std::vector<float> pi(3);
 std::vector<float> pf(3);
 
 
 
 point[0].position=p0;
 point[1].position=p1;
 point[2].position=p2;
 
std::cout << "p0x"<<"\n"<< point[0].position[0] << "\n" << " p0y"<< "\n"<< point[0].position[1]<<"\n"<< "p0z"<<"\n"<< point[0].position[2] << std::endl;
std::cout << "p1x"<<"\n"<< point[1].position[0] << "\n" << " p1y"<< "\n"<< point[1].position[1]<<"\n"<< "p1z"<<"\n"<< point[1].position[2] << std::endl;
std::cout << "p2x"<<"\n"<< point[2].position[0] << "\n" << " p2y"<< "\n"<< point[2].position[1]<<"\n"<< "p2z"<<"\n"<< point[2].position[2] << std::endl;
     
    ros::init(argc, argv, "three_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
    
    
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);


    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    geometry_msgs::PoseStamped pose;
    
      
      
    ros::Time last_request = ros::Time::now();

    while(ros::ok()){
        
      for(i=0; i< 3; i++){
	
      pi[0]=point[i].position[0];
      pi[1]=point[i].position[1];
      pi[2]=point[i].position[2];
      
      std::cout << "pi0"<<"\n"<< pi[0] << "\n" << " piy"<< "\n"<< pi[1]<<"\n"<< "piz"<<"\n"<< pi[2] << std::endl;
      
   
      
      }
	
      /*pose.pose.position.x = pf[0];
      pose.pose.position.y = pf[1];
      pose.pose.position.z = pf[2];*/

        local_pos_pub.publish(pose);
	
 std::cout << "pos x"<<"\n"<< pose.pose.position.x << "\n" << " pos y"<< "\n"<<pose.pose.position.y<<"\n"<< "pos z"<<"\n"<< pose.pose.position.z << std::endl;
      

      // pi=pf;
 
        ros::spinOnce();
        rate.sleep();
    }
    
   

    return 0;
}