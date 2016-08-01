/*
 *	SystemController.h
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#pragma once

#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

class SystemController
{

public:

	typedef enum MouseButton
	{
		LeftButton = 1,
		RightButton,
		MiddleButton,
		WheelUp,
		WheelDown
	};

	SystemController(void);
	~SystemController(void);

	static void init(void);
	static void uninit(void);

	static void moveMouse(const int xOffset, const int yOffset);
	static void moveMouseTo(const int x, const int y);
	static void click(const MouseButton which);

	static int						ScreenWidth;
	static int						ScreenHeight;

private:

	static void _moveMouse(const int xOffset, const int yOffset);
	static void _moveMouseTest(const int xOffset, const int yOffset);

	static void _moveMouseTo(const int x, const int y);
	static void _moveMouseToTest(const int x, const int y);

	static Display					*m_display;

	static bool					m_isXTestSupported;

};


