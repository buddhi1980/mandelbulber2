#!/usr/bin/env php
################################# WORK IN PROGRESS
# this file takes a mandelbulb3d formula file (m3f) and does the following:
# - detect code part from m3f file
# - convert code hex string to binary
# - convert binary to intel hex
# - decompile intel hex to python code
# - TODO: further processing
#
# example input file: https://github.com/thargor6/mb3d/blob/66c45c98eb666e1cb0ccb8aaf36bccbb3fc574d2/M3Formulas/_FoldingTetra3d.m3f

<?php
require_once(dirname(__FILE__) . '/common.inc.php');
printStart();
$retDecBinPath = '~/Downloads/retdec/bin'; // from here: https://github.com/avast/retdec
$bin2hexPath = '~/Downloads/intelhex-master/scripts'; // from here https://github.com/bialix/intelhex

$fileName = @$argv[1];
$contents = file_get_contents($fileName);
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

printFinish();
exit;

?>

