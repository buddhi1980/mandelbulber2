#!/usr/bin/env php
#
# this file updates release relevant meta things:
# - updating READMEs with current cli --help output
# - updating READMEs with contributors
# - updating about Mandelbulber popup with contributors
#
# requires php (apt-get install php5-cli) and the current mandelbulber
# executable in pathToCurrentMandelbulberExec (set locale to english)
#

<?php
define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');
define('pathToCurrentMandelbulberExec', PROJECT_PATH . '/Debug/mandelbulber2');

$newsContent = file_get_contents(PROJECT_PATH .'deploy/NEWS');
$readmeContent = file_get_contents(PROJECT_PATH .'deploy/README');
$readmeOSXContent = file_get_contents(PROJECT_PATH .'deploy/README-osx.txt');
$readmeWINContent = file_get_contents(PROJECT_PATH .'deploy/README-win32.txt');
// $contributorsContent = file_get_contents(PROJECT_PATH .'contributors.txt');

// help output to READMEs
$helpOutput = getHelpOutput();
$helpOutput = substr($helpOutput, strpos($helpOutput, 'Options:') + strlen('Options:'));
$readmeContent = substr($readmeContent, 0, strpos($readmeContent, 'Options:') + strlen('Options:'));
$readmeContent .= $helpOutput;
$readmeOSXContent = substr($readmeOSXContent, 0, strpos($readmeOSXContent, 'Options:') + strlen('Options:'));
$readmeOSXContent .= $helpOutput;
$readmeWINContent = substr($readmeWINContent, 0, strpos($readmeWINContent, 'Options:') + strlen('Options:'));
$readmeWINContent .= $helpOutput;

// contributors to README
// TODO

// contributors to about Mandelbulber popup
// TODO

file_put_contents(PROJECT_PATH .'deploy/README', $readmeContent);
file_put_contents(PROJECT_PATH .'deploy/README-osx.txt', $readmeOSXContent);
file_put_contents(PROJECT_PATH .'deploy/README-win32.txt', $readmeWINContent);

echo successString('meta updated.') . PHP_EOL;

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

function getHelpOutput(){
	$cmd = pathToCurrentMandelbulberExec . ' --help';
	$out = shell_exec($cmd);
	return $out;
}

?>
