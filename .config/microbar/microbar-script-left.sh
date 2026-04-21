
#! if you select _NET_CURRENT_DESKTOP on terminal, workspaces or text entirely break, thanks to the CUT_BUFFER0(STRING) updating









#!/bin/bash

# hide all errors
	exec 2> /dev/null

# ansi color
	ESC=$(printf '\e')

# separators
	majorSeparator="$ESC[90m▕$ESC[m "
	minorSeparator=" / "

# globals
	root=$(xprop -root)

#! workspaces
	currentWorkspace=$(echo "$root" | grep _NET_CURRENT_DESKTOP\( | cut -d ' ' -f 3)
	workspaces=$(echo "$root" | grep _NET_DESKTOP_NAMES\( | cut -d ' ' -f 3- | awk "{gsub(/[,\"]/, \"\"); for(i=1; i<=NF; ++i) if(i==$((currentWorkspace+1))) printf \"$ESC[36m[$ESC[37m%s$ESC[36m]\", \$i; else printf \" $ESC[90m%s \", \$i}")
	if [ "$workspaces" == '' ]; then
		workspaces=$ESC[90mnone
	fi

#! active window
	activeWindow=$(echo "$root" | grep _NET_ACTIVE_WINDOW\( | cut -d ' ' -f 5)
	if [ "$activeWindow" == '0x0' ] || [ "$activeWindow" == '' ]; then
		activeWindow="$(echo "$root" | grep WM_NAME\( | head -n 1 | cut -d ' ' -f 3)"
		activeWindow=${activeWindow#\"}
		activeWindow=${activeWindow%\"}
		if [ "$activeWindow" == '' ]; then
			activeWindow='untitled'
		fi
		activeWindow=$majorSeparator$ESC[37m$activeWindow
	else
		activeWindow=$(xprop -id $activeWindow | grep WM_NAME\( | head -n 1 | cut -d ' ' -f 3- | grep -oPz '(*UTF)[\x{0000}-\x{FFFF}]+')
		activeWindow=${activeWindow#\"}
		activeWindow=${activeWindow%\"}
		if [ "$activeWindow" == '' ]; then
			activeWindow='untitled'
		fi
		activeWindow=$majorSeparator$ESC[37m$activeWindow
		temp=$(echo "$activeWindow" | awk '{gsub(/\\\"/, "\""); print substr($0, 1, 70)}')
		if (( ${#temp} < ${#activeWindow} )); then
			activeWindow="$temp..."
		fi
	fi

# tray icons
	color="$ESC[90m"
	trayIcons="$color[-][-][-][-][-]"

# language
	color="$ESC[37m"
	language="$color$(xkb-switch)"

# result
	printf "%s$majorSeparator%s$majorSeparator%s%s" "$workspaces" "$trayIcons" "$language" "$activeWindow"

exit 0

