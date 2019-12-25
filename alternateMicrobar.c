/*int main(const int argumentCount, const char *const *const argumentVector){
	const char *configPath;
	if(getParameters((unsigned int *)&argumentCount, argumentVector, &configPath)){
		unsigned int mode = ModeContinue;
		Display *display;
		unsigned int monitorAmount;
		while(mode == ModeContinue || mode == ModeRestart){
			if(mode == ModeRestart){
				mode = ModeContinue;
			}
			if((display = XOpenDisplay(NULL))){



				unsigned int menuAmount;
				unsigned int boxAmount;
				unsigned int innerBoxAmount;
				unsigned int currentMonitor = 0;
				Window rootWindow = XDefaultRootWindow(display);
				readConfigScan(display, currentMonitor, configPath, rootWindow, &menuAmount, &boxAmount, &innerBoxAmount);
				Window menu[menuAmount];
				Window box[boxAmount];
				Window innerBox[innerBoxAmount];



				XRRGetMonitors(display, rootWindow, True, (int *)&monitorAmount);
				Window window[monitorAmount];
				if(createWindows(display, configPath, window, &monitorAmount)){
					setTopLevelWindowProperties(display, window, &monitorAmount);
					eventLoop(display, configPath, window, &monitorAmount, &mode);
					cleanupWindows(display, window, &monitorAmount);
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
}*/
