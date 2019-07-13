#!/usr/bin/env php
#
# this file checks the source and header files
# requires clang-format, git and php (apt-get install clang-format php5-cli git)
# clang-format is required in version 3.8.1, get executable from here: http://releases.llvm.org/download.html
# highlight package is required in version 3.39
#
# on default this script runs dry,
# it will try to parse all files and prints problems inside the files
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to the project files
#

<?php
require_once(dirname(__FILE__) . '/common.inc.php');

printStart();

define('WILDCARD', '*');

$filesToCheckSource = array();
$filesToCheckSource[] = PROJECT_PATH . "src/" . WILDCARD . ".cpp";
$filesToCheckSource[] = PROJECT_PATH . "src/" . WILDCARD . ".c";
$filesToCheckSource[] = PROJECT_PATH . "qt/" . WILDCARD . ".cpp";
$filesToCheckSource[] = PROJECT_PATH . "qt/" . WILDCARD . ".c";
$filesToCheckSource[] = PROJECT_PATH . "opencl/" . WILDCARD . ".cpp";
$filesToCheckSource[] = PROJECT_PATH . "opencl/" . WILDCARD . ".c";
$filesToCheckSource[] = PROJECT_PATH . "opencl/engines/" . WILDCARD . ".cl";

$filesToCheckHeader = array();
$filesToCheckHeader[] = PROJECT_PATH . "src/" . WILDCARD . ".hpp";
$filesToCheckHeader[] = PROJECT_PATH . "src/" . WILDCARD . ".h";
$filesToCheckHeader[] = PROJECT_PATH . "qt/" . WILDCARD . ".hpp";
$filesToCheckHeader[] = PROJECT_PATH . "qt/" . WILDCARD . ".h";
$filesToCheckSource[] = PROJECT_PATH . "opencl/" . WILDCARD . ".hpp";
$filesToCheckSource[] = PROJECT_PATH . "opencl/" . WILDCARD . ".h";

$sourceFiles = glob("{" . implode(",", $filesToCheckSource) . "}", GLOB_BRACE);
$headerFiles = glob("{" . implode(",", $filesToCheckHeader) . "}", GLOB_BRACE);

printStartGroup('RUNNING CHECKS ON SOURCE FILES');
foreach ($sourceFiles as $i => $sourceFilePath) {
	$sourceFileName = basename($sourceFilePath);
	if ($sourceFileName == 'clew.cpp') continue;
	$folderName = basename(str_replace($sourceFileName, '', $sourceFilePath));
	$status = array();
	$success = true;
	$sourceContent = file_get_contents($sourceFilePath);

	if ($success) $success = checkFileHeader($sourceFilePath, $sourceContent, $status);
	if ($success) $success = checkClangFormat($sourceContent, $status);
	if ($success) $success = checkIncludeHeaders($sourceFilePath, $sourceContent, $status, $folderName);

	if ($success && !isDryRun() && count($status) > 0) {
		file_put_contents($sourceFilePath, $sourceContent);
	}
	printResultLine($folderName . '/' . $sourceFileName, $success, $status, $i / count($sourceFiles));
}
printEndGroup();

printStartGroup('RUNNING CHECKS ON HEADER FILES');
foreach ($headerFiles as $i => $headerFilePath) {
	$headerFileName = basename($headerFilePath);
	if (substr($headerFileName, 0, strlen('ui_')) == 'ui_') continue;
	if ($headerFileName == 'clew.h') continue;
	if ($headerFileName == 'clew-cl.hpp') continue;
	if ($headerFileName == 'include_header_wrapper.hpp') continue;
	$folderName = basename(str_replace($headerFileName, '', $headerFilePath));
	$status = array();
	$success = true;
	$headerContent = file_get_contents($headerFilePath);

	if ($success) $success = checkFileHeader($headerFilePath, $headerContent, $status);
	if ($success) $success = checkDefines($headerContent, $headerFileName, $folderName, $status);
	if ($success) $success = checkClangFormat($headerContent, $status);
	if ($success) $success = checkIncludeHeaders($headerFilePath, $headerContent, $status, $folderName);

	if ($success && !isDryRun() && count($status) > 0) {
		file_put_contents($headerFilePath, $headerContent);
	}
	printResultLine($folderName . '/' . $headerFileName, $success, $status, $i / count($headerFiles));
}
printEndGroup();

if(argumentContains('checkTidy')){
	printStartGroup('RUNNING CLANG-TIDY');
	echo noticeString('This may take some time ...') . PHP_EOL;
	checkClangTidy();
	printEndGroup();
}
printFinish();
exit;


function checkFileHeader($filePath, &$fileContent, &$status)
{
	$headerRegex = '/^(\/\*\*?[\s\S]*?\*\/)([\s\S]*)$/';
	if (preg_match($headerRegex, $fileContent, $matchHeader)) {
		$fileHeader = $matchHeader[1];
		// $fileSourceCode = $matchHeader[2];
		$modificationString = getModificationInterval($filePath);

		if (strpos($fileHeader, 'MKNmMMKmm') === false) {
			// not a recent proper file header, try other known formats
			// old format
			$regexParseHeaderOld = '/^[\s\S]+Mandelbulber\sv2.*[\s\S]*?(\w.*)[\S\s]+';
			$regexParseHeaderOld .= 'General\sPublic[\S\s]+';
			$regexParseHeaderOld .= 'Authors:\s(.*)[\s\S]*?\*\/([\s\S]*)$/';
			$regexParseHeaderEclipse = '/^[\s\S]+Author:[\s\S]*?\*\/([\s\S]*)$/';
			$regexParseHeaderAny = '/^\/\*\*?([\s\S]*?)\*\/([\s\S]*)$/';

			if (preg_match($regexParseHeaderOld, $fileContent, $matchHeaderOld)) {
				$status[] = noticeString('header is old, will rewrite to new!');
				$newFileContent = getFileHeader($matchHeaderOld[2], $matchHeaderOld[1], $modificationString) . $matchHeaderOld[3];
				$fileContent = $newFileContent;
				return true;
			} else if (preg_match($regexParseHeaderEclipse, $fileContent, $matchHeaderEclipse)) {
				$status[] = noticeString('header is in eclipse format, will rewrite to new!');
				$newFileContent = getFileHeader('Krzysztof Marczak (buddhi1980@gmail.com)', 'TODO: description', date('Y')) . $matchHeaderEclipse[1];
				$fileContent = $newFileContent;
				return true;
			} else if (preg_match($regexParseHeaderAny, $fileContent, $matchHeaderAny)) {
				$status[] = noticeString('header is unknown format, just copying contents of header!');
				$newFileContent = getFileHeader('### TODO: unknown author! ###', $matchHeaderAny[1], date('Y')) . $matchHeaderAny[2];
				$fileContent = $newFileContent;
				return true;
			} else {
				$status[] = errorString('header unknown!');
			}
		} else {
                        $regexParseHeader = '/^[\s\S]+?#{50}?[\S\s]+?Authors:\s([\s\S]*?)\*\n([\s\S]*?)\*\/([\s\S]*)$/';
			if (preg_match($regexParseHeader, $fileContent, $matchHeaderNew)) {
				$newFileContent = getFileHeader($matchHeaderNew[1], $matchHeaderNew[2], $modificationString) . $matchHeaderNew[3];
				if ($newFileContent != $fileContent) {
					$status[] = noticeString('header is new, will rewrite to new!');
					$fileContent = $newFileContent;
				}
				return true;
			} else {
                                $status[] = errorString('header unknown2!');
			}
		}

	} else {
		$status[] = errorString('No header found!');
	}
	return false;
}

function checkDefines(&$fileContent, $headerFileName, $folderName, &$status)
{
	$defineRegex = '/^([\s\S]*?#ifndef\s)([\s\S]+?)(\n#define\s)(\S+)([\S\s]+)(#endif\s)[\s\S]+$/';
	if (preg_match($defineRegex, $fileContent, $match)) {
		$defineName = 'MANDELBULBER2_' . strtoupper($folderName) . '_' . strtoupper(str_replace('.', '_', $headerFileName)) . '_';
		$newFileContent = $match[1] . $defineName . $match[3] . $defineName . $match[5] . $match[6] . '/* ' . $defineName . ' */' . PHP_EOL;

		if ($newFileContent != $fileContent) {
			$fileContent = $newFileContent;
			$status[] = noticeString('define changed');
		}
		return true;
	} else {
		$status[] = errorString('define not found!');
	}
	return false;
}

function checkClangFormat(&$fileContent, &$status)
{
	$contentsBefore = $fileContent;

	$filepathTemp = PROJECT_PATH . '/tools/.tmp.c';
	file_put_contents($filepathTemp, $fileContent);
	shell_exec(CLANG_FORMAT_EXEC_PATH . ' -i --style=file ' . escapeshellarg($filepathTemp));
	$fileContent = file_get_contents($filepathTemp);
	unlink($filepathTemp); // nothing to see here :)

	if ($contentsBefore != $fileContent) {
		$status[] = noticeString('checkClangFormat changed');
	}
	return true;
}

function checkIncludeHeaders($filepath, &$fileContent, &$status, $folderName)
{
	$includeRegex = '/^([\s\S]*?)(\n#include[\s\S]+)?(\n#include.*)([\s\S]+)$/';
	if (preg_match($includeRegex, $fileContent, $match)) {
		$beforeCapture = $match[1];
		$includesCapture = $match[2] . $match[3];
		$afterCapture = $match[4];
		$includesIn = array_filter(explode(PHP_EOL, $includesCapture));
		$includesIn = array_values($includesIn);
		$includesOut = array(
			'moduleHeader' => array(),
			'cHeader' => array(),
			'cppHeader' => array(),
			'qtHeader' => array(),
			'uiAutogenHeader' => array(),
			'localHeader' => array(),
			'srcHeader' => array(),
			'uiHeader' => array(),
			'openclHeader' => array(),
		);
		$customIncludes = '';
		foreach ($includesIn as $k => $includeLine) {
			$includeLine = str_replace('../src/', 'src/', $includeLine);
			$includeLine = str_replace('../qt/', 'qt/', $includeLine);
			$includeLine = str_replace('../opencl/', 'opencl/', $includeLine);
			$includeLine = str_replace('qt/ui_', 'ui_', $includeLine);
			$includeLine = str_replace('"' . $folderName . '/', '"', $includeLine); // strip folder, when include from same folder
			$includeFormatRegex = '/^#include ([<"])([a-zA-Z0-9._\/]+)([>"])$/';
			if ($includeLine == '// custom includes') {
				$customIncludes = implode(PHP_EOL, array_splice($includesIn, $k));
				break;
			}
			if (preg_match($includeFormatRegex, $includeLine, $matchInclude)) {
				$includeFileName = $matchInclude[2];
				if ($matchInclude[1] == '"') {
					// project includes
					if (pathinfo(basename($includeFileName), PATHINFO_FILENAME)
						== pathinfo(basename($filepath), PATHINFO_FILENAME)) $includesOut['moduleHeader'][] = $includeLine;
					else if (strpos($includeFileName, 'ui_') === 0) $includesOut['uiAutogenHeader'][] = $includeLine;
					else if (strpos($includeFileName, '/') === false) $includesOut['localHeader'][] = $includeLine;
					else if (strpos($includeFileName, 'src/') !== false) $includesOut['srcHeader'][] = $includeLine;
					else if (strpos($includeFileName, 'qt/') !== false) $includesOut['uiHeader'][] = $includeLine;
					else if (strpos($includeFileName, 'opencl/') !== false) $includesOut['openclHeader'][] = $includeLine;
					else {
						$status[] = errorString('invalid include line: "' . $includeLine . '"');
						return false;
					}
				} else if ($matchInclude[1] == '<') {
					// system includes
					if (substr($includeFileName, 0, 1) == 'Q') $includesOut['qtHeader'][] = $includeLine;
                    elseif (substr($includeFileName, -2, 2) == '.h') $includesOut['cHeader'][] = $includeLine;
					else $includesOut['cppHeader'][] = $includeLine;
				} else {
					$status[] = errorString('invalid include line: "' . $includeLine . '"');
					return false;
				}
			} else {
				$status[] = errorString('invalid include line: "' . $includeLine . '"');
				return false;
			}
		}
		$newIncludes = '';
		foreach ($includesOut as $includeGroup) {
			if (empty($includeGroup)) continue;
			sort($includeGroup, SORT_STRING);
			$includeGroup = array_unique($includeGroup);
			$newIncludes .= PHP_EOL . PHP_EOL . implode(PHP_EOL, $includeGroup);
		}
		if (!empty($customIncludes)) $newIncludes .= PHP_EOL . PHP_EOL . $customIncludes;
		$newFileContent = $beforeCapture . PHP_EOL . trim($newIncludes) . $afterCapture;
		if ($newFileContent != $fileContent) {
			$fileContent = $newFileContent;
			$status[] = noticeString('includes changed');
		}
		return true;
	}

	if (isVerbose()) $status[] = warningString('no includes in file');
	return true;
}

function checkClangTidy()
{
	$cmakeFolder = PROJECT_PATH . 'cmake';
    shell_exec('cmake ' . escapeshellarg($cmakeFolder) . ' -DCMAKE_EXPORT_COMPILE_COMMANDS=ON');

	// TODO apply more styles, maybe modernize-*?
	// Following checks are available with clang-tidy-6.0
	$checks = array(
		'modernize-avoid-bind',
		'modernize-deprecated-headers',
        'modernize-loop-convert',
		'modernize-make-shared',
		'modernize-make-unique',
		'modernize-pass-by-value',
		'modernize-raw-string-literal',
		'modernize-redundant-void-arg',
		'modernize-replace-auto-ptr',
		'modernize-replace-random-shuffle',
		// 'modernize-return-braced-init-list',
		'modernize-shrink-to-fit',
		'modernize-unary-static-assert',
		// 'modernize-use-auto',
		'modernize-use-bool-literals',
		// 'modernize-use-default-member-init',
		'modernize-use-emplace',
                'modernize-use-equals-default',
		'modernize-use-equals-delete',
		'modernize-use-noexcept',
		'modernize-use-nullptr',
		'modernize-use-override',
		'modernize-use-transparent-functors',
		'modernize-use-default-member-init',
		//'modernize-use-using',
	);

	$checkString = '-*,' . implode(',', $checks);

	$cmd = RUN_CLANG_TIDY_EXEC_PATH
            . ' -clang-apply-replacements-binary=' . CLANG_APPLY_BINARY_PATH
            . ' -checks=' . $checkString
            . ' -header-filter=".*"'
            // . ' -quiet'
          //  . ' -p ' . escapeshellarg($cmakeFolder)
            . (!isDryRun() ? ' -fix' : '');
         //   . ' 2>/dev/null';

	// echo $cmd; exit;
	$out = shell_exec($cmd);
	foreach($checks as $check){
		$cnt = substr_count ($out , '[' . $check . ']');
        echo noticeString(str_pad($check, 30)) . ' - ';
        if($cnt > 0){
            echo successString($cnt . ' occurences');
		}else{
			echo $cnt . ' occurences';
        }
        echo PHP_EOL;
    }
    file_put_contents(PROJECT_PATH . '/tools/clang-tidy.log', $out);

	return true;
}


function getFileHeader($author, $description, $modificationString)
{
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
	foreach ($lines as $line) {
		$l = trim($line);
		if (!$firstFound && ($l == '*' || empty($l))) continue;
		$regexCommentStart = '/^\*\s?(.*)$/';
		if (preg_match($regexCommentStart, $l, $match)) {
			$l = $match[1];
		}
		$nonEmptyLines[] = rtrim(($firstFound ? ' * ' : '') . $l);
		$firstFound = true;
	}
	if ($nonEmptyLines[count($nonEmptyLines) - 1] == ' *') {
		unset($nonEmptyLines[count($nonEmptyLines) - 1]);
	}

	$description = implode(PHP_EOL, $nonEmptyLines);
	$spacing = str_repeat(' ', 10 - strlen($modificationString));
	return str_replace(array('__author__', '__description__', '__date__', '__spacing__'),
		array(formatAuthorLine($author), $description, $modificationString, $spacing), $out);
}

function formatAuthorLine($authorLine)
{
	$oldAuthors = explode(',', $authorLine);
	$out = '';
	foreach ($oldAuthors as $oldAuthor) {
		$name = trim($oldAuthor);
		if ($name == '') continue;
		$author = lookUpAuthor($name);
		if (strlen($out) > 0) $out .= ', ';
		if (strlen($out) - strrpos($out, PHP_EOL) + strlen($author) > 90)
			$out = substr($out, 0, -1) . PHP_EOL . ' *  ';
		$out .= $author;
	}
	return $out;
}

function lookUpAuthor($authorName)
{
	// use consistent author name
	if (strpos($authorName, 'Krzysztof Marczak') !== false) return 'Krzysztof Marczak (buddhi1980@gmail.com)';
	if (strpos($authorName, 'Graeme McLaren') !== false) return 'Graeme McLaren';
	if (strpos($authorName, 'pmneila') !== false) return 'pmneila';
	if (strpos($authorName, 'Stanislaw Adaszewski') !== false) return 'Stanislaw Adaszewski';
	if (strpos($authorName, 'Rayan Hitchman') !== false) return 'Rayan Hitchman';
	if (strpos($authorName, 'Robert Pancoast') !== false) return 'Robert Pancoast (RobertPancoast77@gmail.com)';
	if (strpos($authorName, 'Sebastian Jennen') !== false) return 'Sebastian Jennen (jenzebas@gmail.com)';

	errorString('unknown author: ' . $authorName);
	return $authorName;
}

?>

