
#!/bin/bash

# separators
	majorSeparator="  "
	minorSeparator=" / "

# globals
	upower=$(upower -e)

# workspaces
	r=$(xprop -root)
	currentWorkspace=$(echo "$r" | grep _NET_CURRENT_DESKTOP\(CARDINAL\) | cut -d ' ' -f 3)
	workspaces=$(echo "$r" | grep _NET_DESKTOP_NAMES\(UTF8_STRING\) | cut -d ' ' -f 3- | awk "{gsub(/[,\"]/, \"\"); for(i=1; i<=NF; ++i) if(i==$((currentWorkspace+1))) printf \"[%s]\", \$i; else printf \" %s \", \$i}")
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
	ethernet=none
	if [ "$wifiName" != '' ]; then
		wifiIP=$(echo $addresses | grep -oP "$wifiName.*inet \K[^ ]+")
		wifiStatus=$(echo "$links" | grep -oP "$wifiName.*state \K[^ ]+")
		wifi=${wifiStatus,,}
		if [ "$wifiIP" != '' ]; then
			wifi="$wifi$minorSeparator$wifiIP"
		fi
	fi
	if [ "$ethernetName" != '' ]; then
		ethernetIP=$(echo $addresses | grep -oP "$ethernetName.*inet \K[^ ]+")
		ethernetStatus=$(echo "$links" | grep -oP "$ethernetName.*state \K[^ ]+")
		ethernet=${ethernetStatus,,}
		if [ "$ethernetIP" != '' ]; then
			ethernet="$ethernet$minorSeparator$ethernetIP"
		fi
	fi

# bluetooth
	bluetooth=$(upower -i $(echo "$upower" | grep headphones | head -n 1) | grep percentage | awk '{print $2}')
	if [ "$bluetooth" == '' ]; then
		bluetooth=none
	fi

# input
	input=$(pactl get-source-volume @DEFAULT_SOURCE@)
	mute=$(pactl get-source-mute @DEFAULT_SOURCE@ | cut -d ' ' -f 2)
	whichInputs='5'
	if [ "$(echo $input | cut -d ' ' -f 12)" != '' ]; then
		whichInputs='5,12'
	fi
	input=$(echo $input | cut -d ' ' -f $whichInputs --output-delimiter="$minorSeparator")
	if [ "$mute" != no ]; then
		input="$input (muted)"
	fi

# volume
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

# load
	load=$(cat /proc/loadavg | cut -d ' ' -f 1)

# battery
	battery=$(upower -i $(echo "$upower" | grep battery | head -n 1) | awk '/present|state|time to full|time to empty|percentage/')
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
	case $(date +%-d) in 1|21|31)o=st;;2|22)o=nd;;3|23)o=rd;;*)o=th;;esac
	dateTime=$(date "+%A, %-d$o of %B %Y, %H:%M:%S")

# result 1
	section1=$(printf "%s" "$workspaces")
	section2=$(printf " %s " "$dateTime")
	section3=$(printf "%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s" "Wi-Fi: $wifi" "Ethernet: $ethernet" "Bluetooth: $bluetooth" "Volume: $volume" "Load: $load" "Battery: $battery")

	section1Width=$(echo "$section1" | wc -m)
	section2Width=$(echo "$section2" | wc -m)
	section3Width=$(echo "$section3" | wc -m)

	barCharacterWidth=322
	spacing1Width=$((($barCharacterWidth - $section2Width) / 2 - $section1Width))
	spacing2Width=$(($barCharacterWidth - $section3Width - $section1Width - $section2Width - $spacing1Width))
	extraWidth=$(($barCharacterWidth - $section1Width - $spacing1Width - $section2Width - $spacing2Width - $section3Width))
	if (( spacing1Width < 0 )); then spacing1Width=0; fi
	if (( spacing2Width < 0 )); then spacing2Width=0; fi
	if ((    extraWidth < 0 )); then    extraWidth=0; fi

	printf "%s%*s%s%*s%s" "$section1" $(($spacing1Width - $extraWidth)) ' ' "$section2" $spacing2Width ' ' "$section3"

# result 2
	#printf "%s [%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s$majorSeparator%s]" \
	#	"$workspaces" \
	#	"Wi-Fi: $wifi" \
	#	"Ethernet: $ethernet" \
	#	"Bluetooth: $bluetooth" \
	#	"Input: $input" \
	#	"Volume: $volume" \
	#	"Load: $load" \
	#	"Battery: $battery" \
	#	"$dateTime"

exit 0
