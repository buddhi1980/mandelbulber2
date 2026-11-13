#!/bin/bash
# Installs the packages that build and run Mandelbulber on Ubuntu 24.04, as in linux.yml.
set -e
sudo apt-get -qq update
sudo apt-get install -y \
	build-essential \
	pkg-config \
	mesa-common-dev \
	libgl1-mesa-dev \
	libpng-dev \
	libgsl-dev \
	libgomp1 \
	liblzo2-2 \
	liblzo2-dev \
	libtiff-dev \
	libopenexr-dev \
	libjxl-dev \
	libsndfile1-dev \
	opencl-headers \
	opencl-clhpp-headers \
	ocl-icd-libopencl1 \
	ocl-icd-opencl-dev \
	qt6-base-dev \
	qt6-tools-dev \
	qt6-multimedia-dev \
	qt6-svg-dev \
	qt6-tools-dev-tools \
	qt6-declarative-dev
