#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

struct goal_pose
{
    double pose[3];
    double orientation[4];
};

struct goal_pose set_point[]=
{
    {4.0,0,0,   0,0,0.707,0.707},
    {4.0,0,0,   0,0,0.707,0.707},
    {4.0,0,0,   0,0,0.707,0.707},
    {4.0,0,0,   0,0,0.707,0.707},
    // {1.0,0,0,   0,0,0.707,0.707},    left
    // {1.0,0.0,0, 0,0,1.0,0},          back 
    // {1.0,0,0,  0,0,-0.707,0.707},    right
    // {1,0.0,0,  0,0,0,1.0},           hold        
};

int size =sizeof(set_point)/sizeof(goal_pose);

 int main(int argc, char *argv[])
{
    
    ros::init(argc, argv, "simple_navigation_goals");
    MoveBaseClient ac("move_base",true);

    while (!ac.waitForServer(ros::Duration(1.0)))
    {
       ROS_INFO("waiting for the move_base action server to come up");
    }
    
    for(int i=0;i<size;i++)  
    {
        struct goal_pose point=set_point[i];
        move_base_msgs::MoveBaseGoal goal;

        goal.target_pose.header.frame_id="base_link";
        goal.target_pose.header.stamp=ros::Time::now();

        goal.target_pose.pose.position.x=point.pose[0];
        goal.target_pose.pose.position.y=point.pose[1];
        goal.target_pose.pose.position.z=point.pose[2];
        goal.target_pose.pose.orientation.x=point.orientation[0];
        goal.target_pose.pose.orientation.y=point.orientation[1];
        goal.target_pose.pose.orientation.z=point.orientation[2];        
        goal.target_pose.pose.orientation.w=point.orientation[3];

        ROS_INFO("sending goal");
        ac.sendGoal(goal);

        ac.waitForResult();
        if(ac.getState()==actionlib::SimpleClientGoalState::SUCCEEDED)
        {
            ROS_INFO("the base move succsess");
            //continue;
        }
        else{
            ROS_INFO("the base failed to move for some reason");
            //break;
        }
    }
    

    return 0;
}