#!/usr/bin/env python
import subprocess
import Queue
import multiprocessing

class Worker(multiprocessing.Process):
	def __init__(self, queue, prefix, binPath):
		super(Worker, self).__init__()
		self.queue= queue
		self.prefix= prefix
		self.binPath= binPath

	def run(self):
		print "Worker started " + str(self.prefix)
		for iteration in iter( self.queue.get, None ):
			print str(iteration) + " of " + str(totalIterations)
			# Use data
			cmd = str(self.prefix) + " " + self.binPath + " --never-delete --nogui " + settings_file + " --format png16alpha --no-cli-color --output /home/mandelbulber2/render/ --flight --start " + str(iteration*RenderSets) + " --end " + str((iteration+1)*RenderSets) + resoution
			cmd = "xterm -e '" + cmd + "'"
			print cmd
			process = subprocess.Popen(cmd, shell=True)
			process.wait()
			print str(self.prefix) + " completed"

#globals
start = 0
#start = 257
#total_frames = 4500
total_frames = 6000
micCount = 8
RenderSets = 10
totalIterations = total_frames/RenderSets
#resoution = " --res 3840x2160"
resoution = " --res 4096x2048"
binPath_k1om = "/home/mandelbulber2/build-mic/mandelbulber2"
binPath_x64 = "/home/mandelbulber2/build/mandelbulber2"
#settings_file = "/home/mandelbulber2/_menger-coastn_anim.fract"
#settings_file = "/home/mandelbulber2/sponged.fract"
#settings_file = "/home/mandelbulber2/continuum.fract"
settings_file = "/home/mandelbulber2/menger-4D.fract"

if __name__ == "__main__":
	request_queue =  multiprocessing.Queue()
	# co-procs
	for i in range(micCount):
		Worker( request_queue, "ssh mic" + str(i), binPath_k1om ).start()
	# host
	Worker( request_queue, "", binPath_x64 ).start()
	
	for data in range(start, total_frames/RenderSets, 1):
		request_queue.put( data )

	# Sentinel objects to allow clean shutdown: 1 per worker.
	for i in range(micCount):
		request_queue.put( None )

