/*
 *	main.cpp
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016 HKUST SmartCar Team
 *	Refer to LICENSE for details
 *	
 */

#include <cstdio>
#include <iostream>
#include <gtkmm.h>
#include "../inc/LeapSensor.h"
#include "../inc/MainWindow.h"

void windowRemoved(Gtk::Window *window)
{
	Gtk::Main::quit();
}

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> pApp = Gtk::Application::create(argc, argv, Glib::ustring("com.petel.linuxapp"));

	MainWindow window("Petel__'s Leap Motion Testing Program");

	LeapSensor leap(LeapSensor::GestureFlag::KeyTapGesture);
	leap.setOnFrameListener(MainWindow::onFrame);

	pApp->signal_window_removed().connect(sigc::ptr_fun(&windowRemoved));

	return pApp->run(window);
}
