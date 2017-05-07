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
require_once(dirname(__FILE__) . '/common.inc.php');

printStart();

define('pathToCurrentMandelbulberExec', PROJECT_PATH . '/Debug/mandelbulber2');

$newsContent = file_get_contents(PROJECT_PATH .'deploy/NEWS');

// $contributorsContent = file_get_contents(PROJECT_PATH .'contributors.txt');

// help output to READMEs
$helpOutput = getHelpOutput();
$helpOutput = substr($helpOutput, strpos($helpOutput, 'Options:') + strlen('Options:'));

$readmes['linux']['path'] = PROJECT_PATH .'deploy/README';
$readmes['osx']['path'] = PROJECT_PATH .'deploy/README-osx.txt';
$readmes['win']['path'] = PROJECT_PATH .'deploy/README-win32.txt';

foreach($readmes as $type => $readme){
	$oldContent = file_get_contents($readme['path']);
	$content = $oldContent;
	$content = substr($content, 0, strpos($content, 'Options:') + strlen('Options:'));
	$content .= $helpOutput;

	if($content != $oldContent){
		if(!isDryRun()){
			file_put_contents($readme['path'], $content);
		}
		echo successString('readme for ' . $readme['path'] . ' changed.') . PHP_EOL;
	}else{
		if(isVerbose()){
			echo noticeString('readme for ' . $readme['path'] . ' has not changed.') . PHP_EOL;
		}
	}
}

printFinish();
exit;

// contributors to README
// TODO

// contributors to about Mandelbulber popup
// TODO

function getHelpOutput(){
	$cmd = pathToCurrentMandelbulberExec . ' --help';
	$out = shell_exec($cmd);
	return $out;
}

?>
