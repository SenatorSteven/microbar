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
extern Mode mode;
extern Display *display;
extern unsigned int monitorAmount;
extern unsigned int whichMonitor;
extern unsigned int containerAmount;
extern Window *topLevelWindow;
extern unsigned int currentMonitor;
extern Window *const *container;

static void start(void);
static bool createTopLevelWindows(void);
static void setTopLevelWindowProperties(void);
static bool createSubwindows(void);
static bool createWindow(Window *const window, const Window parentWindow, const int x, const int y, unsigned int width, unsigned int height, const unsigned int border, const ARGB borderColor, const ARGB backgroundColor);
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
	for(;;){
		mode = ContinueMode;
		if((display = XOpenDisplay(NULL))){
			setlocale(LC_CTYPE, "");
			if(!XSupportsLocale()){
				fprintf(stderr, "%s: locale is not supported\n", programName);
			}
			{
				XRRMonitorInfo *const monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, (int *)&monitorAmount);
				if(monitorInfo){
					for(currentMonitor = 1; currentMonitor < monitorAmount; ++currentMonitor){
						if(monitorInfo[currentMonitor - 1].x == monitorInfo[currentMonitor].x && monitorInfo[currentMonitor - 1].y == monitorInfo[currentMonitor].y){
							--monitorAmount;
						}
					}
					XRRFreeMonitors(monitorInfo);
				}else{
					monitorAmount = 0;
				}
			}
			if(monitorAmount){
				{
					ConfigInfo ci;
					if(!readConfig(ScanConfigMode, ci)){
						mode = ExitMode;
					}
				}
				if(mode == ContinueMode){
					Window _topLevelWindow[monitorAmount];
					topLevelWindow = _topLevelWindow;
					if(createTopLevelWindows()){
						setTopLevelWindowProperties();
						Window __container[monitorAmount][containerAmount];
						Window *_container[monitorAmount];
						for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
							_container[currentMonitor] = __container[currentMonitor];
						}
						container = _container;
						if(!createSubwindows()){
							fprintf(stderr, "%s: could not create subwindows\n", programName);
						}
						eventLoop();
						cleanup();
					}else{
						fprintf(stderr, "%s: could not create top level windows\n", programName);
						break;
					}
				}else{
					break;
				}
			}else{
				fprintf(stderr, "%s: there are no monitors to display on\n", programName);
				break;
			}
			XCloseDisplay(display);
		}else{
			fprintf(stderr, "%s: could not connect to server\n", programName);
			break;
		}
		if(mode == ExitMode){
			break;
		}
	}
	return;
}
static bool createTopLevelWindows(void){
	bool value = 0;
	Window rootWindow = XDefaultRootWindow(display);
	unsigned int trueMonitorAmount;
	XRRMonitorInfo *const monitorInfo = XRRGetMonitors(display, rootWindow, True, (int *)&trueMonitorAmount);
	if(monitorInfo){
		int x[monitorAmount];
		int y[monitorAmount];
		unsigned int width[monitorAmount];
		unsigned int height[monitorAmount];
		unsigned int border[monitorAmount];
		ARGB borderColor;
		ARGB backgroundColor;
		XVisualInfo visualInfo;
		XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &visualInfo);
		XSetWindowAttributes setWindowAttributes = {
			.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone)
		};
		{
			const Window *windowArray[1] = {&rootWindow};
			int *_intArray[1][2] = {{x, y}};
			int **intArray[1] = {*_intArray};
			unsigned int *_uintArray[1][3] = {{width, height, border}};
			unsigned int **uintArray[1] = {*_uintArray};
			ARGB *_argbArray[1][2] = {{&borderColor, &backgroundColor}};
			ARGB **argbArray[1] = {*_argbArray};
			ConfigInfo configInfo = {
				.window = windowArray,
				.integer = intArray,
				.integerDimension0 = 1,
				.integerDimension1 = 2,
				.integerDimension2 = monitorAmount,
				.unsignedInteger = uintArray,
				.unsignedIntegerDimension0 = 1,
				.unsignedIntegerDimension1 = 3,
				.unsignedIntegerDimension2 = monitorAmount,
				.argb = argbArray,
				.argbDimension0 = 1,
				.argbDimension1 = 2,
				.argbDimension2 = 1,
				.character = NULL
			};
			if(!readConfig(TopLevelWindowsConfigMode, configInfo)){
				mode = ExitMode;
			}
		}
		if(mode == ContinueMode){
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				value = 0;
				if(width[currentMonitor] && height[currentMonitor]){
					if(whichMonitor == trueMonitorAmount){
						x[currentMonitor] += monitorInfo[currentMonitor].x;
						y[currentMonitor] += monitorInfo[currentMonitor].y;
					}else{
						x[currentMonitor] += monitorInfo[whichMonitor].x;
						y[currentMonitor] += monitorInfo[whichMonitor].y;
					}
					setWindowAttributes.background_pixel = backgroundColor;
					setWindowAttributes.border_pixel = borderColor;
					topLevelWindow[currentMonitor] = XCreateWindow(display, rootWindow, x[currentMonitor], y[currentMonitor], width[currentMonitor], height[currentMonitor], border[currentMonitor], visualInfo.depth, InputOutput, visualInfo.visual, CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWColormap, &setWindowAttributes);
					XSync(display, False);
					value = 1;
				}
				if(!value){
					fprintf(stderr, "%s: could not create top level window %u\n", programName, currentMonitor);
					break;
				}
			}
		}
		XRRFreeMonitors(monitorInfo);
	}
	return value;
}
static void setTopLevelWindowProperties(void){
	unsigned int programNameLength = 1;
	while(programName[programNameLength]){
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
		.input = True,
		.initial_state = NormalState
	};
	XClassHint classHint = {
		.res_name = (char *)programName,
		.res_class = (char *)programName
	};
	unsigned long int data[12];
	unsigned int trueMonitorAmount;
	XRRMonitorInfo *const monitorInfo = XRRGetMonitors(display, XDefaultRootWindow(display), True, (int *)&trueMonitorAmount);
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
			if(whichMonitor == trueMonitorAmount){
				if(windowAttributes.y - monitorInfo[currentMonitor].y < monitorInfo[currentMonitor].height / 2){
					goto topPlacement;
				}else{
					goto bottomPlacement;
				}
			}else{
				if(windowAttributes.y - monitorInfo[whichMonitor].y < monitorInfo[whichMonitor].height / 2){
					topPlacement:{
						data[2] = windowAttributes.y + windowAttributes.height;
						data[3] = 0;
						data[8] = windowAttributes.x;
						data[9] = windowAttributes.x + windowAttributes.width - 1;
						data[10] = 0;
						data[11] = 0;
					}
				}else{
					bottomPlacement:{
						data[2] = 0;
						data[3] = XDisplayHeight(display, XDefaultScreen(display)) - windowAttributes.y;
						data[8] = 0;
						data[9] = 0;
						data[10] = windowAttributes.x;
						data[11] = windowAttributes.x + windowAttributes.width - 1;
					}
				}
			}
			XChangeProperty(display, topLevelWindow[currentMonitor], XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&data, 12);
			XSelectInput(display, topLevelWindow[currentMonitor], KeyPressMask | ButtonPressMask | ExposureMask);
			XRRSelectInput(display, topLevelWindow[currentMonitor], RRScreenChangeNotifyMask);
		}
		XRRFreeMonitors(monitorInfo);
	}
	return;
}
static bool createSubwindows(void){
	bool value = 0;
	unsigned int sectionAmount;
	unsigned int rectangleAmount;
	{
		unsigned int *_uintArray[1][2] = {{&sectionAmount, &rectangleAmount}};
		unsigned int **uintArray[1] = {*_uintArray};
		ConfigInfo configInfo = {
			.integer = NULL,
			.unsignedInteger = uintArray,
			.unsignedIntegerDimension0 = 1,
			.unsignedIntegerDimension1 = 2,
			.unsignedIntegerDimension2 = 1,
			.argb = NULL,
			.character = NULL
		};
		if(!readConfig(SectionRectangleAmountConfigMode, configInfo)){
			mode = ExitMode;
		}
	}
	if(mode == ContinueMode){
		int *x[monitorAmount];
		int *y[monitorAmount];
		unsigned int *width[monitorAmount];
		unsigned int *height[monitorAmount];
		unsigned int *border[monitorAmount];
		unsigned int childrenAmount;
		unsigned int currentContainer;
		{
			unsigned int currentSection;
			Window _section[monitorAmount][sectionAmount];
			Window *section[monitorAmount];
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				section[currentMonitor] = _section[currentMonitor];
			}
			{
				int _x[monitorAmount][sectionAmount];
				int _y[monitorAmount][sectionAmount];
				unsigned int _width[monitorAmount][sectionAmount];
				unsigned int _height[monitorAmount][sectionAmount];
				unsigned int _border[monitorAmount][sectionAmount];
				ARGB borderColor[sectionAmount];
				ARGB backgroundColor[sectionAmount];
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					x[currentMonitor] = _x[currentMonitor];
					y[currentMonitor] = _y[currentMonitor];
					width[currentMonitor] = _width[currentMonitor];
					height[currentMonitor] = _height[currentMonitor];
					border[currentMonitor] = _border[currentMonitor];
				}
				{
					const Window *windowArray[1] = {topLevelWindow};
					int *_intArray[2][monitorAmount];
					int **intArray[2] = {_intArray[0], _intArray[1]};
					unsigned int *_uintArray[3][monitorAmount];
					unsigned int **uintArray[3] = {_uintArray[0], _uintArray[1], _uintArray[2]};
					ARGB *_argbArray[1][2] = {{borderColor, backgroundColor}};
					ARGB **argbArray[1] = {*_argbArray};
					for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
						intArray[0][currentMonitor] = x[currentMonitor];
						intArray[1][currentMonitor] = y[currentMonitor];
						uintArray[0][currentMonitor] = width[currentMonitor];
						uintArray[1][currentMonitor] = height[currentMonitor];
						uintArray[2][currentMonitor] = border[currentMonitor];
					}
					ConfigInfo configInfo = {
						.window = (const Window **)windowArray,
						.integer = intArray,
						.integerDimension0 = 2,
						.integerDimension1 = monitorAmount,
						.integerDimension2 = sectionAmount,
						.unsignedInteger = uintArray,
						.unsignedIntegerDimension0 = 3,
						.unsignedIntegerDimension1 = monitorAmount,
						.unsignedIntegerDimension2 = sectionAmount,
						.argb = argbArray,
						.argbDimension0 = 1,
						.argbDimension1 = 2,
						.argbDimension2 = sectionAmount,
						.character = NULL
					};
					if(!readConfig(SectionWindowsConfigMode, configInfo)){
						mode = ExitMode;
					}
				}
				if(mode == ContinueMode){
					for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
						for(currentSection = 0; currentSection < sectionAmount; ++currentSection){
							if(!createWindow(&section[currentMonitor][currentSection], topLevelWindow[currentMonitor], x[currentMonitor][currentSection], y[currentMonitor][currentSection], width[currentMonitor][currentSection], height[currentMonitor][currentSection], border[currentMonitor][currentSection], borderColor[currentSection], backgroundColor[currentSection])){
								fprintf(stderr, "%s: could not create section %u, %u\n", programName, currentMonitor, currentSection);
							}
						}
					}
					XSync(display, False);
				}else{
					fprintf(stderr, "%s: could not create sections\n", programName);
				}
			}
			{
				int _x[monitorAmount][containerAmount];
				int _y[monitorAmount][containerAmount];
				unsigned int _width[monitorAmount][containerAmount];
				unsigned int _height[monitorAmount][containerAmount];
				unsigned int _border[monitorAmount][containerAmount];
				ARGB borderColor[containerAmount];
				ARGB backgroundColor[containerAmount];
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					x[currentMonitor] = _x[currentMonitor];
					y[currentMonitor] = _y[currentMonitor];
					width[currentMonitor] = _width[currentMonitor];
					height[currentMonitor] = _height[currentMonitor];
					border[currentMonitor] = _border[currentMonitor];
				}
				unsigned int sectionChildrenAmount[sectionAmount];
				{
					unsigned int *_uintArray[1][1] = {{sectionChildrenAmount}};
					unsigned int **uintArray[1] = {*_uintArray};
					ConfigInfo configInfo = {
						.integer = NULL,
						.unsignedInteger = uintArray,
						.unsignedIntegerDimension0 = 1,
						.unsignedIntegerDimension1 = 1,
						.unsignedIntegerDimension2 = sectionAmount,
						.argb = NULL,
						.character = NULL
					};
					if(!readConfig(SectionChildrenConfigMode, configInfo)){
						mode = ExitMode;
					}
				}
				if(mode == ContinueMode){
					{
						int *_intArray[2][monitorAmount];
						int **intArray[2] = {_intArray[0], _intArray[1]};
						unsigned int *_uintArray[3][monitorAmount];
						unsigned int **uintArray[3] = {_uintArray[0], _uintArray[1], _uintArray[2]};
						ARGB *_argbArray[1][2] = {{borderColor, backgroundColor}};
						ARGB **argbArray[1] = {*_argbArray};
						for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
							intArray[0][currentMonitor] = x[currentMonitor];
							intArray[1][currentMonitor] = y[currentMonitor];
							uintArray[0][currentMonitor] = width[currentMonitor];
							uintArray[1][currentMonitor] = height[currentMonitor];
							uintArray[2][currentMonitor] = border[currentMonitor];
						}
						ConfigInfo configInfo = {
							.window = (const Window **)section,
							.integer = intArray,
							.integerDimension0 = 2,
							.integerDimension1 = monitorAmount,
							.integerDimension2 = containerAmount,
							.unsignedInteger = uintArray,
							.unsignedIntegerDimension0 = 3,
							.unsignedIntegerDimension1 = monitorAmount,
							.unsignedIntegerDimension2 = containerAmount,
							.argb = argbArray,
							.argbDimension0 = 1,
							.argbDimension1 = 2,
							.argbDimension2 = containerAmount,
							.character = NULL
						};
						if(!readConfig(ContainerWindowsConfigMode, configInfo)){
							mode = ExitMode;
						}
					}
					if(mode == ContinueMode){
						for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
							currentSection = 0;
							childrenAmount = 0;
							for(currentContainer = 0; currentContainer < containerAmount; ++currentContainer){
								if(!createWindow(&container[currentMonitor][currentContainer], section[currentMonitor][currentSection], x[currentMonitor][currentContainer], y[currentMonitor][currentContainer], width[currentMonitor][currentContainer], height[currentMonitor][currentContainer], border[currentMonitor][currentContainer], borderColor[currentContainer], backgroundColor[currentContainer])){
									fprintf(stderr, "%s: could not create container %u, %u\n", programName, currentMonitor, currentContainer);
								}
								childrenAmount += sectionChildrenAmount[currentSection];
								if(currentContainer == childrenAmount - 1){
									++currentSection;
								}else{
									childrenAmount -= sectionChildrenAmount[currentSection];
								}
							}
						}
						XSync(display, False);
					}else{
						fprintf(stderr, "%s: could not create containers\n", programName);
					}
				}else{
					fprintf(stderr, "%s: could not create containers\n", programName);
				}
			}
		}
		Window rectangle;
		unsigned int containerChildrenAmount[containerAmount];
		{
			unsigned int *_uintArray[1][1] = {{containerChildrenAmount}};
			unsigned int **uintArray[1] = {*_uintArray};
			ConfigInfo configInfo = {
				.integer = NULL,
				.unsignedInteger = uintArray,
				.unsignedIntegerDimension0 = 1,
				.unsignedIntegerDimension1 = 1,
				.unsignedIntegerDimension2 = containerAmount,
				.argb = NULL,
				.character = NULL
			};
			if(!readConfig(ContainerChildrenConfigMode, configInfo)){
				mode = ExitMode;
			}
		}
		if(mode == ContinueMode){
			unsigned int currentRectangle;
			int _x[monitorAmount][rectangleAmount];
			int _y[monitorAmount][rectangleAmount];
			unsigned int _width[monitorAmount][rectangleAmount];
			unsigned int _height[monitorAmount][rectangleAmount];
			unsigned int _border[monitorAmount][rectangleAmount];
			ARGB borderColor[rectangleAmount];
			ARGB backgroundColor[rectangleAmount];
			for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
				x[currentMonitor] = _x[currentMonitor];
				y[currentMonitor] = _y[currentMonitor];
				width[currentMonitor] = _width[currentMonitor];
				height[currentMonitor] = _height[currentMonitor];
				border[currentMonitor] = _border[currentMonitor];
			}
			{
				int *_intArray[2][monitorAmount];
				int **intArray[2] = {_intArray[0], _intArray[1]};
				unsigned int *_uintArray[3][monitorAmount];
				unsigned int **uintArray[3] = {_uintArray[0], _uintArray[1], _uintArray[2]};
				ARGB *_argbArray[1][2] = {{borderColor, backgroundColor}};
				ARGB **argbArray[1] = {*_argbArray};
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					intArray[0][currentMonitor] = x[currentMonitor];
					intArray[1][currentMonitor] = y[currentMonitor];
					uintArray[0][currentMonitor] = width[currentMonitor];
					uintArray[1][currentMonitor] = height[currentMonitor];
					uintArray[2][currentMonitor] = border[currentMonitor];
				}
				ConfigInfo configInfo = {
					.window = (const Window **)container,
					.integer = intArray,
					.integerDimension0 = 2,
					.integerDimension1 = monitorAmount,
					.integerDimension2 = rectangleAmount,
					.unsignedInteger = uintArray,
					.unsignedIntegerDimension0 = 3,
					.unsignedIntegerDimension1 = monitorAmount,
					.unsignedIntegerDimension2 = rectangleAmount,
					.argb = argbArray,
					.argbDimension0 = 1,
					.argbDimension1 = 2,
					.argbDimension2 = rectangleAmount,
					.character = NULL
				};
				if(!readConfig(RectangleWindowsConfigMode, configInfo)){
					mode = ExitMode;
				}
			}
			if(mode == ContinueMode){
				for(currentMonitor = 0; currentMonitor < monitorAmount; ++currentMonitor){
					currentContainer = 0;
					childrenAmount = 0;
					for(currentRectangle = 0; currentRectangle < rectangleAmount; ++currentRectangle){
						if(!createWindow(&rectangle, container[currentMonitor][currentContainer], x[currentMonitor][currentRectangle], y[currentMonitor][currentRectangle], width[currentMonitor][currentRectangle], height[currentMonitor][currentRectangle], border[currentMonitor][currentRectangle], borderColor[currentRectangle], backgroundColor[currentRectangle])){
							fprintf(stderr, "%s: could not create rectangle %u, %u\n", programName, currentMonitor, currentRectangle);
						}
						childrenAmount += containerChildrenAmount[currentContainer];
						if(currentRectangle == childrenAmount - 1){
							++currentContainer;
						}else{
							childrenAmount -= containerChildrenAmount[currentContainer];
						}
					}
				}
				XSync(display, False);
			}else{
				fprintf(stderr, "%s: could not create rectangles %u, n\n", programName, currentMonitor);
			}
		}else{
			fprintf(stderr, "%s: could not create rectangles %u, n\n", programName, currentMonitor);
		}
		value = 1;
	}
	return value;
}
static bool createWindow(Window *const window, const Window parentWindow, const int x, const int y, unsigned int width, unsigned int height, const unsigned int border, const ARGB borderColor, const ARGB backgroundColor){
	bool value = 0;
	if(width && height){
		value = 1;
	}
	if(!width){
		width = 1;
	}
	if(!height){
		height = 1;
	}
	if(!(*window = XCreateSimpleWindow(display, parentWindow, x, y, width, height, border, borderColor, backgroundColor))){
		value = 0;
	}
	if(value){
		XMapWindow(display, *window);
	}
	return value;
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
