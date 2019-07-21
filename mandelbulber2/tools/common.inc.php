<?php

/* This file contains common logic needed in the php scripts */

define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');
define('START_TIME', microtime(true));

if (file_exists(realpath(dirname(__FILE__)) . '/customConfig.php'))
	require_once(realpath(dirname(__FILE__)) . '/customConfig.php');

if (!defined('MANDELBULBER_EXEC_PATH')) define('MANDELBULBER_EXEC_PATH', 'mandelbulber2');
if (!defined('CLANG_FORMAT_EXEC_PATH')) define('CLANG_FORMAT_EXEC_PATH', 'clang-format');
if(!defined('RUN_CLANG_TIDY_EXEC_PATH')) define('RUN_CLANG_TIDY_EXEC_PATH', '/usr/bin/run-clang-tidy-6.0.py');
if(!defined('CLANG_APPLY_BINARY_PATH')) define('CLANG_APPLY_BINARY_PATH', '/usr/bin/clang-apply-replacements-6.0');

checkArguments();

function printStart()
{
	echo 'Processing...' . PHP_EOL;
}

function printFinish()
{
	echo "\33[2K\r";
	$secString = noticeString(number_format(microtime(true) - START_TIME, 2) . ' seconds');
	echo 'script took ' . $secString . ' to complete.' . PHP_EOL;
	if (isDryRun()) {
		echo 'This is a dry run.' . PHP_EOL;
		echo 'If you want to apply the changes, execute with argument "nondry"' . PHP_EOL;
	} else {
		echo 'Changes applied' . PHP_EOL;
	}
}

function printResultLine($name, $success, $status, $progress = -1)
{
	$out = "\33[2K\r";
	$itemLine = str_pad(' ├── ' . $name, 50);
	if (!($success && !isVerbose() && count($status) == 0)) {
		if ($success) {
			$out .= $itemLine . successString(' [ All OK ]') . PHP_EOL;
		} else {
			$out .= $itemLine . errorString(' [ ERROR  ]') . PHP_EOL;
		}
		if (count($status) > 0) {
			foreach ($status as $i => $s) {
				$treeStr = ($i == count($status) - 1) ? ' │   ╰── ' : ' │   ├── ';
				$out .= $treeStr . $s . PHP_EOL;
			}
		}
	}
	$out .= printProgress($progress) . " handled: $name";
	echo $out;
}

function printProgress($percent)
{
	if ($percent < 0) return '';
	$freeWidth = 20;
	$intProgress = floor($freeWidth * $percent);

	$text = noticeString('[');
	$text .= progressString(str_repeat('#', $intProgress));
	$text .= str_repeat(' ', $freeWidth - $intProgress);
	$text .= '/ ' . progressString(str_pad(number_format($percent * 100, 1), 4, ' ', STR_PAD_LEFT) . '%');
	$text .= noticeString(']');
	return $text;
}

function printStartGroup($title)
{
	echo PHP_EOL . "\033[1m\033[44m" . ' ' . str_pad($title, 54) . ' ' . "\033[0m" . PHP_EOL;
}

function printEndGroup()
{
	echo "\33[2K\r";
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

function progressString($s)
{
	return "\033[1;35m" . $s . "\033[0m";
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

function checkArguments()
{
	global $argv;
	foreach ($argv as $i => $arg) {
		if ($i == 0) continue;
		if (!in_array($arg, array('nondry', 'verbose', 'warning', 'checkCl', 'checkTidy'))){
			if($argv[0] != 'portMandelbulb3dFormula.php')
				die('Unknown argument: ' . $arg . PHP_EOL);
		}
	}
}

function getModificationIntervals()
{
	$modificationFileInfo = array();
	$cmd = "git log --format='COMMIT_SEPARATOR_1____________%adCOMMIT_SEPARATOR_2____________%s' --name-only";
	$commitStringRaw = trim(shell_exec($cmd));
	$commitStrings = explode('COMMIT_SEPARATOR_1____________', $commitStringRaw);
	foreach ($commitStrings as $commitString) {
		if (empty($commitString)) continue;
		$commitLines = explode(PHP_EOL, $commitString);
		$meta = explode('COMMIT_SEPARATOR_2____________', $commitLines[0]);
		$date = substr($meta[0], -10, 4);
		$title = $meta[1];

		// these commits contain only auto formatting code changes
		// and should not be counted for the modification invertal
		if (strpos($title, 'source code') !== false) continue;
		if (strpos($title, 'nullptr') !== false) continue;
		if (strpos($title, 'remove src dir') !== false) continue;

		foreach ($commitLines as $commitLine) {
			if (!empty($commitLine) && strpos($commitLine, 'COMMIT_SEPARATOR_2____________') === false) {
				$modificationFileInfo[$commitLine]['start'] =
					min(empty($modificationFileInfo[$commitLine]['start']) ? 10000 : $modificationFileInfo[$commitLine]['start'], $date);
				$modificationFileInfo[$commitLine]['end'] = max(@$modificationFileInfo[$commitLine]['end'], $date);
			}
		}
	}
	return $modificationFileInfo;
}

function getModificationInterval($filePath, $onlyEndYear = false)
{
	global $modificationIntervals;
	if (empty($modificationIntervals)) $modificationIntervals = getModificationIntervals();

	$filePathRelative = str_replace(PROJECT_PATH, 'mandelbulber2/', $filePath);
	if (array_key_exists($filePathRelative, $modificationIntervals)) {
		$yearStart = $modificationIntervals[$filePathRelative]['start'];
		$yearEnd = $modificationIntervals[$filePathRelative]['end'];
	}
	if (empty($yearStart)) return date('Y');
	if (empty($yearEnd) || $yearStart == $yearEnd) {
		return $yearStart;
	}
	if ($onlyEndYear) return $yearEnd;
	return $yearStart . '-' . substr($yearEnd, 2, 2);
}


?>
