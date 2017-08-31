#!/bin/bash

command_not_found_handle () {
  return 127
}

eval "$1 $2 || python-config $2"
