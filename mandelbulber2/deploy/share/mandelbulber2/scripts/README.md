This folder contains scripts to use with Mandelbulber.

For this to work you will need to install Python 3.6 or newer in your system (no extra packages needed).
Then you can invoke these scripts on the command line with `python3 [SCRIPT_NAME].py`
(on Windows: `py [SCRIPT_NAME].py`).

- mbsettings.py & mbsettings_examples.py
	Sometimes Mandelbulber comes short when doing sophisticated parameter manipulation.
	The MbSettings class in mbsettings.py can be used to script your own logic.
	See also useful examples in mbsettings_examples.py on how to use this class.
	Note: mbsettings_examples.py rewrites `../examples/flight_anim_menger sponge_3.fract`;
	copy the scripts and the example file to a folder of your own first.
