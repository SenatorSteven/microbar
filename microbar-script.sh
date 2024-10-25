
# if the minor separator is used in awk with some \"\" trick it will not work as intented all the time. prime example: the \.









#!/bin/bash

# separators
	majorSeparator=" | "
	minorSeparator=" / "

# workspaces
	root=$(xprop -root)
	currentWorkspace=$(($(echo "$root" | grep _NET_CURRENT_DESKTOP\(CARDINAL\) | cut -d ' ' -f 3)+1))
	workspaces=$(echo "$root" | grep _NET_DESKTOP_NAMES\(UTF8_STRING\) | cut -d ' ' -f3- | awk "{gsub(/[,\"]/, \"\"); for(i=1; i<=NF; ++i) if(i==$currentWorkspace) printf \"[%s]\", \$i; else printf \" %s \", \$i}")
	if [ "$workspaces" == '' ]; then
		workspaces=none
	fi

# wifi and ethernet
	wifiName=''
	ethernetName=''
	addresses=$(ip -o -f inet addr)
	links=$(ip -o link)
	interfaces=$(echo "$links" | awk '{print "/sys/class/net/" substr($2, 1, length($2)-1) "/type"}')
	tnterfaceAmount=$(echo "$interfaces" | wc -w)
	for ((i = 1; i <= $tnterfaceAmount; ++i)); do
		location=$(echo $interfaces | awk "{print \$$i}")
		if [ $(cat $location) == 1 ]; then
			status=$(echo "$links" | awk "NR==$i {print substr(\$2, 1, length(\$2)-1)}")
			if [ -d ${location:0:-4}wireless ]; then
				if [ "$wifiName" == '' ]; then
					wifiName=$status
					if [ "$ethernetName" != '' ]; then
						break
					fi
				fi
			else
				if [ "$ethernetName" == '' ]; then
					ethernetName=$status
					if [ "$wifiName" != '' ]; then
						break
					fi
				fi
			fi
		fi
	done
	wifi=none
	if [ "$wifiName" != '' ]; then
		wifiIP=$(echo $addresses | grep -oP "$wifiName.*inet \K[^ ]+")
		wifiStatus=$(echo "$links" | grep -oP "$wifiName.*state \K[^ ]+")
		wifiStatus=${wifiStatus,,}
		if [ "$wifiIP" != '' ]; then
			wifi="$wifiStatus$minorSeparator$wifiIP"
		else
			wifi=$wifiStatus
		fi
	fi
	ethernet=none
	if [ "$ethernetName" != '' ]; then
		ethernetIP=$(echo $addresses | grep -oP "$ethernetName.*inet \K[^ ]+")
		ethernetStatus=$(echo "$links" | grep -oP "$ethernetName.*state \K[^ ]+")
		ethernetStatus=${ethernetStatus,,}
		if [ "$ethernetIP" != '' ]; then
			ethernet="$ethernetStatus$minorSeparator$ethernetIP"
		else
			ethernet=$ethernetStatus
		fi
	fi

# bluetooth
	bluetooth=$(upower -i $(upower -e | grep headphones | head -n 1) | grep percentage | awk '{print $2}')
	if [ "$bluetooth" == '' ]; then
		bluetooth=none
	fi

# sound
	sound=$(echo $(pactl get-sink-volume $(pactl get-default-sink)) | awk "{print \$5 \"$minorSeparator\" \$12}")

# load
	load=$(cat /proc/loadavg | awk '{print $1}')

# battery
	battery=$(upower -i $(upower -e | grep battery | head -n 1) | awk '/present|state|time to full|time to empty|percentage/')
	present=$(echo "$battery" | grep present | awk '{print $2}')
	if [ "$present" == no ]; then
		battery=none
	else
		state=$(echo "$battery" | grep state | awk "{gsub(/-/, \" \", \$2); print \$2}")
		timeToFull=$(echo "$battery" | grep 'time to full' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		timeToEmpty=$(echo "$battery" | grep 'time to empty' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		percentage=$(echo "$battery" | grep percentage | awk '{print $2}')
		battery=$percentage$minorSeparator$state
		if [ "$timeToFull" != '' ]; then
			battery="$battery$minorSeparator$timeToFull"
		elif [ "$timeToEmpty" != '' ]; then
			battery="$battery$minorSeparator$timeToEmpty"
		fi
	fi

# date and time
	case $(date +%d) in
		1 | 21 | 31) dayOrdinal=st;;
		2 | 22     ) dayOrdinal=nd;;
		3 | 23     ) dayOrdinal=rd;;
		*          ) dayOrdinal=th;;
	esac
	dateTime=$(date "+%A, %d$dayOrdinal of %B %Y$majorSeparator%H:%M:%S")

# result
	printf "%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s" \
		"$workspaces" \
		"Wi-Fi: $wifi" \
		"Ethernet: $ethernet" \
		"Bluetooth: $bluetooth" \
		"Sound: $sound" \
		"Load: $load" \
		"Battery: $battery" \
		"$dateTime"

exit 0
