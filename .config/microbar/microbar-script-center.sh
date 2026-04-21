
#!/bin/bash

# hide all errors
	exec 2> /dev/null

# ansi color
	ESC=$(printf '\e')

# date and time
	color="$ESC[37m"
	case $(date +%-d) in 1|21|31)o=st;;2|22)o=nd;;3|23)o=rd;;*)o=th;;esac
	dateTime=$color$(date "+%A, %-d$o of %B %Y, %H:%M:%S")

# result
	printf " %s " "$dateTime"

exit 0

