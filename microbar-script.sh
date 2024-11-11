#

#! use xrandr to get the smallest active monitor size and use the size to calculate the amount of characters possible on screen in a line. width of current font is 6 per char.

#! consider this later
	# wifi="$((($(iw dev $wifiName link | grep signal | cut -d ' ' -f 2)+90)*5/3))%$minorSeparator$wifi"









#!/bin/bash

# hide all errors
	exec 2> /dev/null

#! ansi color
	# $escape=$(echo -e '\x1b')

# separators
	majorSeparator="[90m▕[m "
	minorSeparator=" / "

# globals
	useIcons=false
	upower=$(upower -e)
	root=$(xprop -root)

#! workspaces
	currentWorkspace=$(echo "$root" | grep _NET_CURRENT_DESKTOP\( | cut -d ' ' -f 3)
	workspaces=$(echo "$root" | grep _NET_DESKTOP_NAMES\( | cut -d ' ' -f 3- | awk "{gsub(/[,\"]/, \"\"); for(i=1; i<=NF; ++i) if(i==$((currentWorkspace+1))) printf \"[36m[[37m%s[36m]\", \$i; else printf \" [90m%s \", \$i}")
	if [ "$workspaces" == '' ]; then
		workspaces=[37mnone
	fi

#! active winow
	activeWindow=$(echo "$root" | grep _NET_ACTIVE_WINDOW\( | cut -d ' ' -f 5)
	if [ "$activeWindow" == '0x0' ]; then
		#! get name of desktop/wm instead, via _NET_WM_NAME
		activeWindow=''
	fi
	if [ "$activeWindow" != '' ]; then
		activeWindow=$(xprop -id $activeWindow | grep _NET_WM_NAME\( | cut -d ' ' -f 3- | grep -oP '[\x{0000}-\x{FFFF}]+')
		activeWindow=${activeWindow#\"}
		activeWindow=${activeWindow%\"}
		activeWindow=$majorSeparator[37m$activeWindow
		temp=$(echo "$activeWindow" | cut -c1-100)
		if (( ${#temp} < ${#activeWindow} )); then
			activeWindow="$temp..."
		fi
	fi

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



	color='[31m'
	wifi=$color'Wi-Fi: none'
	if [ "$wifiName" != '' ]; then
		wifiIP=$(echo $addresses | grep -oP "$wifiName.*inet \K[^ ]+")
		wifiStatus=$(echo "$links" | grep -oP "$wifiName.*state \K[^ ]+")
		wifi=${wifiStatus,,}
		if [ "$wifiIP" != '' ]; then
			color='[32m'
			wifi="$wifi$minorSeparator$wifiIP"
		elif [ "$wifi" == 'up' ]; then
			color='[32m'
		elif [ "$wifi" == 'dormant' ]; then
			color='[33m'
		else
			color='[31m'
		fi
		wifi=$color"Wi-Fi: $wifi"
	fi
	color='[31m'
	ethernet=$color'Ethernet: none'
	if [ "$ethernetName" != '' ]; then
		ethernetIP=$(echo $addresses | grep -oP "$ethernetName.*inet \K[^ ]+")
		ethernetStatus=$(echo "$links" | grep -oP "$ethernetName.*state \K[^ ]+")
		ethernet=${ethernetStatus,,}
		if [ "$ethernetIP" != '' ]; then
			ethernet="[32mEthernet: $ethernet$minorSeparator$ethernetIP"
		elif [ "$ethernet" == 'up' ]; then
			ethernet="[32mEthernet: $ethernet"
		else
			ethernet="[31mEthernet: $ethernet"
		fi
	fi

# bluetooth
	bluetooth=$(upower -i $(echo "$upower" | grep headphones | head -n 1) | grep percentage | awk '{print $2}')
	if [ "$bluetooth" == '' ]; then
		bluetooth="[31mBluetooth: none"
	else
		color='[32m'
		bluetooth=${bluetooth%\%}
		if (( bluetooth < 50 )); then
			color='[33m'
		elif (( bluetooth <= 10 )); then
			color='[31m'
		fi
		bluetooth=$color"Bluetooth: $bluetooth%[m"
	fi

#! volume
	# add colors

	volume=$(pactl get-sink-volume @DEFAULT_SINK@)
	mute=$(pactl get-sink-mute @DEFAULT_SINK@ | cut -d ' ' -f 2)
	whichVolumes='5'
	if [ "$(echo $volume | cut -d ' ' -f 12)" != '' ]; then
		whichVolumes='5,12'
	fi
	volume=$(echo $volume | cut -d ' ' -f $whichVolumes --output-delimiter="$minorSeparator")
	if [ "$mute" != no ]; then
		volume="$volume (muted)"
	fi
	volume="[37mVolume: $volume"

#! load
	load=$(awk "BEGIN {printf \"%.2f\", $(cat /proc/loadavg | cut -d ' ' -f 1)/$(grep -c ^processor /proc/cpuinfo)}")
	load=$((10#${load//./}))
	color='[37m'
	if (( load > 50 )); then
		color='[33m'
	elif (( load > 75 )); then
		color='[31m'
	fi
	load=$color"CPU: $load%"

#! battery
	color='[31m'
	battery=$(upower -i $(echo "$upower" | grep battery | head -n 1) | awk '/present|state|time to full|time to empty|percentage/')
	icon=' '
	present=$(echo "$battery" | grep present | awk '{print $2}')
	if [ "$present" == no ]; then
		battery=$color'Battery: none'
	else
		state=$(echo "$battery" | grep state | awk "{gsub(/-/, \" \", \$2); print \$2}")
		# timeToFull=$(echo "$battery" | grep 'time to full' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		# timeToEmpty=$(echo "$battery" | grep 'time to empty' | awk '{for (i=4; i<NF; ++i) printf $i " "; printf $i}')
		percentage=$(echo "$battery" | grep percentage | awk '{print $2}')
		percentage=${percentage%\%}
		if (( percentage >= 50 )); then
			color='[32m'
		elif (( percentage > 10 )); then
			color='[33m'
		fi
		percentage="$percentage"0
		if [ "$useIcons" == true ]; then
			icons=(╳ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █)
			icon=▕${icons[$(((percentage + 124) / 125))]}▏
		fi
		battery=${percentage%0}%
		# battery=${percentage%0}%$minorSeparator$state
		# if [ "$timeToFull" != '' ]; then
		# 	battery="$battery$minorSeparator$timeToFull"
		# elif [ "$timeToEmpty" != '' ]; then
		# 	battery="$battery$minorSeparator$timeToEmpty"
		# fi
		battery=$color"Battery:$icon$battery"
	fi

# date and time
	case $(date +%-d) in 1|21|31)o=st;;2|22)o=nd;;3|23)o=rd;;*)o=th;;esac
	dateTime=[37m$(date "+%A, %-d$o of %B %Y, %H:%M:%S")

# result 1
	island1=$(printf "%s%s" "$workspaces" "$activeWindow")
	island2=$(printf " %s " "$dateTime")
	island3=$(printf "%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s" "$wifi" "$ethernet" "$bluetooth" "$volume" "$load" "$battery")
	island1Width=$(echo "$island1" | sed 's/\x1b\[[0-9;]*m//g' | wc -m)
	island2Width=$(echo "$island2" | sed 's/\x1b\[[0-9;]*m//g' | wc -m)
	island3Width=$(echo "$island3" | sed 's/\x1b\[[0-9;]*m//g' | wc -m)
	barCharacterWidth=322
	spacing1Width=$((($barCharacterWidth - $island2Width) / 2 - $island1Width))
	spacing2Width=$(($barCharacterWidth - $island3Width - $island1Width - $island2Width - $spacing1Width))
	extraWidth=$(($barCharacterWidth - $island1Width - $spacing1Width - $island2Width - $spacing2Width - $island3Width))
	if (( spacing1Width < 0 )); then spacing1Width=0; fi
	if (( spacing2Width < 0 )); then spacing2Width=0; fi
	if ((    extraWidth < 0 )); then    extraWidth=0; fi
	printf "%s%*s%s%*s%s" "$island1" $(($spacing1Width - $extraWidth)) ' ' "$island2" $spacing2Width ' ' "$island3"

# result 2
	# printf "%s [%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s]" \
	#	"$workspaces" "$wifi" "$ethernet" "$bluetooth" "$volume" "$load" "$battery" "$dateTime"

exit 0
