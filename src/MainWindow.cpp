/*
 *	MainWindow.cpp
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#include "../inc/MainWindow.h"
#include <Xlib.h>

MainWindow *MainWindow::m_instance = nullptr;

MainWindow::MainWindow(const char *_title, const int _width, const int _height)
:
	m_grid(),
	m_button1("Testing"),
	m_isFrameUpdated(false),
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
		m_label1.set_size_request(200, 300);
		m_label1.set_text(Glib::ustring(""));

		// Label2
		m_label2.set_size_request(200, 300);
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

		for (std::vector<LeapSensor::Gesture>::const_iterator i = frame.gestures.begin(); i != frame.gestures.end(); i++)
		{
			const LeapSensor::Gesture gesture = *i;
			if (!gesture.gesture.hands().isEmpty())
				(gesture.gesture.hands()[0].isLeft())? m_instance->m_leftTapCount++ : m_instance->m_rightTapCount++;
		}

		for (std::vector<LeapSensor::Hand>::const_iterator i = frame.hands.begin(); i != frame.hands.end(); i++)
		{
			const LeapSensor::Hand hand = *i;
			if ((*i).type == LeapSensor::HandType::Left)
			{
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
				sprintf(m_instance->m_rightLabelBuffer,
						"Right hand:\nIndex:\t%.1f,\t%.1f,\t%.1f\n\nTapCount:\t%d\n",

						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.x,
						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.y,
						hand.fingers[LeapSensor::FingerType::IndexFinger].bones[LeapSensor::BoneType::DistalBone].end.z,

						m_instance->m_rightTapCount);
				flag |= 0x01;
			}
		}

		if (~flag & 0x02)
			strcpy(m_instance->m_leftLabelBuffer, "None\0");
		if (~flag & 0x01)
			strcpy(m_instance->m_rightLabelBuffer, "None\0");
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
	m_label1.set_label(Glib::ustring("HelloWorld!!!\n").append(m_label1.get_text()));
}
