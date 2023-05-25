#!/bin/bash

mpstat | awk '/all/ { print 100 - $NF "%" }'
