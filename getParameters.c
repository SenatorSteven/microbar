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

#define NoParameters /*-------*/ 0
#define ConfigParameter /*----*/ (1 << 0)
#define WorkplaceParameter /*-*/ (1 << 1)
#define HelpParameter /*------*/ (1 << 2)
#define ExitParameter /*------*/ (1 << 3)

extern const char *programName;
extern const char *configPath;
extern const char *workplacePath;

typedef uint8_t ParameterList;

static bool isParameter(const char *const parameter, const char *const vector);

bool getParameters(const unsigned int parameterCount, const char *const *const parameterVector){
	bool value = 0;
	programName = parameterVector[0];
	if(parameterCount > 1){
		workplacePath = NULL;
		const char *currentParameterVector;
		ParameterList hasReadParameter = NoParameters;
		DIR *dir;
		FILE *file;
		for(unsigned int currentParameter = 1; currentParameter < parameterCount; ++currentParameter){
			currentParameterVector = parameterVector[currentParameter];
			if(!(hasReadParameter & ConfigParameter)){
				if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector)){
					hasReadParameter |= ConfigParameter;
					if(++currentParameter < parameterCount){
						currentParameterVector = parameterVector[currentParameter];
						if(isParameter("-h", currentParameterVector) || isParameter("--help", currentParameterVector)){
							fprintf(stdout, "%s: usage: %s --config \"/path/to/file\"\n", programName, programName);
							fprintf(stdout, "%sif the specified file doesn't exist, it will be created\n%sand it will contain the hardcoded default configuration\n", Tab, Tab);
							hasReadParameter |= HelpParameter;
							break;
						}else if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector) || isParameter("-w", currentParameterVector) || isParameter("--workplace", currentParameterVector)){
							fprintf(stderr, "%s: no config value specified\n", programName);
							hasReadParameter |= ExitParameter;
							break;
						}else{
							configPath = currentParameterVector;
							if((dir = opendir(configPath))){
								closedir(dir);
								fprintf(stderr, "%s: \"%s\" config value is directory\n", programName, configPath);
								hasReadParameter |= ExitParameter;
								break;
							}else if((file = fopen(configPath, "r"))){
								fclose(file);
								continue;
							}else if((file = fopen(configPath, "w"))){
								fclose(file);
								remove(configPath);
								continue;
							}else{
								fprintf(stderr, "%s: could not create config file\n", programName);
								hasReadParameter |= ExitParameter;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no config value specified\n", programName);
						hasReadParameter |= ExitParameter;
						break;
					}
				}
			}
			if(!(hasReadParameter & WorkplaceParameter)){
				if(isParameter("-w", currentParameterVector) || isParameter("--workplace", currentParameterVector)){
					hasReadParameter |= WorkplaceParameter;
					if(++currentParameter < parameterCount){
						currentParameterVector = parameterVector[currentParameter];
						if(isParameter("-h", currentParameterVector) || isParameter("--help", currentParameterVector)){
							fprintf(stdout, "%s: usage: %s --workplace \"/path/to/directory/\"\n", programName, programName);
							fprintf(stdout, "%sif the specified directory doesn't exist, it will not be created\n", Tab);
							fprintf(stdout, "%sif not specified, workplace directory will be the directory of config\n", Tab);
							hasReadParameter |= HelpParameter;
							break;
						}else if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector) || isParameter("-w", currentParameterVector) || isParameter("--workplace", currentParameterVector)){
							fprintf(stderr, "%s: no workplace value specified\n", programName);
							hasReadParameter |= ExitParameter;
							break;
						}else{
							workplacePath = currentParameterVector;
							if((dir = opendir(workplacePath))){
								closedir(dir);
								continue;
							}else if((file = fopen(workplacePath, "r"))){
								fclose(file);
								fprintf(stderr, "%s: \"%s\" workplace value is file\n", programName, workplacePath);
								hasReadParameter |= ExitParameter;
								break;
							}else{
								fprintf(stderr, "%s: \"%s\" workplace value is not valid\n", programName, workplacePath);
								hasReadParameter |= ExitParameter;
								break;
							}
						}
					}else{
						fprintf(stderr, "%s: no workplace value specified\n", programName);
						hasReadParameter |= ExitParameter;
						break;
					}
				}
			}
			if(isParameter("-h", currentParameterVector) || isParameter("--help", currentParameterVector)){
				fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", programName, programName, programName);
				fprintf(stdout, "%s[-h], [--help]     %sdisplay this message\n", Tab, Tab);
				fprintf(stdout, "%s[-c], [--config]   %sspecify path to config, necessary\n", Tab, Tab);
				fprintf(stdout, "%s[-w], [--workplace]%sspecify path to directory used for temporary files, optional\n", Tab, Tab);
				hasReadParameter |= HelpParameter;
				break;
			}else if(isParameter("-c", currentParameterVector) || isParameter("--config", currentParameterVector)){
				fprintf(stderr, "%s: the config parameter has already been specified\n", programName);
			}else if(isParameter("-w", currentParameterVector) || isParameter("--workplace", currentParameterVector)){
				fprintf(stderr, "%s: the workplace parameter has already been specified\n", programName);
			}
			fprintf(stderr, "%s: \"%s\" is not recognized as program parameter, check help? [-h]\n", programName, currentParameterVector);
			hasReadParameter |= ExitParameter;
			break;
		}
		if(!(hasReadParameter & HelpParameter)){
			if(hasReadParameter & ConfigParameter){
				if(!(hasReadParameter & ExitParameter)){
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
static bool isParameter(const char *const parameter, const char *const vector){
	bool value = 0;
	unsigned int element = 0;
	char v = *vector;
	char p = *parameter;
	while(v || p){
		if(v >= 'A' && v <= 'Z'){
			if(v != p && v != p - 32){
				element = 0;
				break;
			}
		}else if(v >= 'a' && v <= 'z'){
			if(v != p && v != p + 32){
				element = 0;
				break;
			}
		}else if(v != p){
			element = 0;
			break;
		}
		++element;
		v = vector[element];
		p = parameter[element];
	}
	if(element){
		value = 1;
	}
	return value;
}
