/* microbar.c

MIT License

Copyright (C) 2019 Stefanos "Steven" Tsakiris

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <stdint.h>
#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "headers/defines.h"
#include "headers/getParameters.h"
#include "headers/readConfig.h"
#include "headers/eventLoop.h"

extern const char *programName;
extern const char *configPath;
extern const char *workplacePath;
extern unsigned int workplacePathLength;
extern unsigned int drawableCommandPathLength;
extern const char *drawableCommandPath;
extern uint8_t mode;
extern Display *display;
extern unsigned int monitorAmount;
extern Window *topLevelWindow;
extern unsigned int currentMonitor;

static void start(void);
static bool createWindows(void);
static void setTopLevelWindowProperties(void);
static void cleanup(void);

int main(const int argumentCount, const char *const *const argumentVector){
	if(getParameters((unsigned int)argumentCount, argumentVector)){
		if(workplacePath){
			workplacePathLength = 1;
			while(workplacePath[workplacePathLength]){
				++workplacePathLength;
			}
			if(workplacePath[workplacePathLength - 1] == '/'){
				--workplacePathLength;
			}
			start();
		}else{
			workplacePathLength = 1;
			while(configPath[workplacePathLength]){
				++workplacePathLength;
			}
			do{
				--workplacePathLength;
			}while(configPath[workplacePathLength] != '/');
			char _workplacePath[workplacePathLength + 1];
			{
				unsigned int element;
				for(element = 0; element < workplacePathLength; ++element){
					_workplacePath[element] = configPath[element];
				}
				_workplacePath[element] = '\0';
			}
			workplacePath = _workplacePath;
			start();
		}
	}
	return 0;
}
static void start(void){
	drawableCommandPathLength = workplacePathLength;
	drawableCommandPathLength += 16;
	char _drawableCommandPath[drawableCommandPathLength + 1];
	{
		unsigned int element = 0;
		for(element = 0; element < workplacePathLength; ++element){
			_drawableCommandPath[element] = workplacePath[element];
		}
		_drawableCommandPath[element] = '/';
		_drawableCommandPath[++element] = 'd';
		_drawableCommandPath[++element] = 'r';
		_drawableCommandPath[++element] = 'a';
		_drawableCommandPath[++element] = 'w';
		_drawableCommandPath[++element] = 'a';
		_drawableCommandPath[++element] = 'b';
		_drawableCommandPath[++element] = 'l';
		_drawableCommandPath[++element] = 'e';
		_drawableCommandPath[++element] = 'C';
		_drawableCommandPath[++element] = 'o';
		_drawableCommandPath[++element] = 'm';
		_drawableCommandPath[++element] = 'm';
		_drawableCommandPath[++element] = 'a';
		_drawableCommandPath[++element] = 'n';
		_drawableCommandPath[++element] = 'd';
		_drawableCommandPath[++element] = '\0';
	}
	drawableCommandPath = _drawableCommandPath;
	mode = ContinueMode;
	while(mode != ExitMode){
		mode = ContinueMode;
		if((display = XOpenDisplay(NULL))){
			if(setlocale(LC_CTYPE, "")){
				if(!XSupportsLocale()){
					fprintf(stderr, "%s: locale is not supported\n", programName);
				}
			}else{
				fprintf(stderr, "%s: could not set locale\n", programName);
			}
			{
				XRRMonitorInfo *const monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, (int *)&monitorAmount);
				if(monitorInfo){
					XRRFreeMonitors(monitorInfo);
				}else{
					monitorAmount = 0;
				}
			}
			if(monitorAmount){
				if(readConfigScan()){
					Window _topLevelWindow[monitorAmount];
					topLevelWindow = _topLevelWindow;
					if(createWindows()){
						setTopLevelWindowProperties();
						eventLoop();
					}else{
						fprintf(stderr, "%s: could not create windows\n", programName);
						mode = ExitMode;
					}
					cleanup();
				}else{
					mode = ExitMode;
				}
			}else{
				fprintf(stderr, "%s: there are no monitors to display on\n", programName);
				mode = ExitMode;
			}
			XCloseDisplay(display);
		}else{
			fprintf(stderr, "%s: could not connect to server\n", programName);
			mode = ExitMode;
		}
	}
	return;
}
static bool createWindows(void){
	bool value;
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int border;
	uint32_t borderColor;
	uint32_t backgroundColor;
	uint32_t globalSectionBorderColor;
	uint32_t globalSectionBackgroundColor;
	unsigned int sectionAmount;
	{
		XRRMonitorInfo *monitorInfo;
		{
			int monitorAmount;
			monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, &monitorAmount);
		}
		if(monitorInfo){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				value = 0;
				if(readConfigTopLevelWindow(XDefaultRootWindow(display), &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalSectionBorderColor, &globalSectionBackgroundColor, &sectionAmount)){
					if(width > 0 && height > 0){
						x += monitorInfo[currentMonitor].x;
						y += monitorInfo[currentMonitor].y;
						XVisualInfo visualInfo;
						XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &visualInfo);
						XSetWindowAttributes windowAttributes = {
							.background_pixel = backgroundColor,
							.border_pixel = borderColor,
							.colormap = XCreateColormap(display, XDefaultRootWindow(display), visualInfo.visual, AllocNone)
						};
						topLevelWindow[currentMonitor] = XCreateWindow(display, XDefaultRootWindow(display), x, y, width, height, border, visualInfo.depth, InputOutput, visualInfo.visual, CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWColormap, &windowAttributes);
						value = 1;
					}
				}
				if(!value){
					break;
				}
			}
			XRRFreeMonitors(monitorInfo);
		}else{
			value = 0;
		}
	}
	if(value){
		unsigned int currentSection;
		Window section;
		uint32_t globalContainerBorderColor;
		uint32_t globalContainerBackgroundColor;
		unsigned int containerAmount;
		unsigned int currentContainer;
		Window container;
		uint32_t globalRectangleBorderColor;
		uint32_t globalRectangleBackgroundColor;
		unsigned int rectangleAmount;
		unsigned int currentRectangle;
		Window rectangle;
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			value = 0;
			currentSection = 0;
			while(currentSection < sectionAmount){
				if(readConfigSectionWindow(topLevelWindow[currentMonitor], currentSection, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalContainerBorderColor, &globalContainerBackgroundColor, &containerAmount)){
					if(width > 0 && height > 0){
						if(borderColor == 0x00000000){
							borderColor = globalSectionBorderColor;
						}
						if(backgroundColor == 0x00000000){
							backgroundColor = globalSectionBackgroundColor;
						}
						section = XCreateSimpleWindow(display, topLevelWindow[currentMonitor], x, y, width, height, border, borderColor, backgroundColor);
						value = 1;
					}
				}
				if(value){
					value = 0;
					currentContainer = 0;
					while(currentContainer < containerAmount){
						if(readConfigContainerWindow(section, currentSection, currentContainer, &x, &y, &width, &height, &border, &borderColor, &backgroundColor, &globalRectangleBorderColor, &globalRectangleBackgroundColor, &rectangleAmount)){
							if(width > 0 && height > 0){
								if(borderColor == 0x00000000){
									borderColor = globalContainerBorderColor;
								}
								if(backgroundColor == 0x00000000){
									backgroundColor = globalContainerBackgroundColor;
								}
								container = XCreateSimpleWindow(display, section, x, y, width, height, border, borderColor, backgroundColor);
								value = 1;
							}
						}
						if(value){
							value = 0;
							currentRectangle = 0;
							while(currentRectangle < rectangleAmount){
								if(readConfigRectangleWindow(container, currentSection, currentContainer, currentRectangle, &x, &y, &width, &height, &border, &borderColor, &backgroundColor)){
									if(width > 0 && height > 0){
										if(borderColor == 0x00000000){
											borderColor = globalRectangleBorderColor;
										}
										if(backgroundColor == 0x00000000){
											backgroundColor = globalRectangleBackgroundColor;
										}
										rectangle = XCreateSimpleWindow(display, container, x, y, width, height, border, borderColor, backgroundColor);
										value = 1;
									}
								}
								if(value){
									value = 0;
									XMapWindow(display, rectangle);
								}else{
									currentRectangle = rectangleAmount;
								}
								++currentRectangle;
							}
							if(currentRectangle == rectangleAmount + 1){
								currentContainer = containerAmount;
							}
							XMapWindow(display, container);
						}else{
							currentContainer = containerAmount;
						}
						++currentContainer;
					}
					if(currentContainer == containerAmount + 1){
						currentSection = sectionAmount;
					}
					XSelectInput(display, section, ExposureMask);
					XMapWindow(display, section);
				}else{
					currentSection = sectionAmount;
				}
				++currentSection;
			}
			if(currentSection == sectionAmount){
				value = 1;
			}else{
				break;
			}
		}
	}
	return value;
}
static void setTopLevelWindowProperties(void){
	unsigned int programNameLength = 0;
	while(programName[programNameLength] != '\0'){
		++programNameLength;
	}
	XTextProperty textProperty = {
		.value = (unsigned char *)programName,
		.encoding = XA_STRING,
		.format = 8,
		.nitems = programNameLength
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
		.win_gravity = ForgetGravity
	};
	XWMHints WMHints = {
		.flags = InputHint | StateHint,
		.input = False,
		.initial_state = NormalState
	};
	XClassHint classHint = {
		.res_name = (char *)programName,
		.res_class = (char *)programName
	};
	unsigned long int data[12];
	XRRMonitorInfo *monitorInfo;
	{
		int monitorAmount;
		monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, &monitorAmount);
	}
	if(monitorInfo){
		for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
			XGetWindowAttributes(display, topLevelWindow[currentMonitor], &windowAttributes);
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
			XSetWMName(display, topLevelWindow[currentMonitor], &textProperty);
			XSetWMNormalHints(display, topLevelWindow[currentMonitor], &sizeHints);
			XSetWMHints(display, topLevelWindow[currentMonitor], &WMHints);
			XSetClassHint(display, topLevelWindow[currentMonitor], &classHint);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)programName, programNameLength);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_VISIBLE_NAME", False), XInternAtom(display, "UTF8_STRING", False), 8, PropModeReplace, (const unsigned char *)programName, programNameLength);
			data[0] = 0xFFFFFFFF;
			data[1] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
			data[2] = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
			data[3] = XInternAtom(display, "_NET_WM_ACTION_STICK", False);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_DESKTOP", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data[0], 1);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[1], 1);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[2], 1);
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_ALLOWED_ACTIONS", False), XA_ATOM, 32, PropModeReplace, (unsigned char *)&data[3], 1);
			data[0] = 0;
			data[1] = 0;
			data[4] = 0;
			data[5] = 0;
			data[6] = 0;
			data[7] = 0;
			if(windowAttributes.y < monitorInfo[currentMonitor].height / 2){
				data[2] = windowAttributes.y + windowAttributes.height;
				data[3] = 0;
				data[8] = windowAttributes.x;
				data[9] = windowAttributes.x + windowAttributes.width - 1;
				data[10] = 0;
				data[11] = 0;
			}else{
				data[2] = 0;
				data[3] = XDisplayHeight(display, XDefaultScreen(display)) - windowAttributes.y;
				data[8] = 0;
				data[9] = 0;
				data[10] = windowAttributes.x;
				data[11] = windowAttributes.x + windowAttributes.width - 1;
			}
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 12);
			XSelectInput(display, topLevelWindow[currentMonitor], KeyPressMask | ButtonPressMask | ExposureMask);
			XRRSelectInput(display, topLevelWindow[currentMonitor], RRScreenChangeNotifyMask);
		}
		XRRFreeMonitors(monitorInfo);
	}
	return;
}
static void cleanup(void){
	for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
		XUnmapSubwindows(display, topLevelWindow[currentMonitor]);
		XDestroySubwindows(display, topLevelWindow[currentMonitor]);
		XUnmapWindow(display, topLevelWindow[currentMonitor]);
		XDestroyWindow(display, topLevelWindow[currentMonitor]);
	}
	return;
}
