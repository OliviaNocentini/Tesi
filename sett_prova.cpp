#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <tf2/LinearMath/Quaternion.h>

#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/transform_listener.h>

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
}


int main(int argc, char **argv)
{
   
  
    ros::init(argc, argv, "sett_prova_node");
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

    
    
    
    // wait for FCU connection
    while(ros::ok() && current_state.connected){
        ros::spinOnce();
        rate.sleep();
	ROS_INFO("State Not Connected");

    }

    geometry_msgs::PoseStamped pose;
    int waypoint_index=0;
    pose.pose.position.x = 0;
    pose.pose.position.y = 0;
    pose.pose.position.z = 2;

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
    
    tf2_ros::Buffer tfBuffer;
    tf2_ros::TransformListener tfListener(tfBuffer);

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

        local_pos_pub.publish(pose);
	geometry_msgs::TransformStamped transformStamped;
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
    
    std::cout <<"Tx"<<" "<< transformStamped.transform.translation.x <<" "<<"Ty"<<" " << transformStamped.transform.translation.y <<" "<<"Tz"<<" " << transformStamped.transform.translation.z <<std::endl;
	
 //std::cout << "pos x"<<"\n"<< pose.pose.position.x << "\n" << " pos y"<< "\n"<<pose.pose.position.y<<"\n"<< "pos z"<<"\n"<< pose.pose.position.z << std::endl;
      
    
        ros::spinOnce();
        rate.sleep();
    }

    return 0;

}