<?php

/* This file contains common logic needed in the php scripts */

define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');

function printStart()
{
	echo 'Processing...' . PHP_EOL . PHP_EOL;
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
	$out = str_pad('> ' . $name, 30);
	if ($success && !isVerbose() && count($status) == 0) return;
	if ($success) {
		echo $out . successString(' -> All Well') . PHP_EOL;
	} else {
		echo $out . errorString(' -> Error') . PHP_EOL;
	}
	if (count($status) > 0) {
		echo ' |-' . implode(PHP_EOL . ' |-', $status) . PHP_EOL;
	}
}

function errorString($s)
{
	return "\033[0;31m\033[47m" . $s . "\033[0m";
}

function successString($s)
{
	return "\033[0;32m\033[47m" . $s . "\033[0m";
}

function noticeString($s)
{
	return "\033[0;34m\033[47m" . $s . "\033[0m";
}

function warningString($s)
{
	return "\033[0;33m\033[47m" . $s . "\033[0m";
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

?>
