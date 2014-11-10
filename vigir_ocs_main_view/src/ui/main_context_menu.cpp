
#include "main_context_menu.h"
#include "main_view_widget.h"

MainViewContextMenu::MainViewContextMenu(MainViewWidget *main_view)
{
    main_view_ = main_view;
    createContextMenu();

    sys_command_pub_ = n_.advertise<std_msgs::String>("/syscommand",1,false);

    base_view_ = main_view->getPrimaryView();
    context_menu_manager_ = base_view_->getContextMenuManager();
    connect(context_menu_manager_,SIGNAL(updateMainViewItems()),main_view_,SLOT(updateContextMenu()));
}

MainViewContextMenu::~MainViewContextMenu()
{
}

void MainViewContextMenu::setAllCheckable()
{
    for (std::map<std::string ,contextMenuItem*>::iterator it=context_menu_items_map_.begin(); it!=context_menu_items_map_.end(); ++it)
    {
        contextMenuItem* item = it->second;
        item->action->setCheckable(true);
    }
}

void MainViewContextMenu::setItemCheckState(std::string name, bool checkable)
{
    context_menu_items_map_[name]->action->setCheckable(checkable);
}

void MainViewContextMenu::createContextMenu()
{
    //create Menu items,
    //the order in which they are created matters
    //must do parent objects before children
    //and in the order you want them to show up in the context menu

    //need to build a map as we add certain items becuase main view references the ui to update isChecked property

    context_menu_manager_->addActionItem("Request Point Cloud",boost::bind(&vigir_ocs::Base3DView::publishPointCloudWorldRequest,base_view_), NULL);

    context_menu_manager_->addSeperator();

    //manage windows-------------
    contextMenuItem * windowVisibility = context_menu_manager_->addMenuItem("Window Visibility");

    context_menu_items_map_["Joystick"] =  context_menu_manager_->addActionItem("Joystick",boost::bind(&MainViewWidget::contextToggleWindow,main_view_, WINDOW_JOYSTICK),windowVisibility);

    context_menu_items_map_["Grasp"]= context_menu_manager_->addActionItem("Grasp",boost::bind(&MainViewWidget::graspWidgetToggle,main_view_), windowVisibility );

    context_menu_items_map_["Position Mode"]= context_menu_manager_->addActionItem("Position Mode",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_POSITION_MODE), windowVisibility);

    //elements from joint control toolbar
    context_menu_items_map_["Joint Control"] = context_menu_manager_->addActionItem("Joint Control",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_JOINT_CONTROL), windowVisibility);
    context_menu_items_map_["Pelvis Pose"] = context_menu_manager_->addActionItem("Pelvis Pose",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_BDI_PELVIS_POSE), windowVisibility);
    context_menu_items_map_["Ghost Control"]= context_menu_manager_->addActionItem("Ghost Control",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_GHOST_CONFIG), windowVisibility);
    context_menu_items_map_["Planner Configuration"]= context_menu_manager_->addActionItem("Planner Configuration",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_PLANNER_CONFIG), windowVisibility);

    //elements from footstep control toolbar
    context_menu_items_map_["Basic Footstep Interface"] = context_menu_manager_->addActionItem("Basic Footstep Interface",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_FOOTSTEP_BASIC), windowVisibility);
    context_menu_items_map_["Advanced Footstep Interface"] = context_menu_manager_->addActionItem("Advanced Footstep Interface",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_FOOTSTEP_ADVANCED), windowVisibility);
    context_menu_items_map_["Footstep Parameter Control"] = context_menu_manager_->addActionItem("Footstep Parameter Control",boost::bind(&MainViewWidget::contextToggleWindow,main_view_,WINDOW_FOOTSTEP_PARAMETER), windowVisibility);

    //------------------------
    context_menu_manager_->addSeperator();

    contextMenuItem* manipulationModes = context_menu_manager_->addMenuItem("Manipulation Mode");

    context_menu_items_map_["Object"] = context_menu_manager_->addActionItem("Object",boost::bind(&MainViewWidget::setObjectManipulationMode,main_view_), manipulationModes);
    context_menu_items_map_["World"] = context_menu_manager_->addActionItem("World",boost::bind(&MainViewWidget::setWorldMode,main_view_), manipulationModes);
    context_menu_items_map_["Camera"]= context_menu_manager_->addActionItem("Camera",boost::bind(&MainViewWidget::setCameraMode,main_view_), manipulationModes);

    contextMenuItem * objectModes = context_menu_manager_->addMenuItem("Object Mode");

    context_menu_manager_->addActionItem("Template",boost::bind(&MainViewContextMenu::setTemplateMode,this), objectModes);
    context_menu_manager_->addActionItem("Left Arm",boost::bind(&MainViewContextMenu::setLeftArmMode,this), objectModes);
    context_menu_manager_->addActionItem("Right Arm",boost::bind(&MainViewContextMenu::setRightArmMode,this), objectModes);

    context_menu_manager_->addSeperator();

    contextMenuItem * systemCommands = context_menu_manager_->addMenuItem("System Commands");

    context_menu_manager_->addActionItem("Reset World Model",boost::bind(&MainViewContextMenu::systemCommandContext,this, "reset"), systemCommands);
    context_menu_manager_->addActionItem("Save Octomap",boost::bind(&MainViewContextMenu::systemCommandContext,this,"save_octomap"), systemCommands);
    context_menu_manager_->addActionItem("Save Pointcloud",boost::bind(&MainViewContextMenu::systemCommandContext,this,"save_pointcloud"), systemCommands);

}
//CALLBACKS/////////////////////

void MainViewContextMenu::systemCommandContext(std::string command)
{
    sysCmdMsg.data = command;
    sys_command_pub_.publish(sysCmdMsg);
}

//void MainViewContextMenu::contextToggleWindow(int window)
//{
//    switch(window)
//    {
//    case WINDOW_JOYSTICK:
//        if(!main_view_->getUi()->joystickBtn->isChecked())
//            main_view_->getUi()->joystickBtn->setChecked(true);
//        else
//            main_view_->getUi()->joystickBtn->setChecked(false);
//        break;
//    case WINDOW_JOINT_CONTROL:
//        if(!main_view_->getUi()->jointControlBtn->isChecked())
//            main_view_->getUi()->jointControlBtn->setChecked(true);
//        else
//            main_view_->getUi()->jointControlBtn->setChecked(false);
//        break;
//    case WINDOW_BDI_PELVIS_POSE:
//        if(!main_view_->getUi()->pelvisControlBtn->isChecked())
//            main_view_->getUi()->pelvisControlBtn->setChecked(true);
//        else
//            main_view_->getUi()->pelvisControlBtn->setChecked(false);
//        break;
//    case WINDOW_FOOTSTEP_BASIC:
//        if(!main_view_->getUi()->basicStepBtn->isChecked())
//            main_view_->getUi()->basicStepBtn->setChecked(true);
//        else
//            main_view_->getUi()->basicStepBtn->setChecked(false);
//        break;
//    case WINDOW_FOOTSTEP_ADVANCED:
//        if(!main_view_->getUi()->stepBtn->isChecked())
//            main_view_->getUi()->stepBtn->setChecked(true);
//        else
//            main_view_->getUi()->stepBtn->setChecked(false);
//        break;
//    case WINDOW_FOOTSTEP_PARAMETER:
//        if(!main_view_->getUi()->footstepParamBtn->isChecked())
//            main_view_->getUi()->footstepParamBtn->setChecked(true);
//        else
//            main_view_->getUi()->footstepParamBtn->setChecked(false);
//        break;
//    case WINDOW_GHOST_CONFIG:
//        if(!main_view_->getUi()->ghostControlBtn->isChecked())
//            main_view_->getUi()->ghostControlBtn->setChecked(true);
//        else
//            main_view_->getUi()->ghostControlBtn->setChecked(false);
//        break;
//    case WINDOW_POSITION_MODE:
//        if(!main_view_->getUi()->positionModeBtn->isChecked())
//            main_view_->getUi()->positionModeBtn->setChecked(true);
//        else
//            main_view_->getUi()->positionModeBtn->setChecked(false);
//        break;
//    case WINDOW_PLANNER_CONFIG:
//        if(!main_view_->getUi()->plannerConfigBtn->isChecked())
//            main_view_->getUi()->plannerConfigBtn->setChecked(true);
//        else
//            main_view_->getUi()->plannerConfigBtn->setChecked(false);
//        break;
//    }
//}
void MainViewContextMenu::setTemplateMode()
{
    main_view_->setObjectMode(0);
}
void MainViewContextMenu::setLeftArmMode()
{
    main_view_->setObjectMode(1);
}
void MainViewContextMenu::setRightArmMode()
{
    main_view_->setObjectMode(2);
}
//void MainViewContextMenu::setCameraMode()
//{
//    main_view_->getUi()->modeBox->setCurrentIndex(2);
//    main_view_->setManipulationMode(2);

//}
//void MainViewContextMenu::setWorldMode()
//{
//    main_view_->getUi()->modeBox->setCurrentIndex(1);
//    main_view_->setManipulationMode(1);
//}



