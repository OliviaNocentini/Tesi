#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>
#include <algorithm>
#include <functional>
#define n 3

//definisco una struttura che definisce i punti della mia traiettoria

struct trajectory{

std::vector<float> position;
double time;

} point[n];


struct time{
  
  double tempo;
  
} t[n];


int main(int argc, char **argv)
{
  
  
  
  // 3 waypoints
   
   std::vector<float> pf={0,0,0};
   std::vector<float> ps={0,0,5};
   std::vector<float> pt={2,0,5};
  
   point[0].position=pf;
   point[1].position=ps;
   point[2].position=pt;
  
   
    
   
 std::cout << "p0x"<<"\n"<< point[0].position[0] << "\n" << " p0y"<< "\n"<< point[0].position[1]<<"\n"<< "p0z"<<"\n"<< point[0].position[2] << std::endl;
 std::cout << "p1x"<<"\n"<< point[1].position[0] << "\n" << " p1y"<< "\n"<< point[1].position[1]<<"\n"<< "p1z"<<"\n"<< point[1].position[2] << std::endl;
 std::cout << "p2x"<<"\n"<< point[2].position[0] << "\n" << " p2y"<< "\n"<< point[2].position[1]<<"\n"<< "p2z"<<"\n"<< point[2].position[2] << std::endl;

   ros::init(argc, argv, "last_node");
   ros::NodeHandle nh;
  
   ROS_INFO("Test_info");
    
   ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    
   ros::Rate rate(20.0);

   geometry_msgs::PoseStamped pose;
  
   ros::Time last_request = ros::Time::now();
   double secs =ros::Time::now().toSec();
   
   ros::Duration delta ;
   double delta_secs  = delta.toSec();
   delta_secs=0.1;
   
   for (int i=1; i< 3; i++) {
      
      ros::Time last_request = ros::Time::now();
      double secs =ros::Time::now().toSec();
      
      std::cout << "sono nel for" <<std::endl;
     
     point[i].time=secs + delta_secs;
     
     std::cout << "time" <<  point[i].time << std::endl;
     
    }
     
 
   
   /*double Vand[n-1][n-1]=
    {
        {1,t[n-3],pow(t[n-3],2)},
        {1,t[n-2],pow(t[n-2],2)},
        
    };*/
   

   
   while(ros::ok()){

     
  
  //local_pos_pub.publish(pose); //pubblica nel topic
	
	ros::spinOnce();
        rate.sleep();
    }
    
   
   std::cout << "tempo ora" << t <<std::endl;

    return 0;
}
   
