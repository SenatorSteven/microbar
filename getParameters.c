#include <stdio.h>
#include "headers/defines.h"

#define NoPositions /*----*/ 0
#define ConfigPosition /*-*/ (1 << 0)
#define HelpPosition /*---*/ (1 << 1)
#define ExitPosition /*---*/ (1 << 2)

static unsigned int isArgument(const char *const argument, const char *const argumentArray);

unsigned int getParameters(const unsigned int *const argumentCount, const char *const *const argumentVector, const char **configPath){
	const unsigned int dereferencedArgumentCount = *argumentCount;
	unsigned int value = 0;
	if(dereferencedArgumentCount > 1){
		unsigned int hasReadVariable = NoPositions;
		for(unsigned int currentArgument = 1; currentArgument < dereferencedArgumentCount; currentArgument++){
			if(!(hasReadVariable & ConfigPosition)){
				if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument])){
					currentArgument++;
					if(argumentVector[currentArgument]){
						if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
							fprintf(stdout, "%s: usage: %s --config \"/path/to/file/\"\n", ProgramName, ProgramName);
							fprintf(stdout, "%s# if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration\n", Tab);
							fprintf(stdout, "%s# the $HOME variable can be used instead of \"/path/to/home/\", case sensitive\n", Tab);
							hasReadVariable |= HelpPosition;
							break;
						}else{
							*configPath = (char *)argumentVector[currentArgument];
							hasReadVariable |= ConfigPosition;
							continue;
						}
					}else{
						fprintf(stderr, "%s: no config value specified\n", ProgramName);
						hasReadVariable |= ExitPosition;
						break;
					}
				}
			}
			if(!(hasReadVariable & HelpPosition)){
				if(isArgument("-h", argumentVector[currentArgument]) || isArgument("--help", argumentVector[currentArgument])){
					fprintf(stdout, "%s: usage: %s [parameters] or %s [parameter] [--help]\n", ProgramName, ProgramName, ProgramName);
					fprintf(stdout, "%s[-h], [--help]  %sdisplay this message\n", Tab, Tab);
					fprintf(stdout, "%s[-c], [--config]%sspecify path to config, necessary\n", Tab, Tab);
					hasReadVariable |= HelpPosition;
					break;
				}
			}
			if(isArgument("-c", argumentVector[currentArgument]) || isArgument("--config", argumentVector[currentArgument])){
				fprintf(stdout, "%s: the config parameter has already been specified\n", ProgramName);
			}else{
				fprintf(stderr, "%s: \"%s\" is not recognized as program parameter, check help? [-h]\n", ProgramName, argumentVector[currentArgument]);
			}
			hasReadVariable |= ExitPosition;
			break;
		}
		if(hasReadVariable & ConfigPosition && !(hasReadVariable & HelpPosition) && !(hasReadVariable & ExitPosition)){
			value = 1;
		}
	}else{
		fprintf(stderr, "%s: no config parameter specified\n", ProgramName);
	}
	return value;
}
static unsigned int isArgument(const char *const argument, const char *const argumentArray){
	unsigned int value = 0;
	unsigned int length = 0;
	while(argument[length] > '\0'){
		length++;
	}
	unsigned int currentCharacter = 0;
	while(currentCharacter < length){
		if(argument[currentCharacter] >= 'A' && argument[currentCharacter] <= 'Z'){
			if(!(argumentArray[currentCharacter] == argument[currentCharacter] || argumentArray[currentCharacter] == argument[currentCharacter] + 32)){
				break;
			}
		}else if(argument[currentCharacter] >= 'a' && argument[currentCharacter] <= 'z'){
			if(!(argumentArray[currentCharacter] == argument[currentCharacter] || argumentArray[currentCharacter] == argument[currentCharacter] - 32)){
				break;
			}
		}else{
			if(!(argumentArray[currentCharacter] == argument[currentCharacter])){
				break;
			}
		}
		currentCharacter++;
	}
	if(currentCharacter == length){
		value = 1;
	}
	return value;
}
