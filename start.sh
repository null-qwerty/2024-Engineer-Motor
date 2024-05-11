#!/usr/bin/bash

cd build
# start remote process
nohup ./remote &
remote_pid=$!

# start motor control process
nohup ./HelloWorld &
motor_pid=$!

# watching dog
while true; do
    # if remote process is dead, restart it
    if ! kill -0 $remote_pid 2>/dev/null; then
        echo "remote process is dead, restart it"
        nohup ./remote &
        remote_pid=$!
    fi
    # if motor control process is dead, restart it
    if ! kill -0 $motor_pid 2>/dev/null; then
        echo "motor control process is dead, restart it"
        nohup ./HelloWorld &
        motor_pid=$!
    fi
    echo "remote_pid: $remote_pid, motor_pid: $motor_pid"
    # if get SIGINT signal, kill all process
    trap "kill $remote_pid $motor_pid; exit" SIGINT

    sleep 3
done

