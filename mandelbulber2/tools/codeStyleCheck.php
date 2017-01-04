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
define('WILDCARD', '*');

$filesToCheckSource = array();
$filesToCheckSource[] = PROJECT_PATH . "src/" . WILDCARD . ".cpp";
$filesToCheckSource[] = PROJECT_PATH . "src/" . WILDCARD . ".c";
$filesToCheckSource[] = PROJECT_PATH . "qt/" . WILDCARD . ".cpp";
$filesToCheckSource[] = PROJECT_PATH . "qt/" . WILDCARD . ".c";

$filesToCheckHeader = array();
$filesToCheckHeader[] = PROJECT_PATH . "src/" . WILDCARD . ".hpp";
$filesToCheckHeader[] = PROJECT_PATH . "src/" . WILDCARD . ".h";
$filesToCheckHeader[] = PROJECT_PATH . "qt/" . WILDCARD . ".hpp";
$filesToCheckHeader[] = PROJECT_PATH . "qt/" . WILDCARD . ".h";

$sourceFiles = glob("{" . implode(",", $filesToCheckSource) . "}", GLOB_BRACE);
$headerFiles = glob("{" . implode(",", $filesToCheckHeader) . "}", GLOB_BRACE);

echo 'Processing...' . PHP_EOL . PHP_EOL;

foreach($sourceFiles as $sourceFilePath) {
	$sourceFileName = basename($sourceFilePath);
	$status = array();
	$success = false;
	$sourceContent = file_get_contents($sourceFilePath);
	if(checkFileHeader($sourceFilePath, $sourceContent, $status)){
		if(checkClang($sourceFilePath, $sourceContent, $status)){
			if(!isDryRun()){
				file_put_contents($sourceFilePath, $sourceContent);
			}
			$success = true;
		}		
	}
	printResultLine($sourceFileName, $success, $status);
}

foreach($headerFiles as $headerFilePath) {
	$headerFileName = basename($headerFilePath);
	if(substr($headerFileName, 0, strlen('ui_')) == 'ui_') continue;
	$folderName = basename(str_replace($headerFileName, '', $headerFilePath));
	$status = array();
	$success = false;
	$headerContent = file_get_contents($headerFilePath);
	if(checkFileHeader($headerFilePath, $headerContent, $status)) {
		if(checkDefines($headerContent, $headerFilePath, $headerFileName, $folderName, $status)){
			if(checkClang($headerFilePath, $headerContent, $status)){
				if(!isDryRun()){
					file_put_contents($headerFilePath, $headerContent);
				}
				$success = true;
			}
		}
	}
	printResultLine($headerFileName, $success, $status);
}

if(isDryRun()){
	echo 'This is a dry run.' . PHP_EOL;
	echo 'If you want to apply the changes, execute with codeStyleCheck.php nondry' . PHP_EOL;
}
else{
	echo 'Changes applied' . PHP_EOL;
}

exit;

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

function checkFileHeader($filePath, &$fileContent, &$status){
        $headerRegex = '/^(\/\*\*?[\s\S]*?\*\/)([\s\S]*)$/';
	if(preg_match($headerRegex, $fileContent, $matchHeader)){
		$functionContentFound = true;
		$fileHeader = $matchHeader[1];
		// $fileSourceCode = $matchHeader[2];
		$modificationString = getModificationInterval($filePath);

		if(strpos($fileHeader, 'MKNmMMKmm') === false){
			// not a recent proper file header, try other known formats
			// old format
			$regexParseHeaderOld = '/^[\s\S]+Mandelbulber\sv2.*[\s\S]*?(\w.*)[\S\s]+';
			$regexParseHeaderOld .= 'General\sPublic[\S\s]+';
			$regexParseHeaderOld .= 'Authors:\s(.*)[\s\S]*?\*\/([\s\S]*)$/';
                        $regexParseHeaderEclipse = '/^[\s\S]+Author:[\s\S]*?\*\/([\s\S]*)$/';
			$regexParseHeaderAny = '/^\/\*\*?([\s\S]*?)\*\/([\s\S]*)$/';
		
			if(preg_match($regexParseHeaderOld, $fileContent, $matchHeaderOld)){
				$status[] = noticeString('header is old, will rewrite to new!');
				$newFileContent = getFileHeader($matchHeaderOld[2], $matchHeaderOld[1], $modificationString) . $matchHeaderOld[3];
				$fileContent = $newFileContent;
				return true;
			}
			else if(preg_match($regexParseHeaderEclipse, $fileContent, $matchHeaderEclipse)){
                            $status[] = noticeString('header is in eclipse format, will rewrite to new!');
                            $newFileContent = getFileHeader('Krzysztof Marczak (buddhi1980@gmail.com)', 'TODO: description', date('Y')) . $matchHeaderEclipse[1];
                            $fileContent = $newFileContent;
                            return true;                           
			}
			else if(preg_match($regexParseHeaderAny, $fileContent, $matchHeaderAny)){
                            $status[] = noticeString('header is unknown format, just copying contents of header!');
                            $newFileContent = getFileHeader('### TODO: unknown author! ###', $matchHeaderAny[1], date('Y')) . $matchHeaderAny[2];
                            $fileContent = $newFileContent;
                            return true;                           
			}
			else{
                            $status[] = errorString('header unknown!');
       		        }
		}
		else{
			$regexParseHeader = '/^[\s\S]+#{50}?[\S\s]+Authors:\s([\s\S]*?)\*\n([\s\S]*?)\*\/([\s\S]*)$/';
			if(preg_match($regexParseHeader, $fileContent, $matchHeaderNew)){
				$newFileContent = getFileHeader($matchHeaderNew[1], $matchHeaderNew[2], $modificationString) . $matchHeaderNew[3];
				if($newFileContent != $fileContent){
					$status[] = noticeString('header is new, will rewrite to new!');
					$fileContent = $newFileContent;
				}
				return true;
			}
			else{
                                $status[] = errorString('header unknown!');
			}
		}

	}
	else{
		$status[] = errorString('No header found!');
	}
	return false;
}

function checkDefines(&$fileContent, $headerFilePath, $headerFileName, $folderName, &$status){
	$defineRegex = '/^([\s\S]*?#ifndef\s)([\s\S]+?)(\n#define\s)(\S+)([\S\s]+)(#endif\s)[\s\S]+$/';
	if(preg_match($defineRegex, $fileContent, $match)){
		$defineName = 'MANDELBULBER2_' . strtoupper($folderName) . '_' . strtoupper(str_replace('.', '_', $headerFileName)) . '_';
		$newFileContent = $match[1] . $defineName . $match[3] . $defineName . $match[5] . $match[6] . '/* ' . $defineName . ' */' . PHP_EOL;

		if($newFileContent != $fileContent){
			$fileContent = $newFileContent;
			$status[] = noticeString('define changed');
		}
		return true;
	}
	else{
		$status[] = errorString('define not found!');
	}
	return false;
}

function checkClang($filepath, &$fileContent, &$status){
  $contentsBefore = $fileContent;
	// method 1
	// $cmd = 'cat <<EOF |' . PHP_EOL . escapeshellarg($fileContent) . PHP_EOL . 'EOF' . PHP_EOL;
        // $cmd = 'echo ' . escapeshellarg($fileContent) . ' | ';
        // $cmd .= "clang-format --style=file --assume-filename=" . escapeshellarg($filePath);
        
	// method 2
	// $cmd = 'cat <<EOF | ' . PHP_EOL . ($fileContent) . PHP_EOL . 'EOF' . PHP_EOL;
	// $cmd .= "clang-format";
	// $fileContent = shell_exec($cmd);
        
	// method 3
	$filepathTemp = $filepath . '.tmp.c';
	file_put_contents($filepathTemp, $fileContent);
	shell_exec('clang-format -i --style=file ' . escapeshellarg($filepathTemp));
	$fileContent = file_get_contents($filepathTemp);
	unlink($filepathTemp); // nothing to see here :)

        // $fileContent = str_replace('// forward declarations', '//forward declarations', $fileContent);

	// echo $fileContent; exit;
	if($contentsBefore != $fileContent){
		$status[] = noticeString('checkClang changed');
	}
	return true;
}

function getModificationInterval($filePath){
	// these commits contain only auto formatting code changes
	// and should not be counted for the modification invertal
	$ignoreString = 'source\scode|nullptr';

	$cmd = "git log --format=%ad%s " . $filePath . " | grep -vE '" . $ignoreString . "' | tail -1 | egrep -o '\s([0-9]{4})\s'";
	$yearStart = trim(shell_exec($cmd));
	$cmd = "git log --format=%ad%s " . $filePath . " | grep -m 1 -vE '" . $ignoreString . "' | head -1 | egrep -o '\s([0-9]{4})\s'";
	$yearEnd =  trim(shell_exec($cmd));
	if(empty($yearStart)) return date('Y');	
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

function getFileHeader($author, $description, $modificationString){
	$out = <<<EOT
/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) __date__ Mandelbulber Team __spacing__ §R-==%w["'~5]m%=L.=~5N
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
	if($nonEmptyLines[count($nonEmptyLines)-1] == ' *'){
		unset($nonEmptyLines[count($nonEmptyLines)-1]);
	}

	$description = implode(PHP_EOL, $nonEmptyLines);
	$spacing = str_repeat(' ', 10 - strlen($modificationString));
	return str_replace(array('__author__', '__description__', '__date__', '__spacing__'),
		array(formatAuthorLine($author), $description, $modificationString, $spacing), $out);
}

function formatAuthorLine($authorLine){
	$oldAuthors = explode(',', $authorLine);
	$authors = array();
	$out = '';
	foreach($oldAuthors as $oldAuthor){
		$name = trim($oldAuthor);
		if($name == '') continue;
		$author = lookUpAuthor($name);
		if(strlen($out) > 0) $out .= ', ';
		if(strlen($out) - strrpos($out, PHP_EOL) + strlen($author) > 90) $out .= PHP_EOL . ' *  ';
		$out .=	$author; 
	}
	return $out;
}

function lookUpAuthor($authorName){
	// use consistent author name
	if(strpos($authorName, 'Krzysztof Marczak') !== false) return 'Krzysztof Marczak (buddhi1980@gmail.com)';
	if(strpos($authorName, 'Graeme McLaren') !== false) return 'Graeme McLaren';
	if(strpos($authorName, 'pmneila') !== false) return 'pmneila';
	if(strpos($authorName, 'Stanislaw Adaszewski') !== false) return 'Stanislaw Adaszewski (http://algoholic.eu)';
	if(strpos($authorName, 'Rayan Hitchman') !== false) return 'Rayan Hitchman';
	if(strpos($authorName, 'Robert Pancoast') !== false) return 'Robert Pancoast (RobertPancoast77@gmail.com)';
	if(strpos($authorName, 'Sebastian Jennen') !== false) return 'Sebastian Jennen (jenzebas@gmail.com)';

	errorString('unknown author: ' . $authorName);
	return $authorName;
}

?>

