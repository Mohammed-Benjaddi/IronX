#!/bin/bash
pid=$(ps aux | grep 8888 | awk '{print $2}')
if [ -n "$pid" ]; then
  pkill -9 $pid
fi
