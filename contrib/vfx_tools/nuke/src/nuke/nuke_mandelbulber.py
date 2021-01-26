import nuke
import os
import math

# info
__author__ = "Adrian Meyer @Animationsinstitut Filmakademie Baden-Wuerttemberg"
__copyright__ = "2019 All rights reserved. See LICENSE for more details."
__status__ = "Prototype"


def paste_stereo_nodeset(nodeset_path, additional_views=[]):
    """
    Pastes the nodeset from the given nodeset_path.
    If the current nuke script doesn't have stereo views, they will be set up.
    :param nodeset_path:
    :return:
    """
    if nuke.views() == ['main']:
        # only one view (main) exists -> setup for stereo
        user_choice = nuke.ask("This Toolset only works properly in a stereo environment."
                               "\n Allow Nuke to set up a left and right view?")
        if (user_choice == True):
            nuke.root()['setlr'].execute()

    # adds all other views (for gizmos that need additional views for calculation)
    for view in additional_views:
        nuke.addView(view)

    node = nuke.nodePaste(nodeset_path) # Nodeset will be created regardless of user choice.
    return node
    

def createExrCamMandelbulber():

    #Create a camera node based on Mandelbulber EXR metadata.
    print "\nCreating Camera Node based on Mandelbulber EXR metadata in selected Read Node...\n"

    ##### get selected read node
    node = nuke.selectedNode()
    ##### get metadata
    meta = node.metadata()

    #### check required metadata    
    reqFields = [
        "exr/camera.x",
        "exr/camera.y",
        "exr/camera.z",
        "exr/camera_rotation.x",
        "exr/camera_rotation.y",
        "exr/camera_rotation.z",
        "exr/target.x",
        "exr/target.y",
        "exr/target.z",
        "exr/top.x",
        "exr/top.y",
        "exr/top.z",
        "exr/fov",
        "exr/perspective_type",
        "exr/stereo_enabled",
        "exr/stereo_eye_distance",
        "exr/stereo_infinite_correction",
        "input/height",
        "input/width"
    ]

    if not set( reqFields ).issubset( meta ):
        print 'Required Metdata for Camera Creation not found.'
        return
    
    ###### get range
    first = node.firstFrame()
    last = node.lastFrame()
    ret = nuke.getFramesAndViews( 'Create Camera from Metadata', '%s-%s' %( first, last )  )
    fRange = nuke.FrameRange( ret[0] )

    perspective_type = meta["exr/perspective_type"] #"persp_three_point", "persp_equirectangular", 
    stereo_enabled = meta["exr/stereo_enabled"] #"yes", "no"
    stereo_infinite_correction = int(meta["exr/stereo_infinite_correction"]) #[0, 1]

    width = float(node.metadata("input/width", 0))
    height = float(node.metadata("input/height", 0))
    aspect = width / height

    print "Perspective Type: {}".format(perspective_type)
    print "Stereo Enabled: {}".format(stereo_enabled)
    print "Stereo Infinite Correction: {}".format(stereo_infinite_correction)


    ###### create camera
    stereocam_path = os.path.join(os.path.dirname(__file__), "gizmos/stereo_cam.nk")
    stereocam_path = stereocam_path.replace("\\", "/")

    if stereo_enabled == "yes":
        if perspective_type == "persp_equirectangular":
            cam = paste_stereo_nodeset(stereocam_path)
            cam["stereo_width"].setAnimated()
            cam["projection_mode"].setValue(3)
        elif perspective_type == "persp_three_point":
            cam = paste_stereo_nodeset(stereocam_path)
            cam["stereo_width"].setAnimated()
            cam["projection_mode"].setValue(0)
        else:
            raise Exception("Perspective Type '{}' not supported by Nuke.".format(perspective_type))

    if stereo_enabled == "no":
        if perspective_type == "persp_equirectangular":
            cam = nuke.createNode('Camera2')
            cam["projection_mode"].setValue(3)
        elif perspective_type == "persp_three_point":
            cam = nuke.createNode('Camera2')
            cam["projection_mode"].setValue(0)
        else:
            raise Exception("Perspective Type '{}' not supported by Nuke.".format(perspective_type))

    cam_aperture_base = 36.0
    cam_vr_focal = 18.0

    haperture = cam_aperture_base
    vaperture = haperture / aspect
    cam['haperture'].setValue(haperture)
    cam['vaperture'].setValue(vaperture)

    ###### set animated knobs overall
    for k in ( 'focal', 'stereo_width', 'translate', 'rotate'):
        try:
            cam[k].setAnimated()
        except:
            pass

    #set rotation order to ZXY
    cam["rot_order"].setValue(4)


    ##### iterate over frames
    for curTask, frame in enumerate( fRange ):
        
        #get values from metadata

        #camera position
        translate_x = float(node.metadata("exr/camera.x", frame))
        translate_y = float(node.metadata("exr/camera.y", frame))
        translate_z = float(node.metadata("exr/camera.z", frame))
        translate = nuke.math.Vector3(translate_x, translate_y, translate_z)

        #camera rotation / not correctly exported from mandelbulber
        #thus cam xform matrix calculated from cam vectors...

        #camera target
        target_x = float(node.metadata("exr/target.x", frame))
        target_y = float(node.metadata("exr/target.y", frame))
        target_z = float(node.metadata("exr/target.z", frame))
        target = nuke.math.Vector3(target_x, target_y, target_z)

        #camera top (up)
        top_x = float(node.metadata("exr/top.x", frame))
        top_y = float(node.metadata("exr/top.y", frame))
        top_z = float(node.metadata("exr/top.z", frame))
        top = nuke.math.Vector3(top_x, top_y, top_z)

        # calculate vectors
        forward = translate - target;
        forward.normalize()
        top.normalize()
        right = top.cross(forward);
        up = forward.cross(right);

        #build matrix
        right_c = [right.x, right.y, right.z, 0];
        up_c = [up.x, up.y, up.z, 0];
        forward_c = [forward.x, forward.y, forward.z, 0];
        translate_c = [translate.x, translate.y, translate.z, 1];

        m_val_list = right_c + up_c + forward_c + translate_c
        m = nuke.math.Matrix4()
        for i, val in enumerate(m_val_list):
            m[i] = val

        #extract rotations
        m.rotationOnly()
        rotate = m.rotationsZXY()
        rotate_x = math.degrees(rotate[0])
        rotate_y = math.degrees(rotate[1])
        rotate_z = math.degrees(rotate[2])

        #set values on camera
        cam['translate'].setValueAt(translate_x, frame, 0)
        cam['translate'].setValueAt(translate_y, frame, 1)
        cam['translate'].setValueAt(translate_z, frame, 2)

        cam['rotate'].setValueAt(rotate_x, frame, 0)
        cam['rotate'].setValueAt(rotate_y, frame, 1) 
        cam['rotate'].setValueAt(rotate_z, frame, 2)

        #get values from metadata
        #calulate focal length based on fov and "cam_aperture_base"
        fov = float(node.metadata("exr/fov", frame))
        focal = cam_aperture_base / (2 * math.tan(math.radians(fov) / 2))

        stereo_eye_distance = 2 * float(node.metadata("exr/stereo_eye_distance", frame))
        
        #if vr cam
        if perspective_type == "persp_equirectangular":
            focal = cam_vr_focal
            cam['focal'].setValueAt(focal, frame)
            if stereo_enabled == "yes":
                cam['stereo_width'].setValueAt(stereo_eye_distance, frame)

        #if normal perspective cam
        if perspective_type == "persp_three_point":
            cam['focal'].setValueAt(focal, frame)
            if stereo_enabled == "yes":
                cam['stereo_width'].setValueAt(stereo_eye_distance, frame)


    #select camera
    cam.setSelected(1)
    print "\nCamera successfully created from Mandelbulber EXR Image Sequence.\n\n"
