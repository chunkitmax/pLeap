/*
 *	LeapSensor.cpp
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#include "LeapSensor.h"

const array<string, 5>	LeapSensor::FingerNames = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const array<string, 4>	LeapSensor::BoneNames = { "Metacarpal", "Proximal", "Middle", "Distal" };
const array<string, 4>	LeapSensor::StateNames = { "STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END" };

LeapSensor				*LeapSensor::m_instance = nullptr;

void LeapSensor::Frame::update(const Leap::Controller &controller, const int history)
{
	const Leap::Frame frame = controller.frame(history);

	try
	{
		if (frame.isValid())
		{
			clear();

			id = frame.id();
			timestamp = frame.timestamp();

			const Leap::FingerList extendedFingers = frame.fingers().extended();
			extendedFingersCount = extendedFingers.count();
			for (Leap::FingerList::const_iterator cur = extendedFingers.begin(); cur != extendedFingers.end(); cur++)
				extendedState |= (1 << (*cur).type());

			Leap::HandList _hands = frame.hands();
			for (Leap::HandList::const_iterator cur = _hands.begin(); cur != _hands.end(); cur++)
			{
				const Leap::Hand hand = *cur;
				Hand tempHand;

				tempHand.id = hand.id();
				tempHand.type = hand.isLeft()? HandType::Left : HandType::Right;

				const Leap::Arm arm = hand.arm();
				if (arm.isValid())
				{
					tempHand.arm.direction = arm.direction();
					tempHand.arm.elbowPos = arm.elbowPosition();
					tempHand.arm.wristPos = arm.wristPosition();
				}

				const Leap::Vector palmDirection = hand.direction();
				tempHand.palm.position = hand.palmPosition();
				tempHand.palm.velocity = hand.palmVelocity();
				tempHand.palm.pitch = palmDirection.pitch();
				tempHand.palm.yaw = palmDirection.yaw();
				tempHand.palm.roll = hand.palmNormal().roll();

				const Leap::FingerList fingers = hand.fingers();
				for (Leap::FingerList::const_iterator cur = fingers.begin(); cur != fingers.end(); cur++)
				{
					const Leap::Finger finger = *cur;
					int type = finger.type();

					tempHand.fingers[type].id = finger.id();

					for (int i = 0; i < 4; i++)
					{
						const Leap::Bone bone = finger.bone((Leap::Bone::Type)i);
						tempHand.fingers[type].bones[i].start = bone.prevJoint();
						tempHand.fingers[type].bones[i].end = bone.nextJoint();
						tempHand.fingers[type].bones[i].direction = bone.direction();
					}
				}

				hands.push_back(tempHand);
			}

			const Leap::ToolList _tools = frame.tools();
			for (Leap::ToolList::const_iterator cur = _tools.begin(); cur != _tools.end(); cur++)
			{
				const Leap::Tool tool = *cur;
				Tool tempTool;

				tempTool.id = tool.id();
				tempTool.position = tool.tipPosition();
				tempTool.direction = tool.tipVelocity();
				tempTool.width = tool.width();
				tempTool.length = tool.length();

				tools.push_back(tempTool);
			}

			const Leap::GestureList _gestures = frame.gestures();
			for (Leap::GestureList::const_iterator cur = _gestures.begin(); cur != _gestures.end(); cur++)
			{
				Leap::Gesture gesture = *cur;
				Gesture tempGesture;

				tempGesture.id = gesture.id();
				tempGesture.type = (GestureType)gesture.type();
				tempGesture.state = (GestureState)gesture.state();
				tempGesture.gesture = gesture;

				gestures.push_back(tempGesture);
			}
		}
	}
	catch (int e)
	{
		cout << "GG" << endl;
		clear();
	}
}

void LeapSensor::Frame::clear(void)
{
	id = 0;
	timestamp = 0;
	extendedState = 0;
	extendedFingersCount = 0;
	hands = vector<Hand>();
	tools = vector<Tool>();
	gestures = vector<Gesture>();
}


LeapSensor::LeapSensor(const GestureFlag flags)
:
	m_listener(*this),
	m_controller(),
	m_lastFrame(),
	m_state(0)
{
	if (!m_instance)
	{
		m_instance = this;
		m_controller.addListener(m_listener);

		if (flags != GestureFlag::NoneGesture)
			for(int i = 0; i < 4; i++)
				if (flags & (1 << i))
					m_controller.enableGesture((Leap::Gesture::Type)gestureFlagToType((GestureFlag)(1 << i)));
	}
}

LeapSensor::~LeapSensor(void)
{
	m_controller.removeListener(m_listener);
}

void LeapSensor::setState(unsigned char _state)
{
	m_state = _state;
}

unsigned char LeapSensor::getState(void) const
{
	return m_state;
}

LeapSensor::Frame LeapSensor::getFrame(void) const
{
	return m_lastFrame;
}

LeapSensor::Frame LeapSensor::getFrame(const int history)
{
	Frame tempFrame;
	tempFrame.update(m_controller, history);
	return tempFrame;
}

LeapSensor::Frame LeapSensor::LastFrame(void)
{
	return m_instance->m_lastFrame;
}

string LeapSensor::getGestureName(const GestureType type)
{
	switch (type)
	{
	default:
		return string("Invalid");

	case GestureType::InvalidType:
		return string("Invalid");

	case GestureType::Circle:
		return string("Circle");

	case GestureType::KeyTap:
		return string("KeyTap");

	case GestureType::ScreenTap:
		return string("ScreenTap");

	case GestureType::Swipe:
		return string("Swipe");
	}
}

LeapSensor::GestureType LeapSensor::gestureFlagToType(const GestureFlag flag)
{
	switch (flag)
	{
	default:
		return (GestureType)0;

	case GestureFlag::CircleGesture:
		return GestureType::Circle;

	case GestureFlag::SwipeGesture:
		return GestureType::Swipe;

	case GestureFlag::KeyTapGesture:
		return GestureType::KeyTap;

	case GestureFlag::ScreenTapGesture:
		return GestureType::ScreenTap;
	}
}

void LeapSensor::setOnFrameListener(const OnFrameListener &listener)
{
	m_onFrameListener = listener;
}

LeapSensor::LeapListener::LeapListener(LeapSensor &_sensor)
:
	m_leapInstance(_sensor)
{}

void LeapSensor::LeapListener::onInit(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() | STATE_INITIALIZED);
}

void LeapSensor::LeapListener::onConnect(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() | STATE_DEVICE_CONNECTED);
}

void LeapSensor::LeapListener::onDisconnect(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() & ~STATE_DEVICE_CONNECTED);
}

void LeapSensor::LeapListener::onExit(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() & ~STATE_INITIALIZED);
}

void LeapSensor::LeapListener::onFrame(const Leap::Controller &controller)
{
	m_leapInstance.m_lastFrame.update(controller);
	m_leapInstance.m_onFrameListener(m_leapInstance.m_lastFrame);
}

void LeapSensor::LeapListener::onFocusGained(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() | STATE_FOCUSED);
}

void LeapSensor::LeapListener::onFocusLost(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() & ~STATE_FOCUSED);
}

void LeapSensor::LeapListener::onDeviceChange(const Leap::Controller &controller)
{
	std::cout << "Device Changed" << std::endl;
	const Leap::DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i)
	{
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void LeapSensor::LeapListener::onServiceConnect(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() | STATE_SERVICE_CONNECTED);
}

void LeapSensor::LeapListener::onServiceDisconnect(const Leap::Controller &controller)
{
	m_leapInstance.setState(m_leapInstance.getState() & ~STATE_SERVICE_CONNECTED);
}
