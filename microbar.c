// clear; cd $HOME/Downloads/microbar/; gcc -Wall -Wextra -pedantic microbar.c getParameters.c readConfig.c eventLoop.c $(pkg-config --cflags --libs x11) -o output/microbar; "$HOME/Downloads/microbar/output/microbar" -c "$HOME/Downloads/microbar/output/microbar.config"

#include <stdio.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "headers/getParameters.h"
#include "headers/readConfig.h"
#include "headers/eventLoop.h"

#define ModeContinue /*-*/ ((unsigned int)0)
#define ModeRestart /*--*/ ((unsigned int)1)
#define ModeExit /*-----*/ ((unsigned int)2)

static unsigned int createWindows(Display *const display, const char *const pathArray, Window *const topLevelWindow);
static void setTopLevelWindowProperties(Display *const display, const Window *const window);
static void cleanupWindows(Display *const display, const Window *const topLevelWindow);

int main(const int argumentCount, const char *const *const argumentVector){
	const char *configPath;
	if(getParameters(&argumentCount, argumentVector, &configPath)){
		unsigned int mode = ModeContinue;
		Display *display;
		Window window;
		while(mode == ModeContinue || mode == ModeRestart){
			mode = ModeContinue;
			if((display = XOpenDisplay(NULL))){
				if(createWindows(display, configPath, &window)){
					setTopLevelWindowProperties(display, &window);
					eventLoop(display, &window, configPath, &mode);
					cleanupWindows(display, &window);
				}else{
					mode = ModeExit;
					fprintf(stderr, "microbar: could not create windows\n");
				}
				XCloseDisplay(display);
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
static void setTopLevelWindowProperties(Display *const display, const Window *const window){
	const Window dereferencedWindow = *window;
	{
		XTextProperty textProperty = {
			.value = (unsigned char *)"microbar",
			.encoding = XA_STRING,
			.format = 8,
			.nitems = 8
		};
		XSetWMName(display, dereferencedWindow, &textProperty);
	}
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(display, dereferencedWindow, &windowAttributes);
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
		XSetWMNormalHints(display, dereferencedWindow, &sizeHints);
	}
	{
		XWMHints WMHints = {
			.flags = InputHint | StateHint,
			.input = False,
			.initial_state = NormalState
		};
		XSetWMHints(display, dereferencedWindow, &WMHints);
	}
	{
		XClassHint classHint = {
			.res_name = "microbar",
			.res_class = "microbar"
		};
		XSetClassHint(display, dereferencedWindow, &classHint);
	}
	{
		const unsigned char data[] = "microbar";
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, data, 8);
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_VISIBLE_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, data, 8);
	}
	{
		unsigned long int data = 0xFFFFFFFF;
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_DESKTOP", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 1);
		data = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
		data = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
		data = XInternAtom(display, "_NET_WM_ACTION_STICK", False);
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data, 1);
	}
	{
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(display, dereferencedWindow, &windowAttributes);
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
		XChangeProperty(display, dereferencedWindow, XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 12);
	}
	XSelectInput(display, dereferencedWindow, ExposureMask);
	return;
}
static void cleanupWindows(Display *const display, const Window *const topLevelWindow){
	const Window dereferencedTopLevelWindow = *topLevelWindow;
	XUnmapSubwindows(display, dereferencedTopLevelWindow);
	XDestroySubwindows(display, dereferencedTopLevelWindow);
	XUnmapWindow(display, dereferencedTopLevelWindow);
	XDestroyWindow(display, dereferencedTopLevelWindow);
	return;
}

