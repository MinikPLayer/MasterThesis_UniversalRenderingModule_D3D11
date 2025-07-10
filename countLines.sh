#!/bin/sh

find . \( -name '*.h' -o -name '*.cpp' -o -name '*.hlsl' \) | xargs wc -l | sort -nr
