#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>
#include <algorithm>
#include <functional>
#define n 70

//definisco una struttura che definisce i punti della mia traiettoria

struct trajectory{

std::vector<float> position;
double time;

} ;

trajectory passo;

trajectory point[n];

int main(int argc, char **argv)
{
  
  //tempo di campionamento e velocità
   
   double T=0.1;
   int v=1;
   
  
   // 3 waypoints
   
   std::vector<float> pf={0,0,0};
   std::vector<float> ps={0,0,5};
   std::vector<float> pt={2,0,5};
   std::vector<float> delta1={0,0,0.1};
   

   
   point[0].position=pf;
   point[49].position=ps;
   point[69].position=pt;
   passo.position=delta1;
    
   
 std::cout << "p0x"<<"\n"<< point[0].position[0] << "\n" << " p0y"<< "\n"<< point[0].position[1]<<"\n"<< "p0z"<<"\n"<< point[0].position[2] << std::endl;
 std::cout << "p1x"<<"\n"<< point[49].position[0] << "\n" << " p1y"<< "\n"<< point[49].position[1]<<"\n"<< "p1z"<<"\n"<< point[49].position[2] << std::endl;
 std::cout << "p2x"<<"\n"<< point[69].position[0] << "\n" << " p2y"<< "\n"<< point[69].position[1]<<"\n"<< "p2z"<<"\n"<< point[69].position[2] << std::endl;

   ros::init(argc, argv, "talker_node");
   ros::NodeHandle nh;
  
   ROS_INFO("Test_info");
    
   ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    
   ros::Rate rate(20.0);

   geometry_msgs::PoseStamped pose;
    
   ros::Time last_request = ros::Time::now();
   double secs =last_request.toSec();
  
  
   ros::Duration d(0.1);
   double d_secs = d.toSec();
   
    for(int i=1;i<n;i++) { 
      
    std::cout << " sono dentro for " << std::endl;
    
   
    point[i].time=secs + d_secs;
     
     std::cout << "time" <<  point[i].time << std::endl;
     
    }
    
    
  
   while(ros::ok()){
     
   
      
      
       
     
    
     
   //local_pos_pub.publish(pose); //pubblica nel topic
	
	ros::spinOnce();
        rate.sleep();
    }
    
   
   
   
    return 0;
 }
   
