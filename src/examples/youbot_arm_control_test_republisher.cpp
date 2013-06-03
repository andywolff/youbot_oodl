/******************************************************************************
* Copyright (c) 2011
* Locomotec
*
* Author:
* Adam Jardim
*
*
* This software is published under a dual-license: GNU Lesser General Public
* License LGPL 2.1 and BSD license. The dual-license implies that users of this
* code may choose which terms they prefer.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Locomotec nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License LGPL as
* published by the Free Software Foundation, either version 2.1 of the
* License, or (at your option) any later version or the BSD license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License LGPL and the BSD license for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License LGPL and BSD license along with this program.
*
******************************************************************************/

#include <iostream>
#include <assert.h>

#include "ros/ros.h"
#include "trajectory_msgs/JointTrajectory.h"
#include "brics_actuator/CartesianWrench.h"

#include <boost/units/io.hpp>

#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/conversion.hpp>

#include <iostream>
#include <assert.h>

#include "ros/ros.h"
#include "brics_actuator/JointPositions.h"
#include "sensor_msgs/JointState.h"

#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/io.hpp>

#include <boost/units/systems/angle/degrees.hpp>
#include <boost/units/conversion.hpp>

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "std_msgs/String.h"

double joint[5];		
double gripperr = 0;
double gripperl = 0;
bool justStarted = false;

using namespace std;
/*
bool allJointsValid()
{
	double jointMax[] = {5.840139, 2.617989, -0.0157081, 3.42919, 5.641589};
	double jointMin[] = {0.01006921, 0.01006921, -5.026549, 0.0221391, 0.11062};
	double gripperMax = 0.0115;
	double gripperMin = 0;

	int validCounter = 0;

	for (int i = 0; i < 5; i++)
	{
		if
}*/

void get_arm_command(const brics_actuator::JointPositions/*::ConstPtr&*/ msg)
{
	joint[0] = msg.positions[0].value;
	joint[1] = msg.positions[1].value;
	joint[2] = msg.positions[2].value;
	joint[3] = msg.positions[3].value;
	joint[4] = msg.positions[4].value;
}

void get_gripper_command(const brics_actuator::JointPositions/*::ConstPtr&*/ msg)
{
	gripperl = msg.positions[0].value;
	gripperr = msg.positions[1].value;
}

int main(int argc, char **argv) {

	ros::init(argc, argv, "youbot_arm_control_test_republisher");
	ros::NodeHandle n;
	ros::Publisher armPositionsPublisher;
	ros::Publisher gripperPositionPublisher;
	ros::Subscriber armSubscriber;
	ros::Subscriber gripperSubscriber;

	armSubscriber = n.subscribe("arm_command_throw", 1, get_arm_command);
	gripperSubscriber = n.subscribe("gripper_command_throw", 1, get_gripper_command);
	armPositionsPublisher = n.advertise<brics_actuator::JointPositions > ("arm_1/arm_controller/position_command", 1);
	gripperPositionPublisher = n.advertise<brics_actuator::JointPositions > ("arm_1/gripper_controller/position_command", 1);

	ros::Rate rate(1); //Hz

	static const int numberOfArmJoints = 5;
	static const int numberOfGripperJoints = 2;


	brics_actuator::JointPositions command;
	vector <brics_actuator::JointValue> armJointPositions;
	vector <brics_actuator::JointValue> gripperJointPositions;

	armJointPositions.resize(numberOfArmJoints); //TODO:change that
	gripperJointPositions.resize(numberOfGripperJoints);

	std::stringstream jointName;

	while(ros::ok() && (justStarted == false))
	{
		ros::spinOnce();
	
		if(joint[0] != 0)
			justStarted = true;
	}

	cout << "Republisher: ACTIVE" << endl;

	while(ros::ok())
	{
		ros::spinOnce();

		//cout<<".";

		for(int i = 0; i < numberOfArmJoints; i++)
		{
			jointName.str("");
			jointName << "arm_joint_" << (i + 1);

			armJointPositions[i].joint_uri = jointName.str();
			armJointPositions[i].value = joint[i];

			armJointPositions[i].unit = boost::units::to_string(boost::units::si::radians);
		}
		
		gripperJointPositions[0].joint_uri = "gripper_finger_joint_l";
		gripperJointPositions[0].value = gripperl;
		gripperJointPositions[0].unit = boost::units::to_string(boost::units::si::meter);

		gripperJointPositions[1].joint_uri = "gripper_finger_joint_r";
		gripperJointPositions[1].value = gripperr;
		gripperJointPositions[1].unit = boost::units::to_string(boost::units::si::meter);

		command.positions = armJointPositions;
		armPositionsPublisher.publish(command);

		//cout << "Republishing: " << endl;
		cout << "\t";
		for(int i = 0; i < numberOfArmJoints; i++)
		{
			cout << " J" << i + 1 << ": " << command.positions[i].value;
		}
		command.positions = gripperJointPositions;
		gripperPositionPublisher.publish(command);
		
		cout << " GL: " << command.positions[0].value << " GR: " << command.positions[1].value << endl;
		
	}
		

	return 0;
}

/* EOF */