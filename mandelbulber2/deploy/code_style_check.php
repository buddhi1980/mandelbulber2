#!/usr/bin/env php
#
# this file checks the source and header files
# 
# on default this script runs dry, 
# it will try to parse all files and prints problems inside the files
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to the project files
#

<?php
define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');

$filesToCheckSource = array();
// $filesToCheckSource[] = PROJECT_PATH . "src/*.cpp";
// $filesToCheckSource[] = PROJECT_PATH . "src/*.c";
$filesToCheckSource[] = PROJECT_PATH . "qt/*.cpp";
$filesToCheckSource[] = PROJECT_PATH . "qt/*.c";

$filesToCheckHeader = array();
// $filesToCheckHeader[] = PROJECT_PATH . "src/*.hpp";
// $filesToCheckHeader[] = PROJECT_PATH . "src/*.h";
$filesToCheckHeader[] = PROJECT_PATH . "qt/*.hpp";
$filesToCheckHeader[] = PROJECT_PATH . "qt/*.h";

$sourceFiles = glob("{" . implode(",", $filesToCheckSource) . "}", GLOB_BRACE);
$headerFiles = glob("{" . implode(",", $filesToCheckHeader) . "}", GLOB_BRACE);

foreach($headerFiles as $headerFilePath) {
	$headerFileName = basename($headerFilePath);
	if(substr($headerFileName, 0, strlen('ui_')) == 'ui_') continue;
	$folderName = basename(str_replace($headerFileName, '', $headerFilePath));
	echo 'handling file: ' . $headerFileName;
	$headerContent = file_get_contents($headerFilePath);
	if(!checkFileHeader($headerContent)) continue;
	if(!checkDefines($headerContent, $headerFilePath, $headerFileName, $folderName)) continue;
	if(!isDryRun()){
		file_put_contents($headerFilePath, $headerContent);	
	}
}

if(isDryRun()){
	echo 'This is a dry run.' . PHP_EOL;
	echo 'If you want to apply the changes, execute with populateUiInformation.php nondry' . PHP_EOL;
}
else{
	echo 'Changes applied' . PHP_EOL;
}

exit;



function checkFileHeader(&$fileContent){
	$headerRegex = '/^(\/\*\*[\s\S]*?\*\/)([\s\S]*)$/';
	if(preg_match($headerRegex, $fileContent, $matchHeader)){
		$functionContentFound = true;
		$fileHeader = $matchHeader[1];
		// $fileSourceCode = $matchHeader[2];
		if(strpos($fileHeader, 'MKNmMMKmm') === false){
			echo errorString('header unknown!') . PHP_EOL;
		}
		else{
			return true;	
		}
	}
	else{
		echo errorString('No header found!') . PHP_EOL;
	}
	return false;
}

function checkDefines(&$fileContent, $headerFilePath, $headerFileName, $folderName){
	$defineRegex = '/^([\s\S]*?#ifndef\s)([\s\S]+?)(\n#define\s)(\S+)([\S\s]+)(#endif\s)[\s\S]+$/';
	if(preg_match($defineRegex, $fileContent, $match)){
		$defineName = 'MANDELBULBER2_' . strtoupper($folderName) . '_' . strtoupper(str_replace('.', '_', $headerFileName)) . '_';
		$newFileContent = $match[1] . $defineName . $match[3] . $defineName . $match[5] . $match[6] . '/* ' . $defineName . ' */' . PHP_EOL;

		if($newFileContent != $fileContent){
			$fileContent = $newFileContent;
			echo noticeString('makro changed') . PHP_EOL;
		}
		else{
			echo successString('All well') . PHP_EOL;
		}
		return true;
	}
	else{
		echo errorString('makro not found!') . PHP_EOL;
	}
	return false;
}

function errorString($s){
	return "\033[0;31m\033[47m" . $s . "\033[0m";
}

function successString($s){
	return "\033[0;32m\033[47m" . $s . "\033[0m";
}

function noticeString($s){
	return "\033[0;34m\033[47m" . $s . "\033[0m";
}

function isDryRun(){
	global $argv;
	if(count($argv) > 1 && $argv[1] == 'nondry'){
		return false;
	}
	return true;
}

?>

