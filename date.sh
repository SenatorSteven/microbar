#!/bin/bash

suffix="th"

while true; do
	day=$(date '+%d')
	[ "$day" =  "1" ] && { suffix="st"; break; } || :
	[ "$day" =  "2" ] && { suffix="nd"; break; } || :
	[ "$day" =  "3" ] && { suffix="rd"; break; } || :
	[ "$day" = "21" ] && { suffix="st"; break; } || :
	[ "$day" = "22" ] && { suffix="nd"; break; } || :
	[ "$day" = "23" ] && { suffix="rd"; break; } || :
	[ "$day" = "31" ] && { suffix="st"; break; } || :
	break
done
while true; do
	clear
	date "+%A, %d$suffix of %B %Y / %H:%M:%S"
	sleep 0.1
done
exit 0
