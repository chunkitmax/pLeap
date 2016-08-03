/*
 *	SystemController.cpp
 *	
 *	Author: petel__
 *	Copyright (c) 2014-2016
 *	Refer to LICENSE for details
 *	
 */

#include "SystemController.h"

Display				*SystemController::m_display = nullptr;
int					SystemController::ScreenWidth = 0;
int					SystemController::ScreenHeight = 0;
bool				SystemController::m_isXTestSupported = false;

SystemController::SystemController(void)
{}

SystemController::~SystemController(void)
{}

void SystemController::init(void)
{
	if (!m_display)
	{
		int eventBase, errorBase, majorVersion, minorVersion;
		m_display = XOpenDisplay(NULL);
		m_isXTestSupported = XTestQueryExtension(m_display, &eventBase, &errorBase, &majorVersion, &minorVersion);
		Screen defaultScreen = *DefaultScreenOfDisplay(m_display);
		ScreenWidth = defaultScreen.width;
		ScreenHeight = defaultScreen.height;
	}
}

void SystemController::uninit(void)
{
	if (m_display)
		XCloseDisplay(m_display);
}

void SystemController::moveMouse(const int xOffset, const int yOffset)
{
	if (m_display)
		if (m_isXTestSupported)
			_moveMouseTest(xOffset, yOffset);
		else
			_moveMouse(xOffset, yOffset);
}
void SystemController::_moveMouse(const int xOffset, const int yOffset)
{
	Window root, child;
	int winX, winY, rootX, rootY;
	unsigned int mask;
	if (XQueryPointer(m_display, XDefaultRootWindow(m_display), &root, &child, &rootX, &rootY, &winX, &winY, &mask))
		_moveMouseTo(rootX + xOffset, rootY + yOffset);
}
void SystemController::_moveMouseTest(const int xOffset, const int yOffset)
{
	XTestFakeRelativeMotionEvent(m_display, xOffset, yOffset, 0);
	XFlush(m_display);
}

void SystemController::moveMouseTo(const int x, const int y)
{
	if (m_display)
		if (m_isXTestSupported)
			_moveMouseToTest(x, y);
		else
			_moveMouseTo(x, y);
}
void SystemController::_moveMouseTo(const int x, const int y)
{
	XWarpPointer(m_display, None, XDefaultRootWindow(m_display), 0, 0, 0, 0, x, y);
	XFlush(m_display);
}
void SystemController::_moveMouseToTest(const int x, const int y)
{
	XTestFakeMotionEvent(m_display, -1, x, y, 0);
	XFlush(m_display);
}

void SystemController::pressMouse(const MouseButton which)
{
	XTestFakeButtonEvent(m_display, which, true, 0);
	XFlush(m_display);
}

void SystemController::releaseMouse(const MouseButton which)
{
	XTestFakeButtonEvent(m_display, which, false, 0);
	XFlush(m_display);
}
