/* getParameters.c

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

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include "headers/defines.h"

#define NoPositions /*-------*/ 0
#define ConfigPosition /*----*/ (1 << 0)
#define WorkplacePosition /*-*/ (1 << 1)
#define HelpPosition /*------*/ (1 << 2)
#define ExitPosition /*------*/ (1 << 3)

extern const char *programName;
extern const char *configPath;
extern const char *workplacePath;
extern FILE *file;

static bool isArgument(const char *const argument, const char *const vector);

bool getParameters(const unsigned int argumentCount, const char *const *const argumentVector){
	bool value = 0;
	programName = argumentVector[0];
	if(argumentCount > 1){
		workplacePath = NULL;
		uint8_t hasReadVariable = NoPositions;
		DIR *dir;
		for(unsigned int currentArgument = 1; currentArgument < argumentCount; ++currentArgument){
			if(!(hasReadVariable & ConfigPosition)){
				if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument])){
					hasReadVariable |= ConfigPosition;
					if(++currentArgument < argumentCount){
						if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
							fprintf(stdout, "%s: usage: %s --config \"/path/to/file\"\n", programName, programName);
							fprintf(stdout, "%s# if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration\n", Tab);
							fprintf(stdout, "%s# environment variables may be used\n", Tab);
							hasReadVariable |= HelpPosition;
							break;
						}else if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument]) || isArgument("-w", argumentVector[currentArgument]) || isArgument("--workplace", argumentVector[currentArgument])){
							fprintf(stderr, "%s: no config value specified\n", programName);
							hasReadVariable |= ExitPosition;
							break;
						}else{
							configPath = argumentVector[currentArgument];
							if(configPath[0] == '/'){
								if(!(dir = opendir(configPath))){
									if((file = fopen(configPath, "r"))){
										fclose(file);
										continue;
									}else{
										if((file = fopen(configPath, "w"))){
											fclose(file);
											remove(configPath);
											continue;
										}else{
											fprintf(stderr, "%s: could not create config file\n", programName);
											hasReadVariable |= ExitPosition;
											break;
										}
									}
								}else{
									closedir(dir);
									fprintf(stderr, "%s: \"%s\" config value is directory\n", programName, configPath);
									hasReadVariable |= ExitPosition;
									break;
								}
							}else{
								fprintf(stderr, "%s: \"%s\" config value is not a path\n", programName, configPath);
								hasReadVariable |= ExitPosition;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no config value specified\n", programName);
						hasReadVariable |= ExitPosition;
						break;
					}
				}
			}
			if(!(hasReadVariable & WorkplacePosition)){
				if(isArgument("-w", argumentVector[currentArgument]) || isArgument("--workplace", argumentVector[currentArgument])){
					hasReadVariable |= WorkplacePosition;
					if(++currentArgument < argumentCount){
						if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
							fprintf(stdout, "%s: usage: %s --workplace \"/path/to/directory/\"\n", programName, programName);
							fprintf(stdout, "%s# if the specified directory doesn't exist, it will not be created\n", Tab);
							fprintf(stdout, "%s# if not specified, workplace directory will be the directory of config\n", Tab);
							fprintf(stdout, "%s# environment variables may be used\n", Tab);
							hasReadVariable |= HelpPosition;
							break;
						}else if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument]) || isArgument("-w", argumentVector[currentArgument]) || isArgument("--workplace", argumentVector[currentArgument])){
							fprintf(stderr, "%s: no workplace value specified\n", programName);
							hasReadVariable |= ExitPosition;
							break;
						}else{
							workplacePath = argumentVector[currentArgument];
							if((dir = opendir(workplacePath))){
								closedir(dir);
								continue;
							}else{
								fprintf(stderr, "%s: \"%s\" workplace value is not valid\n", programName, workplacePath);
								hasReadVariable |= ExitPosition;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no workplace value specified\n", programName);
						hasReadVariable |= ExitPosition;
						break;
					}
				}
			}
			if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
				fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", programName, programName, programName);
				fprintf(stdout, "%s[-h], [--help]     %sdisplay this message\n", Tab, Tab);
				fprintf(stdout, "%s[-c], [--config]   %sspecify path to config, necessary\n", Tab, Tab);
				fprintf(stdout, "%s[-w], [--workplace]%sspecify path to directory used for temporary files, optional\n", Tab, Tab);
				hasReadVariable |= HelpPosition;
				break;
			}
			if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument])){
				fprintf(stderr, "%s: the config parameter has already been specified\n", programName);
			}else if(isArgument("-w", argumentVector[currentArgument]) || isArgument("--workplace", argumentVector[currentArgument])){
				fprintf(stderr, "%s: the workplace parameter has already been specified\n", programName);
			}else{
				fprintf(stderr, "%s: \"%s\" is not recognized as program parameter, check help? [-h]\n", programName, argumentVector[currentArgument]);
			}
			hasReadVariable |= ExitPosition;
			break;
		}
		if(!(hasReadVariable & HelpPosition)){
			if(hasReadVariable & ConfigPosition){
				if(!(hasReadVariable & ExitPosition)){
					value = 1;
				}
			}else{
				fprintf(stderr, "%s: no config parameter specified\n", programName);
			}
		}
	}else{
		fprintf(stderr, "%s: no config parameter specified\n", programName);
	}
	return value;
}
static bool isArgument(const char *const argument, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	while(argument[element] || vector[element]){
		if(vector[element] >= 'A' && vector[element] <= 'Z'){
			if(!(vector[element] == argument[element] || vector[element] == argument[element] + 32)){
				element = 0;
				break;
			}
		}else if(vector[element] >= 'a' && vector[element] <= 'z'){
			if(!(vector[element] == argument[element] || vector[element] == argument[element] - 32)){
				element = 0;
				break;
			}
		}else{
			if(!(vector[element] == argument[element])){
				element = 0;
				break;
			}
		}
		++element;
	}
	if(element != 0){
		value = 1;
	}
	return value;
}
