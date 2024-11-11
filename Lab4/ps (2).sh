#!/bin/bash

# ELEC377 - Operating System
# Lab 4 - Shell Scripting, ps.sh
# Julia Viger and Sasha Oginni

# Program Description:
# This program uses shell programming to list running processes using the /proc interface

# Initialize flag variables
showRSS="no"
showComm="no"
showCommand="no"
showGroup="no"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -rss)
            showRSS="yes"
            ;;
        -comm)
            if [[ $showCommand == "yes" ]]; then
                echo "Error: Cannot use both -comm and -command options." >&2
                exit 1
            fi
            showComm="yes"
            ;;
        -command)
            if [[ $showComm == "yes" ]]; then
                echo "Error: Cannot use both -comm and -command options." >&2
                exit 1
            fi
            showCommand="yes"
            ;;
        -group)
            showGroup="yes"
            ;;
        *)
            echo "Error: Invalid option '$1'" >&2
            exit 1
            ;;
    esac
    shift
done

# Temp file to store process information
temp_file="/tmp/tmpPs$$.txt"
> "$temp_file"  # Clear any existing content in the file
# Print headers
printf "%-10s %-15s" "PID" "USER" >> "$temp_file"
if [[ "$showGroup" == "yes" ]]; then
    printf "%-15s" "GROUP" >> "$temp_file"
fi
if [[ "$showRSS" == "yes" ]]; then
    printf "%-10s" "RSS" >> "$temp_file"
fi
if [[ "$showComm" == "yes" ]]; then
    printf "%-20s" "COMM" >> "$temp_file"
elif [[ "$showCommand" == "yes" ]]; then
    printf "%-50s" "COMMAND" >> "$temp_file"
fi
printf "\n" >> "$temp_file"

write_process_info() {
    local pid="$1" username="$2" groupname="$3" rss="$4" name="$5" cmdline="$6"
    
    # Conditionally format output based on flags
    printf "%-10s %-15s" "$pid" "$username" >> "$temp_file"
    if [[ "$showGroup" == "yes" ]]; then
        printf "%-15s" "$groupname" >> "$temp_file"
    fi
    if [[ "$showRSS" == "yes" ]]; then
        printf "%-10s" "$rss" >> "$temp_file"
    fi
    if [[ "$showComm" == "yes" ]]; then
        printf "%-20s" "$name" >> "$temp_file"
    elif [[ "$showCommand" == "yes" ]]; then
        printf "%-50s" "$cmdline" >> "$temp_file"
    fi
    printf "\n" >> "$temp_file"
}

for pid_dir in $(ls -d /proc/[0-9]* | sort -n); do
    if [[ -d "$pid_dir" ]]; then
        # Extract relevant process information (Phase 3a)
        pid=$(grep '^Pid:' "$pid_dir/status" | sed -E 's/^Pid:[[:space:]]+//')
        name=$(grep '^Name:' "$pid_dir/status" | sed -E 's/^Name:[[:space:]]+//')
        uid=$(grep '^Uid:' "$pid_dir/status" | sed -E 's/^Uid:[[:space:]]+([0-9]+).*/\1/')
        gid=$(grep '^Gid:' "$pid_dir/status" | sed -E 's/^Gid:[[:space:]]+([0-9]+).*/\1/')
        rss=$(grep '^VmRSS:' "$pid_dir/status" | sed -E 's/^VmRSS:[[:space:]]+([0-9]+).*/\1/')
        rss=${rss:-0} 
        cmdline=$(tr '\0' ' ' < "$pid_dir/cmdline")
        cmdline=${cmdline:-$name}  # Use name if cmdline is empty

        # Convert UID and GID to symbolic names (Phase 3b)
        username=$(grep -E "^([^:]*):[^:]*:$uid:" /etc/passwd | cut -d ':' -f 1)
        if [[ -z "$username" ]]; then
            username="($uid)"
        fi
        groupname=$(grep -E "^([^:]*):[^:]*:$gid:" /etc/group | cut -d ':' -f 1)

        # Write formatted process info to the temp file (Phase 4)
        write_process_info "$pid" "$username" "$groupname" "$rss" "$name" "$cmdline"
    fi
done

# Sort the temp file
sort -n -k1 "$temp_file"

# Clean up the file
rm "$temp_file"