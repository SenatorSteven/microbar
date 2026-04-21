
#! consider this later
	# wifi="$((($(iw dev $wifiName link | grep signal | cut -d ' ' -f 2)+90)*5/3))%$minorSeparator$wifi"









#!/bin/bash

# hide all errors
	exec 2> /dev/null

# ansi color
	ESC=$(printf '\e')

# separators
	majorSeparator="$ESC[90m▕$ESC[m "
	minorSeparator=" / "

# globals
	upower=$(upower -e)

#! wifi and ethernet
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
				wifiName=$status
			else
				ethernetName=$status
			fi
		fi
	done
	color="$ESC[31m"
	wifi=$color'Wi-Fi: none'
	if [ "$wifiName" != '' ]; then
		wifiIP=$(echo $addresses | grep -oP "$wifiName.*inet \K[^ ]+")
		wifiStatus=$(echo "$links" | grep -oP "$wifiName.*state \K[^ ]+")
		wifi=${wifiStatus,,}
		if [ "$wifiIP" != '' ]; then
			color="$ESC[32m"
			wifi="$wifiIP"
		elif [ "$wifi" == 'up' ]; then
			color="$ESC[32m"
		elif [ "$wifi" == 'dormant' ]; then
			color="$ESC[33m"
		fi
		wifi=$color"Wi-Fi: $wifi"
	fi
	color="$ESC[31m"
	ethernet=$color'Ethernet: none'
	if [ "$ethernetName" != '' ]; then
		ethernetIP=$(echo $addresses | grep -oP "$ethernetName.*inet \K[^ ]+")
		ethernetStatus=$(echo "$links" | grep -oP "$ethernetName.*state \K[^ ]+")
		ethernet=${ethernetStatus,,}
		if [ "$ethernetIP" != '' ]; then
			color="$ESC[32m"
			ethernet="$ethernetIP"
		elif [ "$ethernet" == 'up' ]; then
			color="$ESC[32m"
			ethernet="$ethernet"
		else
			ethernet="$ethernet"
		fi
		ethernet=$color"Ethernet: $ethernet"
	fi

# bluetooth
	bluetooth=$(upower -i $(echo "$upower" | grep headphones | head -n 1) | grep percentage | awk '{print $2}')
	if [ "$bluetooth" == '' ]; then
		bluetooth="$ESC[31mBluetooth: none"
	else
		color="$ESC[32m"
		bluetooth=${bluetooth%\%}
		if (( bluetooth < 50 )); then
			color="$ESC[33m"
		fi
		bluetooth=$color"Bluetooth: $bluetooth%$ESC[m"
	fi

# volume
	volume=$(pactl get-sink-volume @DEFAULT_SINK@)
	mute=$(pactl get-sink-mute @DEFAULT_SINK@ | cut -d ' ' -f 2)
	volume1=$(echo $volume | cut -d ' ' -f 5)
	volume2=$(echo $volume | cut -d ' ' -f 12)
	if [ "$volume2" != '' ]; then
		volume=$volume1$minorSeparator$volume2
		averageVolume=$(((${volume1%\%} + ${volume2%\%}) / 2))
	else
		volume=$volume1
		averageVolume=${volume1%\%}
	fi
	color="$ESC[37m"
	if (( averageVolume > 75 )); then
		color="$ESC[31m"
	fi
	if [ "$mute" != no ]; then
		volume="$volume (muted)"
	fi
	volume=$color"Volume: $volume"

# load
	load=$(awk "BEGIN {printf \"%.2f\", $(cat /proc/loadavg | cut -d ' ' -f 1)/$(grep -c ^processor /proc/cpuinfo)}")
	load=$((10#${load//./}))
	color="$ESC[37m"
	if (( load > 100 )); then
		color="$ESC[31m"
	elif (( load > 50 )); then
		color="$ESC[33m"
	fi
	load=$color"CPU: $load%"

# battery
	color="$ESC[31m"
	battery=$(upower -i $(echo "$upower" | grep battery | head -n 1) | awk '/present|state|time to full|time to empty|percentage/')
	present=$(echo "$battery" | grep present | awk '{print $2}')
	if [ "$present" == no ]; then
		battery=$color'Battery: none'
	else
		icon=' '
		# state=$(echo "$battery" | grep state | awk "{gsub(/-/, \" \", \$2); print \$2}")
		timeToFull=$(echo "$battery" | grep 'time to full' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		timeToEmpty=$(echo "$battery" | grep 'time to empty' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		percentage=$(echo "$battery" | grep percentage | awk '{print $2}')
		percentage=${percentage%\%}
		if (( percentage > 50 )); then
			color="$ESC[37m"
		elif (( percentage > 10 )); then
			color="$ESC[33m"
		fi
		percentage="$percentage"0
		if [ "$useIcons" == true ]; then
			icons=(╳ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █)
			icon=▕${icons[$(((percentage + 124) / 125))]}▏
		fi
		battery=${percentage%0}%
		if [ "$timeToFull" != '' ]; then
			battery="$battery$minorSeparator$timeToFull"
		elif [ "$timeToEmpty" != '' ]; then
			battery="$battery$minorSeparator$timeToEmpty"
		fi
		battery=$color"Battery:$icon$battery"
	fi

# result
	printf "%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s" "$wifi" "$ethernet" "$bluetooth" "$volume" "$load" "$battery"

exit 0

