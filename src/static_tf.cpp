#include <ros/ros.h>

// msgs
#include <geometry_msgs/Pose.h>
#include <tf/transform_broadcaster.h>

// User-defined functions/conversions
geometry_msgs::Pose vec2pose(std::vector<float> vec)
{
    geometry_msgs::Pose pose;
    
    pose.position.x    = vec[0];
    pose.position.y    = vec[1];
    pose.position.z    = vec[2];
    pose.orientation.x = vec[3];
    pose.orientation.y = vec[4];
    pose.orientation.z = vec[5];
    pose.orientation.w = vec[6];

    return (pose);
}

int main(int argc, char **argv)
{
    // ROS
    ros::init(argc, argv, "static_tf");
    ros::NodeHandle n;
    ros::Rate       loop_rate(100); // 10Hz
    // TF
    tf::TransformBroadcaster br;
    tf::Transform            trtransform;
    // Params
    std::string root_name, camera_frame;
    if (!n.getParam("root_name", root_name)) ROS_ERROR_STREAM("No root name found on parameter server (/root_name)");
    if (!n.getParam("frame_id", camera_frame)) ROS_ERROR_STREAM("No root name found on parameter server (/frame_id)");
    std::vector<float> camvector, translation, rotation;
    if (!n.getParam("translation", translation))
    {
        ROS_ERROR("Could not read translation");
        return 0; 
    }
    camvector.insert(camvector.end(), translation.begin(), translation.end());
    if (!n.getParam("rotation", rotation))
    {
        ROS_ERROR("Could not read rotation");
        return 0; 
    }
    camvector.insert(camvector.end(), rotation.begin(), rotation.end());
    geometry_msgs::Pose campose = vec2pose(camvector);

    trtransform.setOrigin(tf::Vector3(campose.position.x, campose.position.y, campose.position.z));
    trtransform.setRotation(tf::Quaternion(campose.orientation.x, campose.orientation.y, campose.orientation.z, campose.orientation.w));

    while(ros::ok())
    {
        // ----------- PUBLISH TF ------------
        br.sendTransform(tf::StampedTransform(trtransform, ros::Time::now(), camera_frame, root_name));

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}