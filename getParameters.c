#include <stdio.h>
#include <stdlib.h>
#include "headers/defines.h"

#define NoPositions /*-------*/ 0
#define ConfigPosition /*----*/ (1 << 0)
#define WorkplacePosition /*-*/ (1 << 1)
#define HelpPosition /*------*/ (1 << 2)
#define ExitPosition /*------*/ (1 << 3)

extern const char *restrict programName;
extern const char *restrict configPath;
extern const char *restrict workplacePath;

static bool isArgument(const char *const restrict argument, const char *const restrict argumentArray);

bool getParameters(const unsigned int *const restrict argumentCount, const char *const restrict *const restrict argumentVector){
	const unsigned int dereferencedArgumentCount = *argumentCount;
	bool value = 0;
	programName = argumentVector[0];
	if(dereferencedArgumentCount > 1){
		unsigned int hasReadVariable = NoPositions;
		for(unsigned int currentArgument = 1; currentArgument < dereferencedArgumentCount; ++currentArgument){
			if(!(hasReadVariable & ConfigPosition)){
				if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument])){
					hasReadVariable |= ConfigPosition;
					++currentArgument;
					if(argumentVector[currentArgument]){
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
							configPath = (char *)argumentVector[currentArgument];
							continue;
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
					++currentArgument;
					if(argumentVector[currentArgument]){
						if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
							fprintf(stdout, "%s: usage: %s --workplace \"/path/to/directory\"\n", programName, programName);
							fprintf(stdout, "%s# if the specified directory doesn't exist, it will not be created\n", Tab);
							fprintf(stdout, "%s# if not specified, workspace directory will be the directory of config\n", Tab);
							fprintf(stdout, "%s# environment variables may be used\n", Tab);
							hasReadVariable |= HelpPosition;
							break;
						}else if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument]) || isArgument("-w", argumentVector[currentArgument]) || isArgument("--workplace", argumentVector[currentArgument])){
							fprintf(stderr, "%s: no workplace value specified\n", programName);
							hasReadVariable |= ExitPosition;
							break;
						}else{
							workplacePath = (char *)argumentVector[currentArgument];
							if((workplacePath = realpath(workplacePath, NULL))){
								continue;
							}else{
								fprintf(stderr, "%s: workplace value is not valid\n", programName);
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
			if(!(hasReadVariable & HelpPosition)){
				if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
					fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", programName, programName, programName);
					fprintf(stdout, "%s[-h], [--help]     %sdisplay this message\n", Tab, Tab);
					fprintf(stdout, "%s[-c], [--config]   %sspecify path to config, necessary\n", Tab, Tab);
					fprintf(stdout, "%s[-w], [--workplace]%sspecify path to directory used for temporary files, optional\n", Tab, Tab);
					hasReadVariable |= HelpPosition;
					break;
				}
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
static bool isArgument(const char *const restrict argument, const char *const restrict argumentArray){
	bool value = 0;
	unsigned int element = 0;
	while(argument[element] != '\0'){
		if(argument[element] >= 'A' && argument[element] <= 'Z'){
			if(!(argumentArray[element] == argument[element] || argumentArray[element] == argument[element] + 32)){
				element = 0;
				break;
			}
		}else if(argument[element] >= 'a' && argument[element] <= 'z'){
			if(!(argumentArray[element] == argument[element] || argumentArray[element] == argument[element] - 32)){
				element = 0;
				break;
			}
		}else{
			if(!(argumentArray[element] == argument[element])){
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
