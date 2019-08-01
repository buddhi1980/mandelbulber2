#!/usr/bin/env php
################################# WORK IN PROGRESS
# this file takes a mandelbulb3d formula file (m3f) and does the following:
# - detect code part from m3f file
# 	- convert code hex string to binary
# 	- convert binary to intel hex
# 	- decompile intel hex to python code
# 	- TODO: further processing
# - detect options
# - TODO
#

<?php
require_once(dirname(__FILE__) . '/common.inc.php');
printStart();
$retDecBinPath = '~/Downloads/retdec/bin'; // from here: https://github.com/avast/retdec
$bin2hexPath = '~/Downloads/intelhex-master/scripts'; // from here https://github.com/bialix/intelhex
$mb3dFolder = @$argv[1]; // retrieve from here: https://github.com/coast77777777777/mb3d, see folder M3Formulas
if(substr($mb3dFolder, -1, 1) != '/') $mb3dFolder .= '/';
$i = 0;
foreach (glob($mb3dFolder . "M3Formulas/*.m3f") as $filename) {
	echo "Handling: $filename" . PHP_EOL;
	portFormulaFromFile($filename);
	$i++;
}

function portFormulaFromFile($fileName){
	$contents = file_get_contents($fileName);
	$code = detectCode($contents);
	$options = detectOptions($contents);
	// $constants = detectConstants(); // TODO
	// $description = detectDescription(); // TODO
	// print_r($options);
	die();
}

function detectCode($contents){
	global $bin2hexPath, $retDecBinPath;
	preg_match('/\[CODE\]([\s\S]+)\[END\]/', $contents, $match);
	$code = trim($match[1]);
	echo 'Detected following code:' . PHP_EOL;
	echo $code;

	echo 'Convert code to binary...' . PHP_EOL;
	file_put_contents('/tmp/code.hex', $code);
	shell_exec('xxd -r -p /tmp/code.hex /tmp/code.bin');

	echo 'Convert binary to intel hex...' . PHP_EOL;
	shell_exec($bin2hexPath . '/bin2hex.py /tmp/code.bin > /tmp/code.ihex');
	echo 'Created following intelHex:' . PHP_EOL;
	echo file_get_contents('/tmp/code.ihex');

	echo 'Decompile intel hex to python ...' . PHP_EOL;
	$cmdDec = $retDecBinPath . '/retdec-decompiler.py \
		-a x86-64 -e little \
	    --backend-no-symbolic-names \
	    --backend-no-var-renaming \
	    -l py \
	    /tmp/code.ihex';
	shell_exec($cmdDec);
	echo 'Created following python code:' . PHP_EOL;
	echo file_get_contents('/tmp/code.ihex.py');
}

function detectOptions($content){
	$options = [];
	if(preg_match('/\[OPTIONS\]\n((?:[\.A-Za-z ]+\s*=\s*[\d.]+\n)*)/', $content, $match)){
		$optionsRaw = $match[1];
		$optionsLines = explode(PHP_EOL, $optionsRaw);
		foreach($optionsLines as $optionsLine){
			if(preg_match('/\.([A-Za-z ]+)\s*=\s*([\d\.]+)/', $optionsLine, $matchOption)){
				$options[$matchOption[1]] = $matchOption[2];
			}
		}
	}
	return $options;
}
printFinish();
exit;

?>

