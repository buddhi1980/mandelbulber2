#!/usr/bin/env php
#
# this file checks the source and header files
# requires clang-format, git and php (apt-get install clang-format php5-cli)
#
# on default this script runs dry,
# it will try to parse all files and prints problems inside the files
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to the project files
#

<?php
define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');

function printStart(){
	echo 'Processing...' . PHP_EOL . PHP_EOL;
}

function printFinish(){
	if(isDryRun()){
		echo 'This is a dry run.' . PHP_EOL;
		echo 'If you want to apply the changes, execute with argument "nondry"' . PHP_EOL;
	}
	else{
		echo 'Changes applied' . PHP_EOL;
	}
}

function printResultLine($name, $success, $status){
	$out = str_pad('> ' . $name, 30);
	if($success && !isVerbose() && count($status) == 0) return;
	if($success)
	{
		echo $out . successString(' -> All Well') . PHP_EOL;
	}
	else{
		echo $out . errorString(' -> Error') . PHP_EOL;
	}
	if(count($status) > 0){
		echo ' |-' . implode(PHP_EOL . ' |-', $status) . PHP_EOL;
	}
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
function warningString($s){
	return "\033[0;33m\033[47m" . $s . "\033[0m";
}

function isDryRun(){
	global $argv;
	if(count($argv) > 1 && in_array('nondry', $argv)){
		return false;
	}
	return true;
}

function isVerbose(){
	global $argv;
	if(count($argv) > 1 && in_array('verbose', $argv)){
		return true;
	}
	return false;
}

function isWarning(){
	global $argv;
	if(count($argv) > 1 && (in_array('verbose', $argv) || in_array('warning', $argv))){
		return true;
	}
	return false;
}
?>
