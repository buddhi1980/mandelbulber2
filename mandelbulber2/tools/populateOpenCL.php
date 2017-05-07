#!/usr/bin/env php
#
# this file autogenerates misc files from cpp to opencl
#
# requires php (apt-get install php5-cli)
# 
# on default this script runs dry, 
# it will try to generate the needed files and show which files would be modified
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write the changes
# to the opencl files
#

<?php
require_once(dirname(__FILE__) . '/common.inc.php');

printStart();

$copyFiles['fractal_h']['path'] = PROJECT_PATH . 'src/fractal.h';
$copyFiles['fractal_h']['pathTarget'] = PROJECT_PATH . 'opencl/fractalCl.h';

foreach($copyFiles as $type => $copyFile){
	$oldContent = file_get_contents($copyFile['pathTarget']);
	$content = file_get_contents($copyFile['path']);

	// replace opencl specific tokens
	$openCLReplaceLookup = array(
	    array('find' => '/(\s)int(\s)/', 'replace' => '$1cl_int$2'),
	    array('find' => '/(\s)bool(\s)/', 'replace' => '$1cl_int$2'),
	    array('find' => '/(\s)double(\s)/', 'replace' => '$1cl_float$2'),
	    array('find' => '/(\s)CVector3(\s)/', 'replace' => '$1cl_float3$2'),
	    array('find' => '/(\s)CVector4(\s)/', 'replace' => '$1cl_float4$2'),
	    array('find' => '/(\s)CRotationMatrix(\s)/', 'replace' => '$1matrix33$2'),
	    array('find' => '/struct\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})/', 'replace' => "typedef struct\n$2$3 sCl$1"),
	    array('find' => '/enum\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})/', 'replace' => "typedef enum\n$2$3 eCl$1"),
	);
	foreach($openCLReplaceLookup as $item){
		$content = preg_replace($item['find'], $item['replace'], $content);
	}

	// clang-format
	$filepathTemp = $copyFile['path'] . '.tmp.c';
	file_put_contents($filepathTemp, $content);
	shell_exec('clang-format -i --style=file ' . escapeshellarg($filepathTemp));
	$content = file_get_contents($filepathTemp);
	unlink($filepathTemp); // nothing to see here :)
	
	if($content != $oldContent){
		if(!isDryRun()){
			file_put_contents($copyFile['pathTarget'], $content);
		}
		echo successString('file ' . $copyFile['pathTarget'] . ' changed.') . PHP_EOL;
	}else{
		if(isVerbose()){
			echo noticeString('file ' . $copyFile['pathTarget'] . ' has not changed.') . PHP_EOL;
		}
	}
}

printFinish();
exit;

?>

