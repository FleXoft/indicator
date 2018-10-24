#!/bin/bash 

die () {
	echo "$1"
	exit 1
}

send () {
	echo "$1" > "$device"
	sleep 0.005	# Don't know why we need this, but it helps to make it more stable. ;-)
}

LED () {
	if [ "$1" -eq 0 ]; then 
		send "+$i:fff333;"
	elif [ "$1" -eq 7 ]; then
		send "+$(($i-1)):333fff;"
	else 
		send "+$(($i-1)):333fff333;"
	fi 
}

fill000 () {
	eval "printf '000%.0s' {1.."$1"}"
}

device=$( ls -l1 /dev/cu.wchusbserial* )

[ -e "$device" ] && echo "Found device ($device) file." || die "Device not found!"

# Set serial device parameters 9600 8 N 1
stty -f "$device" 9600 cs8 -parenb -cstopb

# Read & show the port output 
cat "$device" &
bgPid=$!
echo "Background process has started. Process #: $bgPid"

# Waiting for the LED module initialization
sleep 3

# KITT (Knight Industries Two Thousand) Effect
for c in {0..5}; do
	echo -n "."
	for i in {0..7}; do
	  send "!"
	  LED "$i"
	done
	for i in {7..0}; do 
	  send "!"
	  LED "$i"
	done
done
echo "."

send "!" # Send reset to clear the screen
send "?" # Get status "about"

# main() loop
echo "Press Ctrl+d to Quit!!!"
while read cmd; do
   send "$cmd" 
done

# Garbage collector
echo -n "Killing background process #: $bgPid"
kill "$bgPid"
echo ", result: $?."