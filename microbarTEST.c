/*#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

static void printInfo(const XRRMonitorInfo *const monitorInfo){
	fprintf(stdout, "primary: %u\n", (*monitorInfo).primary);
	fprintf(stdout, "x: %u\n", (*monitorInfo).x);
	fprintf(stdout, "y: %u\n", (*monitorInfo).y);
	fprintf(stdout, "width: %u\n", (*monitorInfo).width);
	fprintf(stdout, "height: %u\n", (*monitorInfo).height);
	fprintf(stdout, "\n");
	return;
}
int main(void){
	Display *display = XOpenDisplay(NULL);
	int monitorsAmount;
	XRRMonitorInfo *monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, &monitorsAmount);
	fprintf(stdout, "monitors: %u\n\n", monitorsAmount);
	for(int i = 0; i < monitorsAmount; i++){
		printInfo(&monitorInfo[i]);
	}
	return 0;
}*/

#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include "headers/getParameters.h"
#include "headers/readConfig.h"
#include "headers/eventLoop.h"

#define ModeContinue /*-*/ ((unsigned int)0)
#define ModeRestart /*--*/ ((unsigned int)1)
#define ModeExit /*-----*/ ((unsigned int)2)

static unsigned int createWindows(Display *const display, const char *const pathArray, Window *const topLevelWindow);
static void setTopLevelWindowProperties(Display *const display, const Window *const window, const unsigned int *const windowAmount);
static void cleanupWindows(Display *const display, const Window *const topLevelWindowArray, const unsigned int *const topLevelWindowAmount);

int main(const int argumentCount, const char *const *const argumentVector){
	const char *configPath;
	if(getParameters(&argumentCount, argumentVector, &configPath)){
		unsigned int mode = ModeContinue;
		Display *display;
		unsigned int windowAmount;
		XRRMonitorInfo *monitorInfo;
		while(mode == ModeContinue || mode == ModeRestart){
			if(mode == ModeRestart){
				mode = ModeContinue;
			}
			if((display = XOpenDisplay(NULL))){
				monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, (int *)&windowAmount);
				Window window[windowAmount];
				if(createWindows(display, configPath, window)){
					setTopLevelWindowProperties(display, window, &windowAmount);
					eventLoop(display, window, configPath, &mode);
					cleanupWindows(display, window, &windowAmount);
				}else{
					mode = ModeExit;
					fprintf(stderr, "microbar: could not create windows\n");
				}
				XCloseDisplay(display);
			}else{
				fprintf(stderr, "microbar: could not connect to server\n");
				break;
			}
		}
	}
	return 0;
}
static unsigned int createWindows(Display *const display, const char *const pathArray, Window *const topLevelWindow){
	unsigned int value = 0;
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	unsigned int border;
	int borderColor;
	int backgroundColor;
	int globalMenuBorderColor;
	int globalMenuBackgroundColor;
	unsigned int menuAmount;
	{
		Window rootWindow = XDefaultRootWindow(display);
		if(readConfigTopLevelWindow(display, pathArray, &rootWindow, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalMenuBorderColor, &globalMenuBackgroundColor, &menuAmount)){
			if(width > 0 && height > 0){
				XVisualInfo visualInfo;
				XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &visualInfo);
				XSetWindowAttributes windowAttributes = {
					.background_pixel = backgroundColor,
					.border_pixel = borderColor,
					.colormap = XCreateColormap(display, XDefaultRootWindow(display), visualInfo.visual, AllocNone)
				};
				*topLevelWindow = XCreateWindow(display, rootWindow, x, y, width, height, border, visualInfo.depth, InputOutput, visualInfo.visual, CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWColormap, &windowAttributes);
				value = 1;
			}
		}
	}
	if(value){
		value = 0;
		unsigned int currentMenu = 0;
		Window menu;
		int globalBoxBorderColor;
		int globalBoxBackgroundColor;
		unsigned int boxAmount;
		unsigned int currentBox;
		Window box;
		while(currentMenu < menuAmount){
			if(readConfigMenuWindow(display, pathArray, topLevelWindow, &currentMenu, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalBoxBorderColor, &globalBoxBackgroundColor, &boxAmount)){
				if(width > 0 && height > 0){
					if(borderColor == 0x00000000){
						borderColor = globalMenuBorderColor;
					}
					if(backgroundColor == 0x00000000){
						backgroundColor = globalMenuBackgroundColor;
					}
					menu = XCreateSimpleWindow(display, *topLevelWindow, x, y, width, height, border, borderColor, backgroundColor);
					value = 1;
				}
			}
			if(value){
				value = 0;
				currentBox = 0;
				while(currentBox < boxAmount){
					if(readConfigBoxWindow(display, pathArray, &menu, &currentMenu, &currentBox, &x, &y, &width, &height, &border, &borderColor, &backgroundColor)){
						if(width > 0 && height > 0){
							if(borderColor == 0x00000000){
								borderColor = globalBoxBorderColor;
							}
							if(backgroundColor == 0x00000000){
								backgroundColor = globalBoxBackgroundColor;
							}
							box = XCreateSimpleWindow(display, menu, x, y, width, height, border, borderColor, backgroundColor);
							value = 1;
						}
					}
					if(value){
						value = 0;
						XMapWindow(display, box);
					}else{
						currentBox = boxAmount;
					}
					currentBox++;
				}
				if(currentBox == boxAmount + 1){
					currentMenu = menuAmount;
				}
				XMapWindow(display, menu);
			}else{
				currentMenu = menuAmount;
			}
			currentMenu++;
		}
		if(currentMenu == menuAmount){
			value = 1;
		}
	}
	return value;
}
static void setTopLevelWindowProperties(Display *const display, const Window *const windowArray, const unsigned int *const windowAmount){
	unsigned int currentWindow;
	const unsigned int dereferencedWindowAmount = *windowAmount;
	{
		XTextProperty textProperty = {
			.value = (unsigned char *)"microbar",
			.encoding = XA_STRING,
			.format = 8,
			.nitems = 8
		};
		for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
			XSetWMName(display, windowArray[currentWindow], &textProperty);
		}
	}









	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(display, windowArray[0], &windowAttributes);
		XSizeHints sizeHints = {
			.flags = PPosition | PSize | PMinSize | PMaxSize | PResizeInc | PAspect | PBaseSize | PWinGravity,
			.x = windowAttributes.x,
			.y = windowAttributes.y,
			.width = windowAttributes.width,
			.height = windowAttributes.height,
			.min_width = windowAttributes.width,
			.min_height = windowAttributes.height,
			.max_width = windowAttributes.width,
			.max_height = windowAttributes.height,
			.width_inc = 0,
			.height_inc = 0,
			.min_aspect = {
				.x = 1,
				.y = 1
			},
			.max_aspect = {
				.x = 1,
				.y = 1
			},
			.base_width = windowAttributes.width,
			.base_height = windowAttributes.height,
			.win_gravity = NorthWestGravity
		};
		XSetWMNormalHints(display, windowArray[0], &sizeHints);
	}









	{
		XWMHints WMHints = {
			.flags = InputHint | StateHint,
			.input = False,
			.initial_state = NormalState
		};
		for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
			XSetWMHints(display, windowArray[currentWindow], &WMHints);
		}
	}
	{
		XClassHint classHint = {
			.res_name = "microbar",
			.res_class = "microbar"
		};
		for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
			XSetClassHint(display, windowArray[currentWindow], &classHint);
		}
	}
	for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
		XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)"microbar", 8);
		XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_VISIBLE_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)"microbar", 8);
	}
	{
		unsigned long int data = 0xFFFFFFFF;
		for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
			XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_DESKTOP", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 1);
			data = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
			XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
			data = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
			XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
			data = XInternAtom(display, "_NET_WM_ACTION_STICK", False);
			XChangeProperty(display, windowArray[currentWindow], XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
		}
	}









	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(display, windowArray[0], &windowAttributes);
		unsigned long int data[12] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		if(windowAttributes.y < XDisplayHeight(display, XDefaultScreen(display)) / 2){
			data[2] = windowAttributes.y + windowAttributes.height;
			data[8] = windowAttributes.x;
			data[9] = windowAttributes.x + windowAttributes.width - 1;
		}else{
			data[3] = XDisplayHeight(display, XDefaultScreen(display)) - windowAttributes.y;
			data[10] = windowAttributes.x;
			data[11] = windowAttributes.x + windowAttributes.width - 1;
		}
		XChangeProperty(display, windowArray[0], XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 12);
	}









	for(currentWindow = 0; currentWindow < dereferencedWindowAmount; currentWindow++){
		XSelectInput(display, windowArray[currentWindow], ExposureMask);
	}
	return;
}
static void cleanupWindows(Display *const display, const Window *const topLevelWindowArray, const unsigned int *const topLevelWindowAmount){
	for(unsigned int currentWindow = 0; currentWindow < *topLevelWindowAmount; currentWindow++){
		XUnmapSubwindows(display, topLevelWindowArray[currentWindow]);
		XDestroySubwindows(display, topLevelWindowArray[currentWindow]);
		XUnmapWindow(display, topLevelWindowArray[currentWindow]);
		XDestroyWindow(display, topLevelWindowArray[currentWindow]);
	}
	return;
}
