#!/bin/bash

set -e

echo "execut make clean cmd."
make clean
echo "clean finish."
echo "execut make cmd."
make -j1
make install
echo ""
echo ""
echo "finished"
echo ""
echo ""
