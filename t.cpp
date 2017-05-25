#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>

#define n 70

//definisco una struttura che definisce i punti della mia traiettoria

struct trajectory{

std::vector<float> position;
double time;

} point[n];

//Callback che salva lo stato attuale del mio autopilota

geometry_msgs::PoseStamped pose;
void pose_cb(const geometry_msgs::PoseStamped::ConstPtr& msg){
    pose = *msg;
}

int main(int argc, char **argv)

{
   std::cout << "sono nel main" << std::endl;
  

   int T=0.1;
   int v=1;
   
   std::vector<float> pf={0,0,0};
   std::vector<float> ps={0,0,5};
   std::vector<float> pt={2,0,5};
   
   point[0].position=pf;
   point[49].position=ps;
   point[69].position=pt;
   
   for(int i=1;i<n;i++) { 
      
       point[i].position[0]=point[i-1].position[0]+v*T;
       point[i].position[1]=point[i-1].position[1]+v*T;
       point[i].position[2]=point[i-1].position[2]+v*T;
       
     std::cout << "pos x"<<"\n"<< point[i].position[0] << "\n" << " pos y"<< "\n"<< point[i].position[1]<<"\n"<< "pos z"<<"\n"<< point[i].position[2] << std::endl;  
   
     
     std::cout << "stampo posizioni" << std::endl;
  
    ros::init(argc, argv, "t_node");
    ros::NodeHandle nh;
  
    ROS_INFO("Test_info");
    
    
    ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10, pose_cb);
    
    ros::Rate rate(20.0);
    
    ros::Time last_request = ros::Time::now();

    double secs =last_request.toSec();
	
    for(int i=1;i<n;i++) { 
      
       point[i].position[0]=point[i-1].position[0]+v*T;
       point[i].position[1]=point[i-1].position[1]+v*T;
       point[i].position[2]=point[i-1].position[2]+v*T;
       
       pose.pose.position.x =point[i].position[0];
       pose.pose.position.y =point[i].position[1];
       pose.pose.position.z =point[i].position[2];
       
       std::cout << " Stampo la mia posizione " << std::endl;
      
      std::cout << "pos x"<<"\n"<< point[i].position[0] << "\n" << " pos y"<< "\n"<< point[i].position[1]<<"\n"<< "pos z"<<"\n"<< point[i].position[2] << std::endl;
      
      while(ros::ok()){
     

        ros::spinOnce();
        rate.sleep();
	
	
  
    }

    }
  
    return 0;
 }
}