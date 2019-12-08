#include <stdio.h>

#define NoPositions /*----*/ 0
#define ConfigPosition /*-*/ (1 << 0)
#define HelpPosition /*---*/ (1 << 1)
#define ExitPosition /*---*/ (1 << 2)

static unsigned int isArgumentHelp(const char *const argumentArray);
static unsigned int isArgumentConfig(const char *const argumentArray);

unsigned int getParameters(const int *const argumentCount, const char *const *const argumentVector, const char **configPath){
	const int dereferencedArgumentCount = *argumentCount;
	unsigned int value = 0;
	if(dereferencedArgumentCount > 1){
		unsigned int hasReadVariable = NoPositions;
		for(int currentArgument = 1; currentArgument < dereferencedArgumentCount; currentArgument++){
			if(!(hasReadVariable & ConfigPosition)){
				if(isArgumentConfig(argumentVector[currentArgument])){
					if(argumentVector[currentArgument + 1]){
						currentArgument++;
						if(isArgumentHelp(argumentVector[currentArgument])){
							hasReadVariable |= ExitPosition;
							fprintf(stdout, "microbar: usage: microbar --config \"/path/to/file/\"\n");
							fprintf(stdout, "   # if the specified file doesn't exist, it will be created and it will contain the hardcoded default configuration\n");
							fprintf(stdout, "   # the $HOME variable can be used instead of \"/path/to/home/\", case sensitive\n");
							break;
						}else{
							*configPath = (char *)argumentVector[currentArgument];
							hasReadVariable |= ConfigPosition;
							continue;
						}
					}else{
						fprintf(stderr, "microbar: no config value specified\n");
						hasReadVariable |= ExitPosition;
						break;
					}
				}
			}
			if(!(hasReadVariable & HelpPosition)){
				if(isArgumentHelp(argumentVector[currentArgument])){
					fprintf(stdout, "microbar: usage: microbar [parameters] or microbar [parameter] [--help]\n");
					fprintf(stdout, "   [-h], [--help]     display this message\n");
					fprintf(stdout, "   [-c], [--config]   specify path to config, necessary\n");
					hasReadVariable |= HelpPosition;
					break;
				}
			}
			fprintf(stderr, "microbar: \"%s\" is not recognized as program parameter, check help? [-h]\n", argumentVector[currentArgument]);
			hasReadVariable |= ExitPosition;
			break;
		}
		if(hasReadVariable & ConfigPosition && !(hasReadVariable & HelpPosition) && !(hasReadVariable & ExitPosition)){
			value = 1;
		}
	}else{
		fprintf(stderr, "microbar: no config parameter specified\n");
	}
	return value;
}
static unsigned int isArgumentHelp(const char *const argumentArray){
	return (
		(
			argumentArray[0] == '-' &&
			(argumentArray[1] == 'H' || argumentArray[1] == 'h') &&
			argumentArray[2] == '\0'
		) || (
			argumentArray[0] == '-' &&
			argumentArray[1] == '-' &&
			(argumentArray[2] == 'H' || argumentArray[2] == 'h') &&
			(argumentArray[3] == 'E' || argumentArray[3] == 'e') &&
			(argumentArray[4] == 'L' || argumentArray[4] == 'l') &&
			(argumentArray[5] == 'P' || argumentArray[5] == 'p') &&
			argumentArray[6] == '\0'
		)
	);
}
static unsigned int isArgumentConfig(const char *const argumentArray){
	return (
		(
			argumentArray[0] == '-' &&
			(argumentArray[1] == 'C' || argumentArray[1] == 'c') &&
			argumentArray[2] == '\0'
		) || (
			argumentArray[0] == '-' &&
			argumentArray[1] == '-' &&
			(argumentArray[2] == 'C' || argumentArray[2] == 'c') &&
			(argumentArray[3] == 'O' || argumentArray[3] == 'o') &&
			(argumentArray[4] == 'N' || argumentArray[4] == 'n') &&
			(argumentArray[5] == 'F' || argumentArray[5] == 'f') &&
			(argumentArray[6] == 'I' || argumentArray[6] == 'i') &&
			(argumentArray[7] == 'G' || argumentArray[7] == 'g') &&
			argumentArray[8] == '\0'
		)
	);
}
