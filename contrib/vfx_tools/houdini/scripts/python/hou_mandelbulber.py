import hou
import sys
from collections import OrderedDict

# info
__author__ = "Adrian Meyer @Animationsinstitut Filmakademie Baden-Wuerttemberg"
__copyright__ = "2019 All rights reserved. See LICENSE for more details."
__status__ = "Prototype"


folder_name = "Mandelbulber Animation"

def import_animation():
    print "\n"
    print "_" * 50
    print "Starting Import of Mandelbulber Animation..."
    print "_" * 50
    print "\n"

    fract_filepath = hou.ui.selectFile(title="Source .fract File", pattern="*.fract")

    if fract_filepath == "":
        print "Import cancelled."
        sys.exit()

    print "Mandelbulber File:\n{}\n".format(fract_filepath)

    fract_file = open(fract_filepath, "r")
    fract_string = fract_file.read()
    fract_file.close()
    # print fract_string

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

    anim_parms_split = []
    for parm in anim_parms:
        split = parm.rsplit("_", 1)
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
        if parm.name() != "display_size":
            parm.setAutoscope(1)
            parm.setScope(1)
            parm.setKeyframe(hou.Keyframe(0))
            parm_pattern.append(parm.path())
    parm_pattern = " ".join(parm_pattern)
    # print "\nParm Pattern: " + parm_pattern

    # load data from .chan file

    hou.hscript("chread -f {} {} -n {} {}".format(anim_start, anim_end, parm_pattern, chan_filepath))
    print "\nAnimation Data read back from .chan File and applied to Parameters on Animation Null."

    # create helper nulls

    display_size_parm = anim_null.parm("display_size")
    display_size_parm.set(0.025)

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

    parm_pattern = []
    for parm in parms:
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

    # read date from .chan file back in

    chan_file = open(chan_filepath, "r")
    chan_string = chan_file.read()
    chan_file.close()

    chan_string = chan_string.replace("\t", ";")
    chan_string = chan_string.replace(".", ",")

    chan_string_lines = chan_string.split("\n")[:-1]

    chan_string_out = []
    i = 0
    for line in chan_string_lines:
        line = "{}{}".format(i, line)
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
