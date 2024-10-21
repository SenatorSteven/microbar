#!/bin/bash

# separators
	majorSeparator=' | '
	minorSeparator=' / '

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
			wifi="$wifiStatus / $wifiIP"
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
			ethernet="$ethernetStatus / $ethernetIP"
		else
			ethernet=$ethernetStatus
		fi
	fi

# load
	load=$(cat /proc/loadavg | awk '{print $1}')

# battery
	battery=$(upower -i $(upower -e | grep 'battery' | head -n 1) | awk '/present|state|percentage/ {print $2}')
	if [ $(echo $battery | awk '{print $1}') == no ]; then
		battery=none
	else
		battery=$(echo $battery | awk "{gsub(/-/, \" \", \$2); print \$3 \"$minorSeparator\" \$2}")
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

	printf "%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s\n" \
		"Wi-Fi: $wifi" \
		"Ethernet: $ethernet" \
		"Load: $load" \
		"Battery: $battery" \
		"$dateTime"

exit 0
