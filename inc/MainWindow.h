/*
 *	MainWindow.h
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#pragma once

#include <iostream>
#include <gtkmm.h>
#include "LeapSensor.h"
#include "SystemController.h"

class MainWindow : public Gtk::Window
{

public:

	MainWindow(const char *_title, const int _width = 640, const int _height = 480);
	virtual ~MainWindow(void);

	static void onFrame(const LeapSensor::Frame &frame);

private:

	static MainWindow				*m_instance;

protected:

	Gtk::Grid							m_grid;
	Gtk::Button						m_button1;

	void onButton1Clicked(void);
	bool updateLabel(void);

	bool								m_isFrameUpdated;
	bool								m_isLeapControlEnabled;

	bool								m_isDown;
	float								m_highestRSpeed;
	bool								m_isDragging;
	LeapSensor::Vector				m_pressPosition;
	float								m_indexPalmDistance;
	int64_t							m_lastTime;

	Gtk::Box							m_box;

	Gtk::Label							m_label1;
	Gtk::Label							m_label2;

	char								m_leftLabelBuffer[255];
	char								m_rightLabelBuffer[255];

	int 									m_leftTapCount;
	int									m_rightTapCount;

};
