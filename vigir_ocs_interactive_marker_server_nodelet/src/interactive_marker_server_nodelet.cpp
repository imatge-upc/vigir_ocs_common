#include "interactive_marker_server_nodelet.h"

namespace ocs_interactive_marker_server
{
void InteractiveMarkerServerNodelet::onInit()
{
    interactive_marker_server_feedback_pub_ = nh_.advertise<flor_ocs_msgs::OCSInteractiveMarkerUpdate>( "/flor/ocs/interactive_marker_server/feedback",100, false );
    interactive_marker_server_add_sub_ = nh_.subscribe<flor_ocs_msgs::OCSInteractiveMarkerAdd>( "/flor/ocs/interactive_marker_server/add", 100, &InteractiveMarkerServerNodelet::addInteractiveMarker, this );
    interactive_marker_server_remove_sub_ = nh_.subscribe<std_msgs::String>( "/flor/ocs/interactive_marker_server/remove", 100, &InteractiveMarkerServerNodelet::removeInteractiveMarker, this );
    interactive_marker_server_update_sub_ = nh_.subscribe<flor_ocs_msgs::OCSInteractiveMarkerUpdate>( "/flor/ocs/interactive_marker_server/update", 100, &InteractiveMarkerServerNodelet::updatePose, this );
    interactive_marker_server_mode_sub_ = nh_.subscribe<flor_ocs_msgs::OCSControlMode>( "/flor/ocs/control_modes", 100, &InteractiveMarkerServerNodelet::setMode, this );
}

void InteractiveMarkerServerNodelet::addInteractiveMarker(const flor_ocs_msgs::OCSInteractiveMarkerAdd::ConstPtr &msg)
{
    // name, topic, frame, scale, pointOCSInteractiveMarkerAdd
    if (marker_map_.find(msg->topic) == marker_map_.end())
    {
        //ROS_INFO("Adding marker %s", msg->topic.c_str());

        marker_map_[msg->topic] = new InteractiveMarkerServerCustom(msg->name, msg->topic, msg->mode, msg->frame, msg->scale, msg->point);
        marker_map_[msg->topic]->onFeedback = boost::bind(&InteractiveMarkerServerNodelet::onMarkerFeedback, this, _1, _2);
    }
}

void InteractiveMarkerServerNodelet::removeInteractiveMarker( const std_msgs::String::ConstPtr& msg )
{
    //ROS_ERROR("%s marker exists?", msg->data.c_str());
    if(marker_map_.find(msg->data) != marker_map_.end())
    {
        //marker_map_[msg->data];
        //ROS_INFO("Removing marker %s", msg->data.c_str());
        delete marker_map_[msg->data];
        marker_map_.erase(marker_map_.find(msg->data));
    }
}

void InteractiveMarkerServerNodelet::updatePose(const flor_ocs_msgs::OCSInteractiveMarkerUpdate::ConstPtr &msg)
{
    if(marker_map_.find(msg->topic) != marker_map_.end())
    {
        //ROS_INFO("Updating marker %s", msg->topic.c_str());
        marker_map_[msg->topic]->setPose(msg->pose);
    }
}

void InteractiveMarkerServerNodelet::onMarkerFeedback(std::string topic_name, geometry_msgs::PoseStamped pose)
{
    flor_ocs_msgs::OCSInteractiveMarkerUpdate cmd;
    cmd.topic = topic_name;
    cmd.pose = pose;
    interactive_marker_server_feedback_pub_.publish(cmd);
}

void InteractiveMarkerServerNodelet::setMode(const flor_ocs_msgs::OCSControlMode::ConstPtr& msg)
{
    //ROS_ERROR("CHANGING MODE");
    std::map<std::string,InteractiveMarkerServerCustom*>::iterator iter;
    for (iter = marker_map_.begin(); iter != marker_map_.end(); ++iter)
        if(iter->second->getMode() != flor_ocs_msgs::OCSInteractiveMarkerAdd::WAYPOINT_3DOF)
            iter->second->setMode(msg->manipulationMode);
}

}

PLUGINLIB_DECLARE_CLASS (vigir_ocs_interactive_marker_server_nodelet, InteractiveMarkerServerNodelet, ocs_interactive_marker_server::InteractiveMarkerServerNodelet, nodelet::Nodelet);
