#!/usr/bin/env bash

shopt -s globstar
clear
clang-tidy src/**/*.{h,cpp}