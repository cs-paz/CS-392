###############################################################################
# Author: Christian Szablewski-Paz and Zac Schuh
# Date: 2/16/21
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: junk
###############################################################################
#!/bin/bash

base=$(basename "$0")
username=$(whoami)

Usage(){
cat << ENDOFTEXT
Usage: $base [-hlp] [list of files]
   -h: Display help.   
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
ENDOFTEXT
}

UnknownOption(){
cat << ENDOFTEXT
Error: Unknown option '-$OPTARG'.
ENDOFTEXT
    Usage
}

TooManyOptionsEnabled(){
cat << ENDOFTEXT
Error: Too many options enabled.
ENDOFTEXT
	Usage
}

FileNotFound(){
cat << ENDOFTEXT
Warning '$f' not found.
ENDOFTEXT
	
}

flag_count=0
h_flag=false
p_flag=false
flag_str=""

readonly junk=".junk"
while getopts ":hlp" option; do
	if [ $flag_count -eq 1 ]; then
		TooManyOptionsEnabled
		exit 1
	fi
	case $option in
		h) 	h_flag=true
			(( ++flag_count ))
			;;
		l) 	(ls -lAF /home/$username/$junk)
			(( ++flag_count ))
			exit 0
			;;
		p) 	p_flag=true
			(( ++flag_count ))
			;;
		?)	UnknownOption
			exit 1
		   	;;
	esac		   	
done

#Take in files to move into .junk
declare -a filenames
shift "$(( OPTIND - 1))"
index=0
for f in $@; do
	#If there are any flags enabled, excluding the purge flan
	if [ $flag_count -eq 1 ] && [ ! $p_flag -eq true]; then
		TooManyOptionsEnabled
		exit 1
	fi
	filenames[$index]="$f"
	(( ++index ))
done

#if the h flag is enabled, then show usage message
if $h_flag; then
	Usage
	exit 0
fi

#if the .junk dir doesn't exist, create it
if [ ! -d "/home/$username/$junk" ]; then
	mkdir "/home/$username/$junk"
fi

#if the p flag is enabled, delete all the files in .junk
if $p_flag; then
	files_to_purge=($(ls -A /home/$username/$junk))
	for f in ${files_to_purge[*]}; do
		(cd /home/$username/$junk; rm -rf "${files_to_purge[$index]}")
		(( ++index ))
	done
	exit 0
fi

#create array of files_not_found to compare when moving the files over
index=0
declare -a files_not_found
for f in ${filenames[*]}; do
	if [ ! -f "/home/$username/junk/$f" ] && [ ! -d "/home/$username/junk/$f" ]; then
		files_not_found[$index]="$f"
		FileNotFound
	fi
	(( ++index ))
done

index=0
file_is_found=true
for f in ${filenames[*]}; do
		innerIndex=0
	for e in ${files_not_found[*]}; do
		#if this file is not found, don't run mv on it
		if [ "$e" = "${files_not_found[$index]}" ]; then
			file_is_found=false
		fi	
		(( ++innerIndex ))	
	done
	#if file exists, run mv on it
	if $file_is_found; then
		mv "$f" "/home/$username/$junk"	
	fi
	file_is_found=true
	(( ++index ))
done

