import hou
import sys
import os
from collections import OrderedDict
import math

# info
__author__ = "Adrian Meyer @Animationsinstitut Filmakademie Baden-Wuerttemberg"
__copyright__ = "2019 All rights reserved. See LICENSE for more details."
__status__ = "Prototype"

def create_render_dir():
	input = hou.ui.readInput("Path to Create", buttons=('OK',))
	path = input[1]
	os.mkdir(path)
	print "{}\nDirectory created.".format(path)


folder_name = "Mandelbulber Animation"

#imports animation curves from mandelbulber .fract files into houdini for editing
def import_animation():
	print "\n"
	print "_" * 50
	print "Starting Import of Mandelbulber Animation..."
	print "_" * 50
	print "\n"


	# precision fixing
	precision_scale = 1000000.0


	fract_filepath = hou.ui.selectFile(title="Source .fract File", pattern="*.fract")
	if fract_filepath == "":
		print "Import cancelled."
		sys.exit()

	precision_scale = hou.ui.readInput("Please enter 'Value Scale Scale Factor' for easier editing of small values in Houdini",
										close_choice=1,
										buttons=('OK', 'Cancel'),
										title="Value Scale",
										initial_contents="1000.0")

	if precision_scale[0] == 1:
		print "Import cancelled."
		sys.exit()

	precision_scale = float(precision_scale[1])

	print "Mandelbulber File:\n{}\n".format(fract_filepath)

	print "Precision Scale fior Import: {}\n".format(precision_scale)

	fract_file = open(fract_filepath, "r")
	fract_string = fract_file.read()
	fract_file.close()

	mode = ""
	if "[frames]" in fract_string and "[keyframes]" not in fract_string:
		print "Only Flight Animation available for import..."
		mode = "flight"

	if "[keyframes]" in fract_string and "[frames]" not in fract_string:
		print "Only Keyframe Animation available for import..."
		mode = "keyframes"

	if "[keyframes]" in fract_string and "[frames]" in fract_string:
		print "Keyframe and Flight Animation available for import. Please choose."
		mode_int = hou.ui.selectFromList(("Flight Animation", "Keyframe Animation"),
										 default_choices=(0,),
										 exclusive=True,
										 message="Choose Animation Type to import",
										 title="Animation Type",
										 column_header="Choices",
										 num_visible_rows=2,
										 clear_on_cancel=True,
										 width=400,
										 height=200)

		if mode_int == ():
			print "Import cancelled."
			sys.exit()

		if mode_int[0] == 0:
			mode = "flight"
			print "Flight Animation Chosen, starting Import...\n"

		if mode_int[0] == 1:
			mode = "keyframes"
			print "Keyframe Animation Chosen, starting Import..."
			print "Please Note that Keyframe Interpolation will be ignored...\n"

	# init variables
	anim_start = 0
	anim_end = 100
	flight_anim_out = ""
	keyframes_anim_out = ""
	flight_anim_parms = ""
	keyframes_anim_parms = ""

	# flight mode
	if mode == "flight":
		flight_anim = fract_string.split("[frames]")[-1]
		try:
			flight_anim = flight_anim.split("[keyframes]")[0]
		except:
			pass

		flight_anim_out = ""
		for line in flight_anim.split("\n"):
			line = " ".join(line.split(";")[1:])
			line = line.replace(",", ".")

			# precision fixing
			nums = []
			try:
				for num in line.split(" "):
					num = float(num) * precision_scale
					num = str(num)
					nums.append(num)
				line = " ".join(nums)
			except:
				pass

			flight_anim_out += line + "\n"

		flight_anim_parms = flight_anim_out.split("\n")[1]
		flight_anim_out = flight_anim_out.split("\n")[2:-2]
		flight_anim_out = "\n".join(flight_anim_out)

		# print "Flight Animation String:"
		# print flight_anim_out
		# print "End Flight Animation String"

		if "flight_first_to_render" in fract_string:
			anim_start = int((fract_string.split("flight_first_to_render ")[-1]).split(";")[0])
		anim_end = int((fract_string.split("flight_last_to_render ")[-1]).split(";")[0]) - 1

	# keyframes mode
	if mode == "keyframes":
		keyframes_anim = fract_string.split("[keyframes]")[-1]

		keyframes_anim_out = ""
		for line in keyframes_anim.split("\n"):
			line = " ".join(line.split(";")[1:])
			line = line.replace(",", ".")

			# precision fixing
			nums = []
			try:
				for num in line.split(" "):
					num = float(num) * precision_scale
					num = str(num)
					nums.append(num)
				line = " ".join(nums)
			except:
				pass

			keyframes_anim_out += line + "\n"
		keyframes_anim_out = "\n".join(keyframes_anim_out.split("\n")[1:-2])

		keyframes_anim_parms = keyframes_anim_out.split("\n")[0]

		keyframes_anim_interpolation = keyframes_anim_out.split("\n")[-1]
		keyframes_anim_out = keyframes_anim_out.split("\n")[1:-1]
		keyframes_anim_out = "\n".join(keyframes_anim_out)

		'''
		print "Keyframes Animation String:"
		print keyframes_anim_out
		print "Keyframes Anim Parms:"
		print keyframes_anim_parms
		print "Keyframes Interpolation"
		print keyframes_anim_interpolation
		print "End Strings\n"
		'''

		if "keyframe_first_to_render" in fract_string:
			anim_start = int((fract_string.split("keyframe_first_to_render ")[-1]).split(";")[0])
		anim_end = int((fract_string.split("keyframe_last_to_render ")[-1]).split(";")[0]) - 1

	# write to .chan file
	chan_filepath = fract_filepath.replace(".fract", ".chan")
	chan_file = open(chan_filepath, "w")
	# flight mode
	if mode == "flight":
		chan_file.write(flight_anim_out)
	if mode == "keyframes":
		chan_file.write(keyframes_anim_out)
	chan_file.close()
	print "Converted to .chan File succesfully:\n{}".format(chan_filepath)

	# set up houdini scene

	fps = 30
	if "keyframe_frames_per_second" in fract_string:
		fps = fract_string.split("keyframe_frames_per_second ")
		fps = (fps[-1].split(";"))[0]
		fps = int(fps)

	print "Anim Start: {}".format(anim_start)
	print "Anim End: {}".format(anim_end)
	print "FPS: {}".format(fps)
	print "\n"

	hou.hipFile.clear()
	hou.hscript("fps {}".format(fps))
	hou.hscript("tset {} {}".format(float(-1) / float(fps), float(anim_end) / float(fps)))
	hou.hscript("frange {} {}".format(anim_start, anim_end))
	print "Animation Range and FPS set in Houdini Scene\n"

	# create null object and extract parms
	anim_null = hou.node("/obj/").createNode("null", "mdb_anim_null")
	anim_null.setSelected(1, 1)
	anim_null.setDisplayFlag(0)

	if mode == "flight":
		anim_parms = flight_anim_parms
	if mode == "keyframes":
		anim_parms = keyframes_anim_parms

	anim_parms = anim_parms.split(" ")

	extensions = ["x", "y", "z", "w"]
	anim_parms_split = []
	for parm in anim_parms:
		
		split = parm.rsplit("_", 1)
		if parm[-1] in extensions:
			if len(split) > 1:
				anim_parms_split.append(split[0])
		else:
			anim_parms_split.append(parm)

	unique_list = []
	for item in anim_parms_split:
		if item not in unique_list:
			unique_list.append(item)

	parms_dict = OrderedDict()
	for item in unique_list:
		parms_dict[item] = anim_parms_split.count(item)

	print "\nAnimation Parameters:"
	for item in parms_dict:
		print "SIZE: {}; NAME: {}".format(parms_dict[item], item)

	# create parms
	parm_group = anim_null.parmTemplateGroup()
	parm_folder = hou.FolderParmTemplate("folder", folder_name)

	i = 0
	for item in parms_dict:
		size = parms_dict[item]
		if size > 1:
			name = "_{}_{}_".format(i, item).format(item)
			parm_folder.addParmTemplate(hou.FloatParmTemplate(name, name, size))
		else:
			name = "_{}_{}".format(i, item).format(item)
			parm_folder.addParmTemplate(hou.FloatParmTemplate(name, name, size))
		i += 1

	parm_folder.addParmTemplate(hou.FloatParmTemplate("display_size", "Display Size", 1))
	parm_folder.addParmTemplate(hou.FloatParmTemplate("precision_scale", "Precision Scale", 1))
	parm_group.append(parm_folder)
	anim_null.setParmTemplateGroup(parm_group)

	# set parms to animated

	xform_parms = anim_null.parmsInFolder(("Transform",))
	for parm in xform_parms:
		parm.setAutoscope(0)
		parm.setScope(0)

	parms = anim_null.parmsInFolder((folder_name,))
	parm_tuples = anim_null.parmTuplesInFolder((folder_name,))

	parm_pattern = []
	for parm in parms:
		parm_exceptions = ["display_size", "precision_scale"]
		if parm.name() not in parm_exceptions:
			parm.setAutoscope(1)
			parm.setScope(1)
			parm.setKeyframe(hou.Keyframe(0))
			parm_pattern.append(parm.path())
	parm_pattern = " ".join(parm_pattern)
	#print "\nParm Pattern: " + parm_pattern

	# load data from .chan file

	hou.hscript("chread -f {} {} -n {} {}".format(anim_start, anim_end, parm_pattern, chan_filepath))
	print "\nAnimation Data read back from .chan File and applied to Parameters on Animation Null."

	# create helper nulls

	display_size_parm = anim_null.parm("display_size")
	display_size_parm.set(0.025)

	precision_scale_parm = anim_null.parm("precision_scale")
	precision_scale_parm.set(precision_scale)

	# cam null
	cam_null = hou.node("/obj/").createNode("null", "cam_null")
	cam_null.move([-2, -2])
	cam_null_col = (1, 0, 0)
	cam_null_col_parm = cam_null.parmTuple("dcolor")
	cam_null_col_parm.set(cam_null_col)
	cam_null.setColor(hou.Color(cam_null_col))
	cam_null_scale = cam_null.parm("geoscale")
	cam_null_scale.setExpression('ch("../mdb_anim_null/display_size")')
	cam_null.parm("tx").setExpression('ch("../mdb_anim_null/_0_main_camera_x")')
	cam_null.parm("ty").setExpression('ch("../mdb_anim_null/_0_main_camera_y")')
	cam_null.parm("tz").setExpression('ch("../mdb_anim_null/_0_main_camera_z")')

	# target null
	target_null = hou.node("/obj/").createNode("null", "target_null")
	target_null.move([2, -2])
	target_null_col = (0, 0, 1)
	target_null_col_parm = target_null.parmTuple("dcolor")
	target_null_col_parm.set(target_null_col)
	target_null.setColor(hou.Color(target_null_col))
	target_null_scale = target_null.parm("geoscale")
	target_null_scale.setExpression('ch("../mdb_anim_null/display_size")')
	target_null.parm("tx").setExpression('ch("../mdb_anim_null/_1_main_target_x")')
	target_null.parm("ty").setExpression('ch("../mdb_anim_null/_1_main_target_y")')
	target_null.parm("tz").setExpression('ch("../mdb_anim_null/_1_main_target_z")')

	### save hipFile
	hip_filepath = fract_filepath.replace(".fract", ".hip")
	hou.hipFile.save(hip_filepath)

	print "\n"
	print "_" * 50
	print "Finished Import of Mandelbulber Animation..."
	print "Hip File saved:"
	print hip_filepath
	print "Edit Animation and then Export..."
	print "_" * 50
	print "\n"


#exports edited animation curves back from houdini to mandelbulber .fract file
def export_animation():
	print "\n"
	print "_" * 50
	print "Starting Export of Edited Mandelbulber Animation..."
	print "Please Note that the export only exports to Mandelbulber Flight Animation..."
	print "_" * 50
	print "\n"

	fract_filepath = hou.ui.selectFile(title="Select Original Source .fract File (Will not be overwritten)",
									   pattern="*.fract")

	if fract_filepath == "":
		print "Export cancelled."
		sys.exit()

	fract_filepath_edit = "{}_edit.fract".format(fract_filepath.split(".fract")[0])

	print "Mandelbulber Source File:\n{}\n".format(fract_filepath)
	print "Mandelbulber Target File:\n{}\n".format(fract_filepath_edit)

	fract_file = open(fract_filepath, "r")
	fract_string = fract_file.read()
	fract_file.close()
	# print fract_string

	# export edited anim as .chan file

	anim_null = hou.node("/obj/mdb_anim_null")
	parms = anim_null.parmsInFolder((folder_name,))
	parm_tuples = anim_null.parmTuplesInFolder((folder_name,))

	precision_scale_parm = anim_null.parm("precision_scale")
	precision_scale = precision_scale_parm.eval()

	parm_pattern = []
	for parm in parms:
		parm_exceptions = ["display_size", "precision_scale"]
		if parm.name() not in parm_exceptions:
			parm.setAutoscope(1)
			parm.setScope(1)
			parm.setKeyframe(hou.Keyframe(0))
			parm_pattern.append(parm.path())
	parm_pattern = " ".join(parm_pattern)
	# print "\nParm Pattern: " + parm_pattern

	chan_filepath = fract_filepath_edit.replace(".fract", ".chan")

	# set frame to -1 so export doesn't kill first frame (houdini bug)
	hou.hscript("fcur -1")

	hou.hscript("chwrite {} {}".format(parm_pattern, chan_filepath))
	print "Exported .chan File from Houdini succesfully:\n{}".format(chan_filepath)

	# read data from .chan file back in

	chan_file = open(chan_filepath, "r")
	chan_string = chan_file.read()
	chan_file.close()

	chan_string_lines = chan_string.split("\n")[:-1]

	chan_string_out = []

	i = 0
	for line in chan_string_lines:
		vals = []
		nums = []
		# precision fixing
		vals = line.split("\t")[1:]
		for num in vals:
			num = float(num) / precision_scale
			num = str(num)
			nums.append(num)

		line = ";".join(nums)
		line = line.replace(".", ",")

		line = "{};{}".format(i, line)
		chan_string_out.append(line)
		i += 1
		
	flight_anim_out = "\n".join(chan_string_out)

	# print "\n.chan String:"
	# print chan_string_out
	# print "\n"

	# check if flight anim present in file
	fract_string_edit = ""
	if "[frames]" in fract_string:
		flight_anim = fract_string.split("[frames]")[-1]
		try:
			flight_anim = flight_anim.split("[keyframes]")[0]
		except:
			pass
		flight_anim = "\n".join(flight_anim.split("\n")[2:-1])
		fract_string_edit = fract_string.replace(flight_anim, flight_anim_out)

	# if not present, insert
	else:
		anim_parms = (fract_string.split("[keyframes]")[-1]).split("\n")[1]
		flight_anim_out = "[frames]\n" + anim_parms + "\n" + flight_anim_out + "\n[keyframes]"
		fract_string_edit = fract_string.replace("[keyframes]", flight_anim_out)

	split_last_to_render = fract_string_edit.split("flight_last_to_render ")
	fract_string_edit = split_last_to_render[0] + "flight_last_to_render " + str(
		int(hou.expandString("$FEND")) + 1) + ";" + ";".join(split_last_to_render[-1].split(";")[1:])

	# write new edited .fract file

	fract_file_edit = open(fract_filepath_edit, "w")
	fract_file_edit.write(fract_string_edit)
	fract_file_edit.close()
	print "\nEdited and saved new .fract File succesfully:\n{}".format(fract_filepath_edit)

	print "\n"
	print "_" * 50
	print "Finished Export of Edited Mandelbulber Animation..."
	print "Open edited File in Mandelbulber..."
	print "_" * 50
	print "\n"



#recreates houdini camera from rendered mandelbulber .exr sequence using exr metadata
def create_cam():
	
	seq = hou.ui.selectFile(
		title="Select first frame of EXR Sequence",
		collapse_sequences=True,
		pattern="*.exr",
		width=800,
		height=600)

	if seq == "":
		print "Cancelled..."
		sys.exit()
	seq = hou.expandString(seq)
	
	fps = hou.ui.readInput(
		"Set FPS for Image Sequence",
		buttons=('OK', 'Cancel',),
		default_choice=0,
		close_choice=1,
		title=None,
		initial_contents="60")

	if fps[0] == 1:
		print "Cancelled..."
		sys.exit()
	else:
		fps = int(fps[1])
		
	print "\nStart Camera creation from EXR Sequence..."
	print "First Frame:"
	print seq
	print "\n"

	#create copnet
	img = hou.node("/img").createNode("img", "img")
	cop = img.createNode("file", "read_seq")
	cop.parm("nodename").set(0)

	fileparm = cop.parm("filename1")
	fileparm.set("")
	
	split = os.path.split(seq)
	dir = split[0]
	start = split[1].split("_")[0]
	temp = split[1].split("_")[-1]
	mid = temp.split(".")[0]
	end = temp.split(".")[-1]
	
	pad = len(mid)
	start_frame = int(mid)
	num_frames = len(os.listdir(dir))
	end_frame = num_frames-1
	
	print "Start Frame: {}".format(start_frame)
	print "End Frame: {}".format(end_frame)
	print "Number of Frames: {}".format(num_frames)
	print "FPS: {}\n".format(fps)

	#houdini scene setup
	hou.hscript("fps {}".format(fps))
	hou.hscript("tset {} {}".format(float(-1) / float(fps), float(end_frame) / float(fps)))
	hou.hscript("frange {} {}".format(start_frame, end_frame))
	hou.hscript("fcur 0")
	
	#static metadata
	fileparm.set(seq)

	meta = cop.getMetaDataString("attributes")
	meta = eval(meta)
	width = cop.xRes()
	height = cop.yRes()
	aspect = float(height) / float(width)

	perspective_type = meta["perspective_type"] #"persp_three_point", "persp_equirectangular", 
	stereo_enabled = meta["stereo_enabled"] #"yes", "no"
	stereo_infinite_correction = int(meta["stereo_infinite_correction"]) #[0, 1]


	print "Perspective Type: {}".format(perspective_type)
	print "Stereo Enabled: {}".format(stereo_enabled)
	print "Stereo Infinite Correction: {}".format(stereo_infinite_correction)
	print "Width: {}".format(width)
	print "Height: {}".format(height)
	print "Aspect: {}".format(aspect)

	#create camera
	cam = ""

	vr_cam_typename_to_create = "vrcam"
	cam_aperture_base = 36
	cam_vr_focal = 18

	def create_vr_cam():
		cam = hou.node("/obj").createNode(vr_cam_typename_to_create)
		cam.parm("vrmergemode").set(2)
		cam.parm("vrmergeangle").set(90)
		cam.parm("vreyetoneckdistance").set(0)
		vr_layout_parm = cam.parm("vrlayout")
		if aspect == 0.5:
			vr_layout_parm.set(2)
		if aspect == 1:
			vr_layout_parm.set(1)
		if aspect == 0.25:
			vr_layout_parm.set(0)

		return cam


	if stereo_enabled == "yes":
		if perspective_type == "persp_equirectangular":
			cam = create_vr_cam()
		elif perspective_type == "persp_three_point":
			cam = hou.node("/obj").createNode("stereocamrig")
		else:
			raise Exception("Perspective Type '{}' not supported by Houdini.".format(perspective_type))

	if stereo_enabled == "no":
		if perspective_type == "persp_equirectangular":
			cam = create_vr_cam()
		elif perspective_type == "persp_three_point":
			cam = hou.node("/obj").createNode("cam")
		else:
			raise Exception("Perspective Type '{}' not supported by Houdini.".format(perspective_type))

	#set res
	cam.parm("resx").set(width)
	cam.parm("resy").set(height)


	# start loop
	print "\nStart iterating over frames...\n"

	keys_tx = []
	keys_ty = []
	keys_tz = []

	keys_rx = []
	keys_ry = []
	keys_rz = []

	keys_targetx = []
	keys_targety = []
	keys_targetz = []

	keys_topx = []
	keys_topy = []
	keys_topz = []

	keys_focal = []
	keys_stereo = []

	for i in range(num_frames):
		frame = str(i).zfill(pad)
		file = "{}/{}_{}.{}".format(dir, start, frame, end)
		#print "Current File: {}".format(file)
		fileparm.set(file)

		meta = cop.getMetaDataString("attributes")
		meta = eval(meta)

		#get values from metadata

		#camera position
		translate_x = float(meta["camera.x"])
		translate_y = float(meta["camera.y"])
		translate_z = float(meta["camera.z"])
		translate = hou.Vector3(translate_x, translate_y, translate_z)

		key_tx = hou.Keyframe(translate_x, hou.frameToTime(i))
		keys_tx.append(key_tx)
		key_ty = hou.Keyframe(translate_y, hou.frameToTime(i))
		keys_ty.append(key_ty)
		key_tz = hou.Keyframe(translate_z, hou.frameToTime(i))
		keys_tz.append(key_tz)

		#camera rotation / not correctly exported from mandelbulber
		#thus cam xform matrix calculated from cam vectors
		'''
		#correct mandelbulber meta output
		rotate_x = 90 - float(meta["camera_rotation.y"])
		rotate_y = float(meta["camera_rotation.z"])
		rotate_z = float(meta["camera_rotation.x"])
		'''

		#calculate rotations from cam vectors

		#camera target
		target_x = float(meta["target.x"])
		target_y = float(meta["target.y"])
		target_z = float(meta["target.z"])
		target = hou.Vector3(target_x, target_y, target_z)

		#camera top (up)
		top_x = float(meta["top.x"])
		top_y = float(meta["top.y"])
		top_z = float(meta["top.z"])
		top = hou.Vector3(top_x, top_y, top_z)

		
		# calculate vectors
		forward = (translate - target).normalized();
		right = top.normalized().cross(forward);
		up = forward.cross(right);

		#build matrix
		right_c = hou.Vector4(right.x(), right.y(), right.z(), 0);
		up_c = hou.Vector4(up.x(), up.y(), up.z(), 0);
		forward_c = hou.Vector4(forward.x(), forward.y(), forward.z(), 0);
		translate_c = hou.Vector4(translate.x(), translate.y(), translate.z(), 1);
		m = hou.Matrix4((right_c, up_c, forward_c, translate_c))

		#extract rotations
		pivot_v = hou.Vector3(0, 0, 0)
		pivot_rotate_v = hou.Vector3(0, 0, 0)
		rotate = m.extractRotates(transform_order='srt', rotate_order='xyz', pivot=pivot_v, pivot_rotate=pivot_rotate_v)
		rotate_x = rotate.x()
		rotate_y = rotate.y()
		rotate_z = rotate.z()


		key_rx = hou.Keyframe(rotate_x, hou.frameToTime(i))
		keys_rx.append(key_rx)
		key_ry = hou.Keyframe(rotate_y, hou.frameToTime(i))
		keys_ry.append(key_ry)
		key_rz = hou.Keyframe(rotate_z, hou.frameToTime(i))
		keys_rz.append(key_rz)


		fov = float(meta["fov"])
		#calulate focal length based on fov and "cam_aperture_base"
		focal = cam_aperture_base / (2 * math.tan(math.radians(fov) / 2))
		key_focal = hou.Keyframe(focal, hou.frameToTime(i))
		keys_focal.append(key_focal)

		stereo_eye_distance = 2 * float(meta["stereo_eye_distance"])
		key_stereo = hou.Keyframe(stereo_eye_distance, hou.frameToTime(i))
		keys_stereo.append(key_stereo)

		#print "\nFrame: {}".format(frame)
		#print "Translate: ({}, {}, {})".format(translate_x, translate_y, translate_z)
		#print "Rotate: ({}, {}, {})".format(rotate_x, rotate_y, rotate_z)
		#print "Stereo Distance: {}".format(stereo_eye_distance)

		
	#set keyframes
	parm_tx = cam.parm("tx")
	parm_tx.setKeyframes(keys_tx)
	parm_ty = cam.parm("ty")
	parm_ty.setKeyframes(keys_ty)
	parm_tz = cam.parm("tz")
	parm_tz.setKeyframes(keys_tz)

	parm_rx = cam.parm("rx")
	parm_rx.setKeyframes(keys_rx)
	parm_ry = cam.parm("ry")
	parm_ry.setKeyframes(keys_ry)
	parm_rz = cam.parm("rz")
	parm_rz.setKeyframes(keys_rz)

	parm_aperture = cam.parm("aperture")
	parm_aperture.set(cam_aperture_base)
	parm_focal = cam.parm("focal")

	if perspective_type == "persp_equirectangular":
		parm_focal.set(cam_vr_focal)
		parm_stereo = cam.parm("vreyeseparation")
		if stereo_enabled == "yes":
			parm_stereo.setKeyframes(keys_stereo)
		else:
			parm_stereo.set(0)
	else:
		parm_focal.setKeyframes(keys_focal)
		if stereo_enabled == "yes":
			parm_stereo = cam.parm("interaxial")
			parm_stereo.setKeyframes(keys_stereo)
		
	
	#delete img node
	img.destroy()

	#select camera
	cam.setSelected(1, 1)
	print "\nCamera successfully created from Mandelbulber EXR Image Sequence.\n\n"
