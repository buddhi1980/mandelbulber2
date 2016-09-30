#!/usr/bin/env python
from __future__ import print_function
import multiprocessing
import os
import platform
import Queue
import random
import subprocess
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
        "keyframe_anim_mandelbulb.fract", \
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

# Determine Host OS #
if(platform.system() == 'Windows'):
        windows = True
else:
        windows = False

# Options for Benchmark #
start = 0
totalIterations = len(settings)

# Tested on WindowsServer2012R2 #
if windows == True:
        WorkersCount = 1
        spacer = "\""
        src_dir = "D:" + os.sep \
                  + "GIT" + os.sep \
                  + "mandelbulber2-VSO" + os.sep
        binPath_x64 = "\"" \
                      + "C:" + os.sep \
                      + "Program Files" + os.sep \
                      + "Mandelbulber v2 win64" + os.sep \
                      + "mandelbulber2.exe" \
                      + "\""

# Tested on CentOS7 #
else:
        WorkersCount = 8
        spacer = "\'"
        src_dir = os.sep + "home" \
                  + os.sep + "mandelbulber2" \
                  + os.sep
        binPath_k1om = os.sep + "home" \
                       + os.sep + "mandelbulber2" \
                       + os.sep + "build-mic" \
                       + os.sep + "mandelbulber2"
        binPath_x64 = os.sep + "home" \
                      + os.sep + "mandelbulber2" \
                      + os.sep + "build" \
                      + os.sep + "mandelbulber2"

# Common Directories #
example_dir = src_dir + "mandelbulber2" \
              + os.sep + "deploy" \
              + os.sep + "share" \
              + os.sep + "mandelbulber2" \
              + os.sep + "examples" \
              + os.sep
output_dir = src_dir + os.sep \
             + "render2" \
             + os.sep

class Worker(multiprocessing.Process):
	def __init__(self, queue, prefix, binPath, results, resolution):
		super(Worker, self).__init__()
		self.queue= queue
		self.prefix= prefix
		self.binPath= binPath
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
			print(str(iteration) + " of " + str(totalIterations - 1))
			cmd = str(self.prefix) \
                              + spacer \
                              + self.binPath \
                              + " --nogui" \
                              + " --format png16alpha" \
                              + " --no-cli-color " \
                              + "\"" \
                              + example_dir + settings[iteration] \
                              + "\"" \
                              + " --output " \
                              + output_dir + os.sep \
                              + GetOutputName(iteration) \
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
                              + settings[iteration] \
                              + " \t " \
                              + str(iteration) \
                              + " \t " \
                              + str(end - start) \
                              + " \t " \
                              + self.resolution
			self.results.append(result)
			print(result)


def GetTimeStamp():
        return str(time.strftime('%Y%m%d%H%M%S'))

def GetOutputName(iteration):
	setting = settings[iteration]
        return str(iteration) \
               + "__" \
               + setting.translate(None, '()\\/ -_.!@#$') \
               + "__" \
               + GetTimeStamp() \
               + ".png"

def trial_run(resolution):
	# Randomize Trial Order #
	random.shuffle(settings)
	request_queue =  multiprocessing.Queue()
	workers = []
	for i in range(WorkersCount):
                if windows == True:
			w = Worker( request_queue, "", binPath_x64, results, resolution )
		else:
                        w = Worker( request_queue, "ssh mic" + str(i) + " ", binPath_k1om, results, resolution )
		w.start()
		workers.insert(0, w)
        # trial iterations #
	for data in range(start, totalIterations, 1):
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
        #trial_run(" --res 50x50")
        #trial_run(" --res 1920x1080")
        trial_run(" --res 3840x2160")
        #trial_run(" --res 7680x4320")
        print_results()

