#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"
#include "headers/getParameters.h"
#include "headers/readConfig.h"
#include "headers/eventLoop.h"

#define ModeContinue /*-*/ ((unsigned int)0)
#define ModeRestart /*--*/ ((unsigned int)1)
#define ModeExit /*-----*/ ((unsigned int)2)

const char *configPath;
unsigned int mode = ModeContinue;
Display *display;
unsigned int monitorAmount;
const XRRMonitorInfo *monitorInfo;
unsigned int totalBoxAmount;
Window *topLevelWindowArray;
unsigned int currentMonitor;

static bool createWindows(void);
static void setTopLevelWindowProperties(void);
static void cleanupWindows(void);

int main(const int argumentCount, const char *const *const argumentVector){
	if(getParameters((unsigned int *)&argumentCount, argumentVector)){
		while(mode == ModeContinue || mode == ModeRestart){
			mode = ModeContinue;
			if((display = XOpenDisplay(NULL))){
				{
					Window rootWindow = XDefaultRootWindow(display);
					monitorInfo = XRRGetMonitors(display, rootWindow, True, (int *)&monitorAmount);
					readConfigScan(&rootWindow);
				}
				Window window[monitorAmount];
				topLevelWindowArray = window;
				if(createWindows()){
					setTopLevelWindowProperties();
					eventLoop();
					cleanupWindows();
				}else{
					fprintf(stderr, "%s: could not create windows\n", ProgramName);
					mode = ModeExit;
				}
				XCloseDisplay(display);
			}else{
				fprintf(stderr, "%s: could not connect to server\n", ProgramName);
				break;
			}
		}
	}
	return 0;
}
static bool createWindows(void){
	bool value;
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int border;
	bytes4 borderColor;
	bytes4 backgroundColor;
	bytes4 globalMenuBorderColor;
	bytes4 globalMenuBackgroundColor;
	unsigned int menuAmount;
	{
		Window rootWindow = XDefaultRootWindow(display);
		for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
			value = 0;
			if(readConfigTopLevelWindow(&rootWindow, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalMenuBorderColor, &globalMenuBackgroundColor, &menuAmount)){
				if(width > 0 && height > 0){
					x += monitorInfo[currentMonitor].x;
					y += monitorInfo[currentMonitor].y;
					XVisualInfo visualInfo;
					XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &visualInfo);
					XSetWindowAttributes windowAttributes = {
						.background_pixel = backgroundColor,
						.border_pixel = borderColor,
						.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone)
					};
					topLevelWindowArray[currentMonitor] = XCreateWindow(display, rootWindow, x, y, width, height, border, visualInfo.depth, InputOutput, visualInfo.visual, CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWColormap, &windowAttributes);
					value = 1;
				}
			}
			if(!value){
				break;
			}
		}
	}
	if(value){
		unsigned int currentMenu;
		Window menu;
		bytes4 globalBoxBorderColor;
		bytes4 globalBoxBackgroundColor;
		unsigned int boxAmount;
		unsigned int currentBox;
		Window box;
		unsigned int innerBoxAmount;
		unsigned int currentInnerBox;
		Window innerBox;
		for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
			value = 0;
			currentMenu = 0;
			while(currentMenu < menuAmount){
				if(readConfigMenuWindow(&topLevelWindowArray[currentMonitor], &currentMenu, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalBoxBorderColor, &globalBoxBackgroundColor, &boxAmount)){
					if(width > 0 && height > 0){
						if(borderColor == 0x00000000){
							borderColor = globalMenuBorderColor;
						}
						if(backgroundColor == 0x00000000){
							backgroundColor = globalMenuBackgroundColor;
						}
						menu = XCreateSimpleWindow(display, topLevelWindowArray[currentMonitor], x, y, width, height, border, borderColor, backgroundColor);
						value = 1;
					}
				}
				if(value){
					value = 0;
					currentBox = 0;
					while(currentBox < boxAmount){
						if(readConfigBoxWindow(&menu, &currentMenu, &currentBox, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &innerBoxAmount)){
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
							currentInnerBox = 0;
							while(currentInnerBox < innerBoxAmount){
								if(readConfigInnerBoxWindow(&box, &currentMenu, &currentBox, &currentInnerBox, &x, &y, &width, &height, &border, &borderColor, &backgroundColor)){
									if(width > 0 && height > 0){
										innerBox = XCreateSimpleWindow(display, box, x, y, width, height, border, borderColor, backgroundColor);
										value = 1;
									}
								}
								if(value){
									value = 0;
									XMapWindow(display, innerBox);
								}else{
									currentInnerBox = innerBoxAmount;
								}
								currentInnerBox++;
							}
							if(currentInnerBox == innerBoxAmount + 1){
								currentBox = boxAmount;
							}
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
			}else{
				break;
			}
		}
	}
	return value;
}
static void setTopLevelWindowProperties(void){
	XTextProperty textProperty = {
		.value = (unsigned char *)ProgramName,
		.encoding = XA_STRING,
		.format = 8,
		.nitems = 8
	};
	XWindowAttributes windowAttributes;
	XSizeHints sizeHints = {
		.flags = PPosition | PSize | PMinSize | PMaxSize | PResizeInc | PAspect | PBaseSize | PWinGravity,
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
		.win_gravity = NorthWestGravity
	};
	XWMHints WMHints = {
		.flags = InputHint | StateHint,
		.input = False,
		.initial_state = NormalState
	};
	XClassHint classHint = {
		.res_name = ProgramName,
		.res_class = ProgramName
	};
	long unsigned int data[12];
	for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
		XGetWindowAttributes(display, topLevelWindowArray[currentMonitor], &windowAttributes);
		sizeHints.x = windowAttributes.x;
		sizeHints.y = windowAttributes.y;
		sizeHints.width = windowAttributes.width;
		sizeHints.height = windowAttributes.height;
		sizeHints.min_width = windowAttributes.width;
		sizeHints.min_height = windowAttributes.height;
		sizeHints.max_width = windowAttributes.width;
		sizeHints.max_height = windowAttributes.height;
		sizeHints.base_width = windowAttributes.width;
		sizeHints.base_height = windowAttributes.height;
		XSetWMName(display, topLevelWindowArray[currentMonitor], &textProperty);
		XSetWMNormalHints(display, topLevelWindowArray[currentMonitor], &sizeHints);
		XSetWMHints(display, topLevelWindowArray[currentMonitor], &WMHints);
		XSetClassHint(display, topLevelWindowArray[currentMonitor], &classHint);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)ProgramName, 8);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_VISIBLE_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)ProgramName, 8);
		data[0] = 0xFFFFFFFF;
		data[1] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
		data[2] = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
		data[3] = XInternAtom(display, "_NET_WM_ACTION_STICK", False);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_DESKTOP", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data[0], 1);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[1], 1);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[2], 1);
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[3], 1);
		data[0] = 0;
		data[1] = 0;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		if(windowAttributes.y < monitorInfo[currentMonitor].height / 2){
			data[2] = windowAttributes.y;
			data[2] += windowAttributes.height;
			data[3] = 0;
			data[8] = windowAttributes.x;
			data[9] = windowAttributes.x;
			data[9] += windowAttributes.width;
			data[9] -= 1;
			data[10] = 0;
			data[11] = 0;
		}else{
			data[2] = 0;
			data[3] = XDisplayHeight(display, XDefaultScreen(display));
			data[3] -= windowAttributes.y;
			data[8] = 0;
			data[9] = 0;
			data[10] = windowAttributes.x;
			data[11] = windowAttributes.x;
			data[11] += windowAttributes.width;
			data[11] -= 1;
		}
		XChangeProperty(display, topLevelWindowArray[currentMonitor], XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 12);
		XSelectInput(display, topLevelWindowArray[currentMonitor], KeyPressMask | ButtonPressMask | ExposureMask);
	}
	return;
}
static void cleanupWindows(void){
	for(currentMonitor = 0; currentMonitor < monitorAmount; currentMonitor++){
		XUnmapSubwindows(display, topLevelWindowArray[currentMonitor]);
		XDestroySubwindows(display, topLevelWindowArray[currentMonitor]);
		XUnmapWindow(display, topLevelWindowArray[currentMonitor]);
		XDestroyWindow(display, topLevelWindowArray[currentMonitor]);
	}
	return;
}
