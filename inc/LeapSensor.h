/*
 *	LeapSensor.h
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#pragma once

#include <array>
#include <functional>
#include <cstring>
#include <Leap.h>

using namespace std;

#define inRange(n, v, x) ((v < n)? n : ((v > x)? x : v))

class LeapSensor
{

public:

	class LeapListener : public Leap::Listener
	{

	public:

		LeapListener(LeapSensor &_sensor);

	    virtual void onInit(const Leap::Controller&);
	    virtual void onConnect(const Leap::Controller&);
	    virtual void onDisconnect(const Leap::Controller&);
	    virtual void onExit(const Leap::Controller&);
	    virtual void onFrame(const Leap::Controller&);
	    virtual void onFocusGained(const Leap::Controller&);
	    virtual void onFocusLost(const Leap::Controller&);
	    virtual void onDeviceChange(const Leap::Controller&);
	    virtual void onServiceConnect(const Leap::Controller&);
	    virtual void onServiceDisconnect(const Leap::Controller&);

	private:

	    LeapSensor			&m_leapInstance;

	};

	typedef enum HandType
	{
		Invalid = 0,
		Left,
		Right
	} HandType;

	typedef enum FingerType
	{
		ThumbFinger = 0,
		IndexFinger,
		MiddleFinger,
		RingFinger,
		PinkyFinger
	} FingerType;

	typedef enum BoneType
	{
		MetacarpalBone = 0,
		ProximalBone,
		MiddleBone,
		DistalBone
	} BoneType;

	typedef enum GestureFlag
	{
		NoneGesture		= 0,
		CircleGesture		= 1 << 0,
		SwipeGesture		= 1 << 1,
		KeyTapGesture		= 1 << 2,
		ScreenTapGesture = 1 << 3
	} GestureFlag;

	typedef enum GestureType
	{
		InvalidType = Leap::Gesture::TYPE_INVALID,
		Circle = Leap::Gesture::TYPE_CIRCLE,
		Swipe = Leap::Gesture::TYPE_SWIPE,
		KeyTap = Leap::Gesture::TYPE_KEY_TAP,
		ScreenTap = Leap::Gesture::TYPE_SCREEN_TAP
	} GestureType;

	typedef enum GestureState
	{
		InvalidState = Leap::Gesture::STATE_INVALID,
		Start = Leap::Gesture::STATE_START,
		Stop = Leap::Gesture::STATE_STOP,
		Update = Leap::Gesture::STATE_UPDATE
	} GestureState;

	struct Palm
	{
		Leap::Vector		position;
		Leap::Vector		velocity;
		float				pitch;
		float				roll;
		float				yaw;

		Palm(void)
		:
			position(), velocity(), pitch(0.0f), roll(0.0f), yaw(0.0f)
		{}
	};

	struct Bone
	{
		Leap::Vector		start;
		Leap::Vector		end;
		Leap::Vector		direction;

		Bone(void)
		:
			start(), end(), direction()
		{}
	};

	struct Finger
	{
		int32_t			id;
		Leap::Vector		tipPosition;
		Leap::Vector		tipVelocity;
		array<Bone, 4>	bones;

		Finger(void)
		:
			id(0), bones()
		{}
	};

	struct Arm
	{
		Leap::Vector		direction;
		Leap::Vector		wristPos;
		Leap::Vector		elbowPos;

		Arm(void)
		:
			direction(), wristPos(), elbowPos()
		{}
	};

	struct Hand
	{
		int32_t			id;
		HandType			type;
		Arm				arm;
		Palm				palm;
		array<Finger, 5>	fingers;

		Hand(void)
		:
			id(0), type(HandType::Invalid), arm(), palm(), fingers()
		{}
	};

	struct Tool
	{
		int32_t				id;
		Leap::Vector			position;
		Leap::Vector			direction;
		float					width;
		float					length;

		Tool(void)
		:
			id(0), position(), direction(), width(0.0f), length(0.0f)
		{}
	};

	struct Gesture
	{
		int32_t					id;
		GestureType				type;
		GestureState				state;
		Leap::Gesture				gesture;

		Gesture(void)
		:
			id(0), type(GestureType::InvalidType), state(GestureState::InvalidState), gesture()
		{}
	};

	struct Frame
	{
		int64_t			id;
		int64_t			timestamp;
		uint8_t			extendedState;
		int					extendedFingersCount;
		vector<Hand>		hands;
		vector<Tool>		tools;
		vector<Gesture>	gestures;

		Frame(void)
		:
			id(0), timestamp(0), extendedState(0), extendedFingersCount(0), hands(2), tools(), gestures()
		{}

		Frame(const Frame &rhs)
		:
			id(rhs.id), timestamp(rhs.timestamp), extendedState(rhs.extendedState), extendedFingersCount(rhs.extendedFingersCount), hands(rhs.hands), tools(rhs.tools), gestures(rhs.gestures)
		{}

		void update(const Leap::Controller &controller, const int history = 0);

		void clear(void);
	};

	enum State
	{
		STATE_INITIALIZED					= 1 << 0,
		STATE_FOCUSED					= 1 << 1,
		STATE_DEVICE_CONNECTED		= 1 << 2,
		STATE_SERVICE_CONNECTED	= 1 << 3
	};

	typedef Leap::Vector Vector;
	typedef function<void (const Frame &)> OnFrameListener;

	LeapSensor(void) = delete;
	explicit LeapSensor(const GestureFlag flags);
	~LeapSensor(void);

	void setState(unsigned char _state);
	unsigned char getState(void) const;

	Frame getFrame(void) const;
	Frame getFrame(const int history);
	static Frame LastFrame(void);

	static string getGestureName(const GestureType type);
	static GestureType gestureFlagToType(const GestureFlag flag);

	void setOnFrameListener(const OnFrameListener &listener);

	static const array<string, 5>		FingerNames;
	static const array<string, 4>		BoneNames;
	static const array<string, 4>		StateNames;

private:

	OnFrameListener		m_onFrameListener;

	LeapListener			m_listener;
	Leap::Controller		m_controller;

	Frame					m_lastFrame;

	unsigned char			m_state;

	static LeapSensor	*m_instance;

};
