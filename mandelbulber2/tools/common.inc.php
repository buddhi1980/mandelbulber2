<?php

/* This file contains common logic needed in the php scripts */

define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');
checkArguments();

function printStart()
{
	echo 'Processing...' . PHP_EOL;
}

function printFinish()
{
	if (isDryRun()) {
		echo 'This is a dry run.' . PHP_EOL;
		echo 'If you want to apply the changes, execute with argument "nondry"' . PHP_EOL;
	} else {
		echo 'Changes applied' . PHP_EOL;
	}
}

function printResultLine($name, $success, $status)
{
	$out = str_pad(' ├── ' . $name, 50);
	if (!($success && !isVerbose() && count($status) == 0))
	{ 
		if ($success) {
			echo $out . successString(' [ All OK ]') . PHP_EOL;
		} else {
			echo $out .   errorString(' [ ERROR  ]') . PHP_EOL;
		}
		if (count($status) > 0) {
			foreach($status as $i => $s){
				$treeStr = ($i == count($status) - 1) ? ' │   ╰── ' : ' │   ├── ';
				echo $treeStr . $s . PHP_EOL;
			}
		}
	}
	echo "\33[2Khandled: $name\r";
}

function printStartGroup($title)
{
	echo PHP_EOL . "\033[1m\033[44m" . ' ' . str_pad($title, 54) . ' ' . "\033[0m" . PHP_EOL;
}

function printEndGroup()
{
	echo "\33[2K";
	echo ' ╹' . PHP_EOL;
}

function errorString($s)
{
	return "\033[1;31m" . $s . "\033[0m";
}

function successString($s)
{
	return "\033[1;32m" . $s . "\033[0m";
}

function noticeString($s)
{
	return "\033[1;34m" . $s . "\033[0m";
}

function warningString($s)
{
	return "\033[1;33m" . $s . "\033[0m";
}

function isDryRun()
{
	return !argumentContains('nondry');
}

function isVerbose()
{
	return argumentContains('verbose');
}

function isWarning()
{
	return argumentContains('verbose') || argumentContains('warning');
}

function argumentContains($needle)
{
	global $argv;
	if (count($argv) > 1 && in_array($needle, $argv)) {
		return true;
	}
	return false;
}

function checkArguments(){
	global $argv;
	foreach($argv as $i => $arg){
		if($i == 0) continue;
		if(!in_array($arg, array('nondry', 'verbose', 'warning')))
			die('Unknown argument: ' . $arg . PHP_EOL);
	}
}

?>
