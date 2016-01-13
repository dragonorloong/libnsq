#!/bin/bash
COREMAXBLOCKS=unlimited
FILEMAXNUM=6553500

ulimit -c $COREMAXBLOCKS
ulimit -n $FILEMAXNUM
./bin_test
