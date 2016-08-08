/*
 *	MainWindow.cpp
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#include "MainWindow.h"

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(const char *_title, const int _width, const int _height)
:
	m_grid(),
	m_button1("LeapControl Off"),
	m_isFrameUpdated(false),
	m_isLeapControlEnabled(false),
	m_isDown(false),
	m_highestRSpeed(0.0f),
	m_box(),
	m_label1(),
	m_label2(),
	m_leftLabelBuffer{ 0 },
	m_rightLabelBuffer{ 0 },
	m_leftTapCount(0),
	m_rightTapCount(0)
{
	if (!m_instance)
	{
		m_instance = this;

		// Window
		set_border_width(10);
		set_title(Glib::ustring(_title));
		set_resizable(false);
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);
//		set_default_size(_width, _height);

		// Grid
		add(m_grid);
		m_grid.set_column_homogeneous(false);
		m_grid.set_row_homogeneous(false);
		m_grid.set_column_spacing(5);
		m_grid.add(m_button1);
		m_grid.attach_next_to(m_label1, m_button1, Gtk::POS_BOTTOM, 2, 1);
		m_grid.attach_next_to(m_label2, m_label1, Gtk::POS_RIGHT, 1, 1);

		// Button1
		m_button1.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onButton1Clicked));
//		m_button1.set_size_request(150, 50);

		// Label1
		m_label1.set_size_request(250, 350);
		m_label1.set_text(Glib::ustring(""));

		// Label2
		m_label2.set_size_request(250, 350);
		m_label2.set_text(Glib::ustring(""));

		Glib::signal_idle().connect(sigc::mem_fun(*this, &MainWindow::updateLabel));

		show_all_children();
	}
}

MainWindow::~MainWindow(void)
{}

void MainWindow::onFrame(const LeapSensor::Frame &frame)
{
	try
	{
		uint8_t flag = 0;
		bool isMouseStateChanged = false;

		for (std::vector<LeapSensor::Hand>::const_iterator i = frame.hands.begin(); i != frame.hands.end(); i++)
		{
			const LeapSensor::Hand hand = *i;
			if ((*i).type == LeapSensor::HandType::Left)
			{
				for (std::vector<LeapSensor::Gesture>::const_iterator i = frame.gestures.begin(); i != frame.gestures.end(); i++)
				{
					const Leap::KeyTapGesture gesture = (*i).gesture;
					if (((Leap::Finger)gesture.pointable()).type() == Leap::Finger::TYPE_INDEX && hand.fingers[LeapSensor::FingerType::IndexFinger].id == ((Leap::Finger)gesture.pointable()).id())
						m_instance->m_leftTapCount++;
				}

				sprintf(m_instance->m_leftLabelBuffer,
						"Left hand:\nIndex:\t%.1f,\t%.1f,\t%.1f\n\nTapCount:\t%d\n",

						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.x,
						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.y,
						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.z,

						m_instance->m_leftTapCount);
				flag |= 0x02;
			}
			else
			{
				float	palmSpeed = hand.palm.velocity.y,
						indexFingerSpeed = hand.fingers[LeapSensor::FingerType::IndexFinger].tipVelocity.y;
//				if (((palmSpeed || indexFingerSpeed) && indexFingerSpeed / palmSpeed >= 2.5f) && indexFingerSpeed >= 0.0f && hand.fingers[LeapSensor::FingerType::IndexFinger].tipPosition.y <= hand.palm.position.y)
//				{
//					if (!m_instance->m_isDown)
//					{
//						m_instance->m_rightTapCount++;
//						m_instance->m_pressPosition = hand.palm.position;
//						m_instance->m_isDragging = false;
//						isMouseStateChanged = true;
//					}
//
//					m_instance->m_isDown = true;
//				}
//				else if (hand.fingers[LeapSensor::FingerType::IndexFinger].tipPosition.y > hand.palm.position.y)
//				{
//					if (m_instance->m_isDown)
//						isMouseStateChanged = true;
//
//					m_instance->m_isDown = false;
//				}

				float dt = (frame.timestamp - m_instance->m_lastTime) / 1000.0f;

				if (indexFingerSpeed / dt <= -15.0f && indexFingerSpeed / palmSpeed >= 2.0f)
				{
					if (!m_instance->m_isDown)
					{
						m_instance->m_rightTapCount++;
						m_instance->m_indexPalmDistance = hand.fingers[LeapSensor::FingerType::IndexFinger].tipPosition.y - hand.palm.position.y;//m_instance->m_pressIndexPositionY - m_instance->m_pressPalmPositionY;
						m_instance->m_pressPosition = hand.palm.position;
						m_instance->m_isDown = true;
						m_instance->m_isDragging = false;
						isMouseStateChanged = true;
					}
				}
				else if (indexFingerSpeed / dt >= 5.0f || hand.fingers[LeapSensor::FingerType::IndexFinger].tipPosition.y > hand.palm.position.y + m_instance->m_indexPalmDistance)
				{
					if (m_instance->m_isDown)
					{
						m_instance->m_isDown = false;
						isMouseStateChanged = true;
					}
				}

				float xyDistance = std::sqrt((hand.palm.position.x - m_instance->m_pressPosition.x) * (hand.palm.position.x - m_instance->m_pressPosition.x) + (hand.palm.position.y - m_instance->m_pressPosition.y) * (hand.palm.position.y - m_instance->m_pressPosition.y));

				sprintf(m_instance->m_rightLabelBuffer,
						"Right hand:\nIPD:\t%.2f\nRelativeSpeed:\t%.2f\nxyDistance:\t%.2f\n\nTapCount:\t%d (%c/%c)\n",

//						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.x,
//						hand.fingers[LeapSensor::FingerType::IndexFinger].tipPosition.y,
//						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.y,
//						hand.palm.position.y,
						m_instance->m_indexPalmDistance,

//						m_instance->m_highestRSpeed,
						indexFingerSpeed / palmSpeed,
						xyDistance,
//						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.z,

						m_instance->m_rightTapCount,
						(m_instance->m_isDown)? 'd' : 'n',
						(m_instance->m_isDragging)? 'd': 'n');
				flag |= 0x01;

				if (m_instance->m_isLeapControlEnabled)
				{
//					SystemController::moveMouseTo((int)((inRange(-100.0f, hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.x, 100.0f) + 100.0f) / 200.0f * SystemController::ScreenWidth), (int)((inRange(-40.0f, hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.z, 40.0f) + 40.0f) / 80.0f * SystemController::ScreenHeight));
					if ((!isMouseStateChanged && !m_instance->m_isDown) || m_instance->m_isDragging || xyDistance >= 5.0f)
					{
						m_instance->m_isDragging = true;
						SystemController::moveMouseTo((int)((inRange(-100.0f, hand.palm.position.x, 100.0f) + 100.0f) / 200.0f * SystemController::ScreenWidth), (int)((inRange(-40.0f, hand.palm.position.z, 40.0f) + 40.0f) / 80.0f * SystemController::ScreenHeight));
					}
					else
						SystemController::moveMouseTo((int)((inRange(-100.0f, m_instance->m_pressPosition.x, 100.0f) + 100.0f) / 200.0f * SystemController::ScreenWidth), (int)((inRange(-40.0f, m_instance->m_pressPosition.z, 40.0f) + 40.0f) / 80.0f * SystemController::ScreenHeight));

					if (isMouseStateChanged)
						if (m_instance->m_isDown)
							SystemController::pressMouse(SystemController::MouseButton::LeftButton);
						else
							SystemController::releaseMouse(SystemController::MouseButton::LeftButton);
				}
			}
		}

		m_instance->m_lastTime = frame.timestamp;
	}
	catch (int e)
	{}
	m_instance->m_isFrameUpdated = true;
}

bool MainWindow::updateLabel(void)
{
	if (m_isFrameUpdated)
	{
		try
		{
			m_instance->m_label1.set_label(Glib::ustring(m_leftLabelBuffer));
			m_instance->m_label2.set_label(Glib::ustring(m_rightLabelBuffer));
		}
		catch (int e)
		{}
	}

	return true;
}

void MainWindow::onButton1Clicked(void)
{
	m_isLeapControlEnabled = !m_isLeapControlEnabled;
	m_button1.set_label(Glib::ustring((m_isLeapControlEnabled)? "LeapControl On" : "LeapControl Off"));
}
