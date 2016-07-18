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

foreach($sourceFiles as $sourceFilePath) {
	$sourceFileName = basename($sourceFilePath);
	echo 'handling file: ' . $sourceFileName;
	$sourceContent = file_get_contents($sourceFilePath);
	if(!checkFileHeader($sourceFilePath, $sourceContent)) continue;
	if(!checkClang($headerContent)) continue;

	if(!isDryRun()){
		file_put_contents($sourceFilePath, $sourceContent);
	}
	echo successString(' -> All well') . PHP_EOL;
}

foreach($headerFiles as $headerFilePath) {
	$headerFileName = basename($headerFilePath);
	if(substr($headerFileName, 0, strlen('ui_')) == 'ui_') continue;
	$folderName = basename(str_replace($headerFileName, '', $headerFilePath));
	echo 'handling file: ' . $headerFileName;
	$headerContent = file_get_contents($headerFilePath);
	if(!checkFileHeader($headerFilePath, $headerContent)) continue;
	if(!checkDefines($headerContent, $headerFilePath, $headerFileName, $folderName)) continue;
	if(!checkClang($headerContent)) continue;
	if(!isDryRun()){
		file_put_contents($headerFilePath, $headerContent);
	}
	echo successString(' -> All well') . PHP_EOL;
}

if(isDryRun()){
	echo 'This is a dry run.' . PHP_EOL;
	echo 'If you want to apply the changes, execute with populateUiInformation.php nondry' . PHP_EOL;
}
else{
	echo 'Changes applied' . PHP_EOL;
}

exit;



function checkFileHeader($filePath, &$fileContent){
	$headerRegex = '/^(\/\*\*[\s\S]*?\*\/)([\s\S]*)$/';
	if(preg_match($headerRegex, $fileContent, $matchHeader)){
		$functionContentFound = true;
		$fileHeader = $matchHeader[1];
		// $fileSourceCode = $matchHeader[2];
		$modificationString = getModificationInterval($filePath);

		if(strpos($fileHeader, 'MKNmMMKmm') === false){
			$regexParseHeader = '/^[\s\S]+Mandelbulber\sv2.*[\s\S]*?(\w.*)[\S\s]+';
			$regexParseHeader .= 'General\sPublic[\S\s]+';
			$regexParseHeader .= 'Authors:\s(.*)[\s\S]*?\*\/([\s\S]*)$/';
			if(preg_match($regexParseHeader, $fileContent, $matchHeaderOld)){
				echo noticeString('header is old, will rewrite to new!') . PHP_EOL;
				$newFileContent = getFileHeader($matchHeaderOld[2], $matchHeaderOld[1], $modificationString) . $matchHeaderOld[3];
				$fileContent = $newFileContent;
				return true;
			}
			else{
				echo errorString('header unknown!') . PHP_EOL;
			}
		}
		else{
			$regexParseHeader = '/^[\s\S]+#{50}?[\S\s]+Authors:\s(.*)([\s\S]*?)\*\/([\s\S]*)$/';
			if(preg_match($regexParseHeader, $fileContent, $matchHeaderNew)){
				echo noticeString('header is new, will rewrite to new!') . PHP_EOL;
				$newFileContent = getFileHeader($matchHeaderNew[1], $matchHeaderNew[2], $modificationString) . $matchHeaderNew[3];
				$fileContent = $newFileContent;
				return true;
			}
			else{
				echo errorString('header unknown!') . PHP_EOL;
			}
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
			echo noticeString('define changed') . PHP_EOL;
		}
		return true;
	}
	else{
		echo errorString('define not found!') . PHP_EOL;
	}
	return false;
}

function checkClang(&$fileContent){
	$cmd = "echo " . escapeshellarg($fileContent);
	$cmd .= " | clang-format --style=file";
	$clangFormattedFileContent =  shell_exec($cmd);
	if($clangFormattedFileContent == ''){
		errorString('clang-format returned empty string, not installed?');
		return false;
	}
	if($fileContent != $clangFormattedFileContent){
		echo noticeString('checkClang changed') . PHP_EOL;
	}
	$fileContent = $clangFormattedFileContent;
	return true;
}

function getModificationInterval($filePath){
	$cmd = "git log --format=%ad " . $filePath . " | tail -1 | egrep -o '\s([0-9]{4})\s'";
	$yearStart = trim(shell_exec($cmd));
	$cmd = "git log --format=%ad " . $filePath . " | head -1 | egrep -o '\s([0-9]{4})\s'";
	$yearEnd =  trim(shell_exec($cmd));
	if($yearStart == $yearEnd){
		return $yearStart;
	}
	return $yearStart . '-' . substr($yearEnd, 2, 2);
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

function getFileHeader($author, $description, $modificationString){
	$out = <<<EOT
/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) __date__ Krzysztof Marczak __spacing__ §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: __author__
 *
 * __description__
 */
EOT;
	$lines = explode(PHP_EOL, $description);
	$nonEmptyLines = array();
	$firstFound = false;
	foreach($lines as $line){
		$l = trim($line);
		if(!$firstFound && ($l == '*' || empty($l))) continue;
		$regexCommentStart = '/^\*\s*(.*)$/';
		if(preg_match($regexCommentStart, $l, $match)){
			$l = $match[1];
		}
		$nonEmptyLines[] = rtrim(($firstFound ? ' * ' : '') . $l);
		$firstFound = true;
	}
	unset($nonEmptyLines[count($nonEmptyLines)-1]);

	$description = implode(PHP_EOL, $nonEmptyLines);
	$spacing = str_repeat(' ', 10 - strlen($modificationString));
	return str_replace(array('__author__', '__description__', '__date__', '__spacing__'),
		array($author, $description, $modificationString, $spacing), $out);
}

?>

