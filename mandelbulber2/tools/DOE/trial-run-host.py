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
SJ = "Sebastian Jennen collection - license Creative Commons  (CC-BY 4.0)"
KM = "Krzysztof Marczak collection - license Creative Commons (CC-BY 4.0)"
settings = [ \
"aboxmod1_001.fract", \
"aboxmod2_001.fract", \
"aboxvsicen1_001.fract", \
"aexion_octopus_001.fract", \
"amoxmodkali_001.fract", \
"benesi_t1_pine_tree_001.fract", \
"bristorbrot001.fract", \
"hybrid002.fract", \
"hybrid004.fract", \
"hybrid007.fract", \
"IFS_002.fract", \
"iq_bulb_001.fract", \
"keyframe_anim_mandelbox_boxes.fract", \
"Makin3D-Julia_001.fract", \
"mandelbox001.fract", \
"mandelbox002.fract", \
"mandelbulb002.fract", \
"menger-mod1_001.fract", \
"msltoe_julia_bulb_eiffie_001.fract", \
"primitives001.fract", \
"quaternion_001.fract", \
"riemann_sphere_msltoe_001.fract", \
SJ + os.sep + "hybrid 02 - rectangle hieroglyphs animation.fract", \
SJ + os.sep + "hybrid 02 - rectangle hieroglyphs.fract", \
KM + os.sep + "aexion01.fract", \
KM + os.sep + "benesi03.fract", \
KM + os.sep + "Bristorbrot.fract", \
KM + os.sep + "bristorbrot01.fract", \
KM + os.sep + "constant factor 2.0  - mandelbox scale 2.0.fract", \
KM + os.sep + "equirectangular menger sponge.fract", \
KM + os.sep + "fish eye.fract", \
KM + os.sep + "folded mender sponge.fract", \
KM + os.sep + "hybrid16.fract", \
KM + os.sep + "hybrid17.fract", \
KM + os.sep + "hybrid18_2.fract", \
KM + os.sep + "hybrid19.fract", \
KM + os.sep + "hybrid20.fract", \
KM + os.sep + "hybrid22 - foldigIntPow v 2.fract", \
KM + os.sep + "hybrid42.fract", \
KM + os.sep + "hypercomplex 03.fract", \
KM + os.sep + "IFS 19 - maxiter.fract", \
KM + os.sep + "IFS 20.fract", \
KM + os.sep + "IFS 21.fract", \
KM + os.sep + "IFS 25.fract", \
KM + os.sep + "IFS 29_2.fract", \
KM + os.sep + "IFS 31.fract", \
KM + os.sep + "IFS 32.fract", \
KM + os.sep + "iter fog 002_2.fract", \
KM + os.sep + "iter fog 2.fract", \
KM + os.sep + "mandelbox 22.fract", \
KM + os.sep + "mandelbox10.fract", \
KM + os.sep + "mandelbox11 - rotations.fract", \
KM + os.sep + "mandelbox15 - rotations.fract", \
KM + os.sep + "mandelbox17.fract", \
KM + os.sep + "mandelbox18.fract", \
KM + os.sep + "mandelbox19.fract", \
KM + os.sep + "mandelbox24.fract", \
KM + os.sep + "mandelbox25_3.fract", \
KM + os.sep + "mandelbox27.fract", \
KM + os.sep + "mandelbox28.fract", \
KM + os.sep + "mandelbox29.fract", \
KM + os.sep + "mandelbox30.fract", \
KM + os.sep + "mandelbox31.fract", \
KM + os.sep + "mandelbox36.fract", \
KM + os.sep + "mandelbox49.fract", \
KM + os.sep + "mandelbox52_2.fract", \
KM + os.sep + "mandelbox8 - maxiter.fract", \
KM + os.sep + "mandelbulb powe 6 - circle.fract", \
KM + os.sep + "mandelbulb power 2 - slice 4.fract", \
KM + os.sep + "mandelbulb power 2 - slice 5.fract", \
KM + os.sep + "mandelbulb power 4 - water.fract", \
KM + os.sep + "mandelbulb power 8 - 4_2.fract", \
KM + os.sep + "mandelbulb power 8 - 7 - volmetric fog.fract", \
KM + os.sep + "menger sponge 004_2.fract", \
KM + os.sep + "modified mandelbulb 001.fract", \
KM + os.sep + "orbitTraps 003.fract", \
KM + os.sep + "orbitTraps 004.fract", \
KM + os.sep + "orbitTraps 005.fract", \
KM + os.sep + "orbitTraps 006.fract", \
KM + os.sep + "primitive objects - water.fract", \
KM + os.sep + "xenodreambuie2.fract", \
KM + os.sep + "xenodreambuie3.fract", \
]

class Worker(multiprocessing.Process):
	def __init__(self, \
	queue, \
	prefix, \
	bin_path, \
	results, \
	resolution, \
	output_dir, \
	example_dir\
	):
		super(Worker, self).__init__()
		self.queue= queue
		self.prefix= prefix
		self.bin_path= bin_path
		self.output_dir= output_dir
		self.example_dir= example_dir
		self.results= results
		self.resolution= resolution

	def run(self):
		print("Worker started " + str(self.prefix))
		while True:
			requested = self.queue.get()
			# finalize iterations #
			if requested == 'exit':
				print("Cleaning up worker " + str(self.prefix))
				break
			# Use data #
			run_number = requested[0]
			iteration = requested[1]
			print(str(run_number) + " of " + str(totalIterations - 1) \
			+ " :: " + str(iteration))
			cmd = str(self.prefix) \
			+ spacer \
			+ self.bin_path \
			+ " --nogui" \
			+ " --format png16alpha" \
			+ " --no-cli-color " \
			+ "\"" \
			+ self.example_dir + settings[iteration] \
			+ "\"" \
			+ " --output " \
			+ self.output_dir + os.sep \
			+ get_output_name(iteration) \
			+ self.resolution \
			+ spacer
			if windows == True:
				cmd = "start /wait cmd /C " + cmd
			else:
				cmd = "xterm -e " + cmd
			print(cmd)
			start = time.time()
			process = subprocess.Popen(cmd, shell=True)
			process.wait()
			end = time.time()
			result = str(self.prefix) \
			+ " \t " \
			+ str(iteration) \
			+ " \t " \
			+ str(run_number) \
			+ " \t " \
			+ settings[iteration] \
			+ " \t " \
			+ str(end - start) \
			+ " \t " \
			+ self.bin_path \
			+ " \t " \
			+ self.resolution
			self.results.append(result)
			print(result)

def get_random_order():
	# Randomize Trial iterations #
	data_list = []
	for data in range(start, totalIterations, 1):
		data_list.append( data )
	random.shuffle(data_list)
	return data_list

def get_time_stamp():
	return str(time.strftime('%Y%m%d%H%M%S'))

def get_output_name(iteration):
	setting = settings[iteration]
	return str(iteration) \
	+ "__" \
	+ setting.translate(None, '()\\/ -_.!@#$') \
	+ "__" \
	+ get_time_stamp() \
	+ ".png"

def trial_run(resolution, bin_path, output_dir, example_dir):
	request_queue = multiprocessing.Queue()
	workers = []
	for i in range(WorkersCount):
		if windows == True:
			w = Worker( \
			request_queue, \
			"", \
			bin_path, \
			results, \
			resolution, \
			output_dir, \
			example_dir \
			)
		else:
			w = Worker( \
			request_queue, \
			"ssh mic" + str(i) + " ", \
			bin_path, \
			results, \
			resolution, \
			output_dir, \
			example_dir \
			)
		w.start()
		workers.insert(0, w)
	# Trial Order #
	random_list = get_random_order()
	for run_number in range(start, totalIterations, 1):
		request_queue.put([run_number, random_list.pop()])
	# finalize iterations #
	for data in range(0, WorkersCount, 1):
		request_queue.put('exit')
	for work in workers:
		work.join()
	print_results()

# Display Results #
def print_results():
	print("###############################")
	print("###########_RESULTS_###########")
	print("###############################")
	# Headers #
	header = "prefix" \
	+ " \t " \
	+ "settings index" \
	+ " \t " \
	+ "run number" \
	+ " \t " \
	+ "settings name" \
	+ " \t " \
	+ "end - start" \
	+ " \t " \
	+ "bin_path" \
	+ " \t " \
	+ "resolution"
	# Data #
	print(header)
	for i in range(0, len(results), 1):
		print(results[i])
	print("###############################")
	print("###############################")
	print("###############################")

# get script directory #
def get_script_dir():
	return os.path.dirname(os.path.realpath(sys.argv[0]))

# binary representation of trial configs #
def bin_trials(run_count):
	trials = []
	for idx in range(0, run_count):
		word = "{0:b}".format(idx).zfill(3)
		trials.append(word)
	return trials

# Experimental #
def execute_experiment(test_output_dir, test_example_dir):
	trials = bin_trials(9)
	test_resolution = " --res 1920x1080"
	for idx, trial in enumerate(trials):
		print("executing Experimental run: " + str(idx))
		test_bin_path = SRC + "..//FACTOR_" + trial \
		+ "//BIN//build-mic//mandelbulber2//mandelbulber2"
		print("Testing " + test_bin_path)
		trial_run(test_resolution, \
		test_bin_path, \
		test_output_dir, \
		test_example_dir\
		)

# Options for Benchmark #
start = 0
totalIterations = len(settings)
DOE=get_script_dir()
SRC=DOE + "//..//..//..//"

# Determine Host Settings #
if(platform.system() == 'Windows'):
	windows = True
	WorkersCount = 1
	spacer = "\""
	test_bin_path = spacer \
	+ "C:\\Program Files\\Mandelbulber v2 win64\\mandelbulber2.exe" \
	+ spacer
	test_output_dir = "C:\\temp\\render2\\"
	test_example_dir = "C:\\Program Files\\Mandelbulber v2 win64\\examples\\"
else:
	windows = False
	WorkersCount = 8
	spacer = "\'"
	test_bin_path = SRC + "//..//build-mic//mandelbulber2//mandelbulber2"
	test_output_dir = SRC + "//render2//"
	test_example_dir = SRC + "//mandelbulber2//deploy//share//mandelbulber2//examples//"

# Entry #
if __name__ == "__main__":
	# multiprocessing safe list #
	results = multiprocessing.Manager().list()
	# Select Resolution #
	# 50x50 1920x1080 3840x2160 7680x4320 #
	test_resolution = " --res 50x50"
	# output directory #
	if not os.path.exists(test_output_dir):
		os.makedirs(test_output_dir)
	# Execute test #
	trial_run(test_resolution, \
	test_bin_path, \
	test_output_dir, \
	test_example_dir\
	)
	# experimental multi run #
	if(windows == False):
		print("executing multiple experimental runs")
		execute_experiment(test_output_dir, test_example_dir)
