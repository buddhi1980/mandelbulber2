#!/usr/bin/env python
from __future__ import print_function
import multiprocessing
import os
import platform
import Queue
import random
import subprocess
import sys
import time

# Settings to Render #
RP = "Robert Pancoast collection - license Creative Commons (CC-BY 4.0)"
settings_file = \
RP + os.sep \
+ "menger-fatbackstrokin.fract"

# Options for Render #
begin = 5190
total_frames = 5500
RenderSets = 2
totalIterations = (total_frames-begin)/RenderSets

# Tested on CentOS7 #
WorkersCount = 8
spacer = "\'"
perspective = " --override perspective_type=3#sweet_spot_vertical_angle=60"
perspective = " --override perspective_type=2"
perspective = ""

# Script Directory #
DOE=os.path.dirname(os.path.realpath(sys.argv[0]))
SRC=DOE + "//..//..//..//"

# Paths #
src_dir = SRC
bin_path_k1om = \
src_dir + os.sep \
+ ".." + os.sep \
+ "build-mic" + os.sep \
+ "mandelbulber2" + os.sep \
+ "mandelbulber2"

# Common Directories #
example_dir = src_dir + "mandelbulber2" \
+ os.sep + "deploy" \
+ os.sep + "share" \
+ os.sep + "mandelbulber2" \
+ os.sep + "examples" \
+ os.sep
output_dir = src_dir + os.sep \
+ "render" \
+ os.sep

class Worker(multiprocessing.Process):
	def __init__(self, queue, prefix, bin_path, results, resolution):
		super(Worker, self).__init__()
		self.queue= queue
		self.prefix= prefix
		self.bin_path= bin_path
		self.results= results
		self.resolution= resolution

	def run(self):
		print("Worker started " + str(self.prefix))
		while True:
			iteration = self.queue.get()
			# finalize iterations #
			if iteration == 'exit':
				print("Cleaning up worker " + str(self.prefix))
				break
			# Use data #
			start = str(begin+iteration*RenderSets)
			end = str(begin+(iteration+1)*RenderSets)
			print(str(iteration) + " of " + str(totalIterations - 1))
			cmd = str(self.prefix) \
			+ spacer \
			+ self.bin_path \
			+ perspective \
			+ " --never-delete" \
			+ " --nogui" \
			+ " --format png16alpha" \
			+ " --no-cli-color " \
			+ "\"" \
			+ example_dir + settings_file \
			+ "\"" \
			+ " --output " \
			+ output_dir \
			+ " --flight" \
			+ " --start " \
			+ start \
			+ " --end " \
			+ end \
			+ self.resolution \
			+ spacer
			cmd = "xterm -e " + cmd
			print(cmd)
			start_time = time.time()
			process = subprocess.Popen(cmd, shell=True)
			process.wait()
			end_time = time.time()
			result = str(self.prefix) \
			+ " \t " \
			+ str(iteration) \
			+ " \t " \
			+ str(end_time - start_time) \
			+ " \t " \
			+ self.resolution
			self.results.append(result)
			print(result)

def render(resolution):
	request_queue = multiprocessing.Queue()
	workers = []
	for i in range(WorkersCount):
		w = Worker( request_queue, "ssh mic" + str(i) + " ", bin_path_k1om, results, resolution )
		w.start()
		workers.insert(0, w)
	# render iterations #
	for data in range(0, totalIterations, 1):
		request_queue.put( data )
	# finalize iterations #
	for data in range(0, WorkersCount, 1):
		request_queue.put('exit')
	for work in workers:
		work.join()

def print_results():
# Display Results #
	print("###############################")
	print("###########_RESULTS_###########")
	print("###############################")
	for i in range(0, len(results), 1):
		print(results[i])
	print("###############################")
	print("###############################")
	print("###############################")

if __name__ == "__main__":
	# multiprocessing safe list #
	results = multiprocessing.Manager().list()
	# output dir #
	if not os.path.exists(output_dir):
		os.makedirs(output_dir)
	# Execute test resolution #
	try:
		#render(" --res 50x50")
		#render(" --res 85x64")
		#render(" --res 320x240")
		#render(" --res 1920x1080")
		#render(" --res 3840x2160")
		#render(" --res 2560x2560")
		render(" --res 7680x4320")
	except KeyboardInterrupt:
		print("KeyboardInterrupt Detected")
	finally:
		print_results()

