#!/usr/bin/env php
#
# this file takes the formulas in src/fractal_formulas.cpp and does a couple of things:
# - generates the information boxes in the ui from the source code
# - generates opencl formulas
# - generates formula thumbnails
#
# requires packages: highlight, clang-format, git and php (apt-get install highlight clang-format git php5-cli)
#
# on default this script runs dry,
# it will try to parse all formulas and show which ui files would be modified
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to ui files
# if you invoke this script with "checkCl" as cli argument it will also check formula cl file compilability (slow)
#

<?php
require_once(dirname(__FILE__) . '/common.inc.php');
printStart();

printStartGroup('READING FORMULA DATA');
$formulas = getFormulasData();
$formulaExampleUsage = getFormulaExampleUsage();

printStartGroup('RUNNING FORMULA CHECKS');
foreach ($formulas as $index => $formula) {
    @$i++;
	$success = true;
	$status = array();
	if ($success) $success = updateInfoBoxes($index, $formula, $status);
	if ($success) $success = generateFormulaOpenCLFiles($formula, $status);
	if ($success) $success = generateFormulaIcons($formula, $status);
	if ($success && argumentContains('checkCl')) $success = checkOpenCLCompile($formula, $status);
	printResultLine($formula['name'], $success, $status, $i / count($formulas));
}
printEndGroup();

printFinish();
exit;


function getFormulasData()
{
	$formulas = array();
	$fractalListContent = file_get_contents(PROJECT_PATH . 'src/fractal_list.cpp');
	$fractalFunctionContent = file_get_contents(PROJECT_PATH . 'src/fractal_formulas.cpp');
	$fractalFunctionContentFormulasOnly = substr($fractalFunctionContent,
		strpos($fractalFunctionContent, 'using namespace fractal;') + strlen('using namespace fractal;'));
	$fractalFunctionContentExploded = explode(PHP_EOL . '}' . PHP_EOL, $fractalFunctionContentFormulasOnly);
	$indexIdLookup = getIndexIdLookUp();

	// parse fractal_list data
	$regexFormulas = '/sFractalDescription\(([\s\S]*?)\)\);/';
	preg_match_all($regexFormulas, $fractalListContent, $formula_matches);

	foreach ($formula_matches[0] as $key => $formulaMatch) {
		if ($key == 0) continue; // skip formula "none"

		// read index and name from fractal_list
		preg_match('/"([a-zA-Z0-9 _\'-\^]+)"[\s\S]*?"([a-zA-Z0-9 _-]+)"[\s\S]*?([a-zA-Z0-9 _-]+)[\s\S]*?([a-zA-Z0-9 _-]+)/', $formulaMatch, $matches);
		if (count($matches) < 4) die('could not read index for formula : ' . $formulaMatch);
		$name = trim($matches[1]);
		$internalName = trim($matches[2]);
		$index = trim($matches[3]);
		$functionName = trim($matches[4]);;

		$internalNameNew = from_camel_case($index);
		$functionNameNew = ucfirst($index) . 'Iteration';

		// check for automatic renaming to fit naming convention
		if ($internalNameNew != $internalName) {
			if (!isDryRun()) {
				upgradeInternalName($internalName, $internalNameNew);
			}
			echo noticeString('internal name upgrade from ' . $internalName . ' to ' . $internalNameNew) . PHP_EOL;
			$internalName = $internalNameNew;
			if (!isDryRun()) die('Changes have been written, check changes and run script again for more changes.');
		}
		if ($functionName != $functionNameNew) {
			if (!isDryRun()) {
				upgradeFunctionName($functionName, $functionNameNew);
			}
			echo noticeString('function name upgrade from ' . $functionName . ' to ' . $functionNameNew) . PHP_EOL;
			if (!isDryRun()) die('Changes have been written, check changes and run script again for more changes.');
		}

		// read function contents
		$functionContentMatchString = '/(\/\*\*[\s\S]+?\*\/)[\s\S]*(void ' . $functionName . '\([\s\S]*)/';
		$functionContentFound = false;
		$code = false;
		$comment = false;
		$rawComment = false;
		foreach ($fractalFunctionContentExploded as $functionKey => $functionValue) {
			if (preg_match($functionContentMatchString, $functionValue, $matchFunctionContent)) {
				$functionContentFound = true;
				$rawComment = $matchFunctionContent[1];
				$comment = parseComment($matchFunctionContent[1]);
				$code = $matchFunctionContent[2] . PHP_EOL . '}';
			}
		}

		if (!$functionContentFound) {
			echo errorString('Warning, could not read code for index: ' . $index) . PHP_EOL;
			continue;
			// die('could not read code for index: ' . $index);
		}

		$formulas[$index] = array(
			'uiFile' => PROJECT_PATH . 'formula/ui/fractal_' . $internalName . '.ui',
			'name' => $name,
			'internalName' => $internalName,
			'functionName' => $functionName,
			'code' => $code,
			'comment' => $comment,
			'rawComment' => $rawComment,
			'id' => $indexIdLookup[$index],
			'openclFile' => PROJECT_PATH . 'formula/opencl/' . $internalName . '.cl',
			'openclCode' => parseToOpenCL($code),
			// 'openclCodeDouble' => parseToOpenCL($code, 'double'),
			'type' => (strpos($internalName, 'transf_') !== false ? 'transf' : 'formula'),
		);
		// print_r($formulas);
	}
	return $formulas;
}

// update information boxes in the ui
function updateInfoBoxes($index, $formula, &$status)
{
	global $formulaExampleUsage;
	$formattedEscapedCode = getFormatCode($formula['code']);
	// remove hardcoded font-size, to use system defined font-size
	$formattedEscapedCode = str_replace('font-size:10pt;', '', $formattedEscapedCode);

	// extract only body element, since html part and comment change between versions of highlight
	$startCode = strpos($formattedEscapedCode, '<body');
	$endCode = strpos($formattedEscapedCode, '</body>') + strlen('</body>');
	$formattedEscapedCode = substr($formattedEscapedCode, $startCode, $endCode - $startCode);

	$comment = $formula['comment'];
	$informationText = '';
	if (!empty($comment['description'])) {
		$informationText = '<p>' . implode('<br>', $comment['description']) . '</p>';
	}
	$informationText .= "<table>" . PHP_EOL;
	// $informationText .= "<tr><th>Name</th><td>" . $formula['name'] . "</td></tr>" . PHP_EOL;

	if (!empty($comment['reference'])) {
		$informationText .= '<tr><th>Reference</th><td>';
		foreach ($comment['reference'] as $ref) {
			$informationText .= "<a href=\"" . $ref . "\">" . $ref . "<br>" . PHP_EOL;
		}
		$informationText .= '</td></tr>' . PHP_EOL;
	}
	if (!empty($comment['author'])) {
		$informationText .= '<tr><th>Author</th><td>';
		foreach ($comment['author'] as $author) {
			$informationText .= $author . "<br>" . PHP_EOL;
		}
		$informationText .= '</td></tr>' . PHP_EOL;
	}
	$informationText .= "</table>" . PHP_EOL;
	if (array_key_exists($formula['id'], $formulaExampleUsage)) {
		$informationText .= '<br><b>Examples using this formula</b><br>';
		$exampleFilenames = $formulaExampleUsage[$formula['id']];
		if (count($exampleFilenames) > 5) { // do not show more than 5 examples
			array_splice($exampleFilenames, 5);
			$exampleFilenames[] = '...';
		}
		$informationText .= implode('<br>', $exampleFilenames);
	} else {
		if (isWarning()) {
			$status[] = warningString('formula ' . $formula['name'] . ' is not used in any examples yet.');
		}
	}

	$informationText .= "<h3>Code</h3>" . PHP_EOL;
	
	$uiFileContent = file_get_contents($formula['uiFile']);
	$regexInformation = '/(<widget class="MyGroupBox" name="groupCheck_info">[\s\S]+?)<item>[\s\S]+?<\/layout>/';

	$replacement = '$1<item>
       <widget class="QLabel" name="label_information_general">
        <property name="text">
         <string notr="true">' . htmlentities($informationText) . '</string>
        </property>
        <property name="wordWrap">
         <bool>true</bool>
        </property>
        <property name="openExternalLinks">
         <bool>true</bool>
        </property>
        <property name="textInteractionFlags">
         <set>Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse</set>
        </property>
       </widget>
      </item>
      <item>
       <widget class="QLabel" name="label_code_content">
        <property name="styleSheet">
         <string notr="true">border-style: outset; border-width: 2px; border-radius: 3px; border-color: black; background-color: #fff5ee; padding: 4px;</string>
        </property>
        <property name="text">
         <string notr="true">' . htmlentities($formattedEscapedCode) . '</string>
        </property>
        <property name="wordWrap">
         <bool>true</bool>
        </property>
        <property name="openExternalLinks">
         <bool>true</bool>
        </property>
        <property name="textInteractionFlags">
         <set>Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse</set>
        </property>
       </widget>
      </item>
     </layout>';
	$newUiFileContent = preg_replace($regexInformation, $replacement, $uiFileContent, -1, $count);
	$newUiFileContent = preg_replace('/<class>.+<\/class>/', '<class>' . $formula['internalName'] . '</class>', $newUiFileContent, 1);
	$postitionOfWindowTitle = strpos($newUiFileContent, 'windowTitle ');
	$postitionOfWindowTitleStringStart = strpos($newUiFileContent, '<string', $postitionOfWindowTitle);
	$postitionOfWindowTitleStringEnd = strpos($newUiFileContent, '</string>', $postitionOfWindowTitle) + strlen('</string>');
	$newUiFileContent = substr_replace($newUiFileContent, '<string notr="true">' . $formula['internalName'] . '</string>',
		$postitionOfWindowTitleStringStart, $postitionOfWindowTitleStringEnd - $postitionOfWindowTitleStringStart);
	if ($count == 0) {
		$status[] = errorString('Warning, could not replace code in ui file for index: ' . $index) . PHP_EOL;
		return false;
	}

	$replaceFormulaLookup = array(
		array('find' => '/\>\s*([^<]+?)\s*<\/string>/', 'replace' => '>$1</string>'), // whitespace fix 1
		array('find' => '/\>\s*([^<]+?)\s*:\s*<\/string>/', 'replace' => '>$1:</string>'), // whitespace fix 2
		array('find' => '/<string>(.*?)\s+(\s.*?)<\/string>/', 'replace' => '<string>$1$2</string>'), // whitespace fix 3, stop those whitespaces! :)
		array('find' => '/<string>fabs\s*\(\s*(.*?)\s*\)\s*<\/string>/', 'replace' => '<string>fabs($1)</string>'),
		array('find' => '/<string>abs\s*\(\s*(.*?)\s*\)\s*<\/string>/', 'replace' => '<string>abs($1)</string>'),
		array('find' => '/<string>(.*?)::(.*?)<\/string>/', 'replace' => '<string>$1:$2</string>'),
		array('find' => '/<string>Menger Scale<\/string>/', 'replace' => '<string>Menger Scale:</string>'),
		array('find' => '/<string>Menger offsets:<\/string>/', 'replace' => '<string>Menger Offset:</string>'),
		array('find' => '/<string>DE tweak temp<\/string>/', 'replace' => '<string>DE tweak temp:</string>'),
		array('find' => '/<string>Offset<\/string>/', 'replace' => '<string>Offset:</string>'),
		array('find' => '/Rotation;<\/string>/', 'replace' => 'Rotation</string>'),
		array('find' => '/>Multiplier<\/string>/', 'replace' => '>Multiplier:</string>'),
		array('find' => '/>Cpixel Multiplier<\/string>/', 'replace' => '>Cpixel Multiplier:</string>'),
		array('find' => '/>Addition Constant<\/string>/', 'replace' => '>Addition Constant:</string>'),
		array('find' => '/>DE Tweak<\/string>/', 'replace' => '>DE Tweak:</string>'),
		array('find' => '/>Post_scale<\/string>/', 'replace' => '>Post Scale:</string>'),
		array('find' => '/>Pre_scale<\/string>/', 'replace' => '>Pre Scale:</string>'),

        // temporary code to find iteration replacement 250 -> 999
		// array('find' => '/(<widget class=".*" name=".*stop_iterations">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*start_iterations">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*stop_iterations">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="minimum">\n\s+<number>.*<\/number>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*start_iterations">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="minimum">\n\s+<number>.*<\/number>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*iter.*">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*iter.*">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*iter.*">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="minimum">\n\s+<number>.*<\/number>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
		// array('find' => '/(<widget class=".*" name=".*iter.*">\n\s+<property name="sizePolicy">\n\s+<sizepolicy hsizetype=".*" vsizetype=".*">\n\s+<horstretch>0<\/horstretch>\n\s+<verstretch>0<\/verstretch>\n\s+<\/sizepolicy>\n\s+<\/property>\n\s+<property name="minimum">\n\s+<number>.*<\/number>\n\s+<\/property>\n\s+<property name="maximum">\n\s+<number)>250/', 'replace' => '$1>999'),
	);
	foreach ($replaceFormulaLookup as $item) {
		$newUiFileContent = preg_replace($item['find'], $item['replace'], $newUiFileContent);
	}

	// untranslatable and remove double point
	$notrRemoveDoublePointCI = array(
		'z\.x', 'z\.y', 'z\.z', 'z\.w',
		'c\.x', 'c\.y', 'c\.z', 'c\.w',
		'xyz', 'xzy', 'yxz', 'yzx', 'zxy', 'zyx',
		'x', 'y', 'z', 'w',
		'xy', 'yx', 'xz', 'zx', 'yz', 'zy',
		'xw', 'yw', 'zw',
		'alpha', 'beta', 'gamma',
		'fabs', 'fabs\(z\.x\)', 'fabs\(z\.y\)', 'fabs\(z\.z\)', 'fabs\(z\)',
		'abs', 'abs\(x\)', 'abs\(y\)', 'abs\(z\)',
		'T1', 'T1mod', 'T2', 'T3', 'T4', 'T5b',
		'asin', 'acos', 'atan', 'atan2',
	);

	foreach ($notrRemoveDoublePointCI as $item) {
		$newUiFileContent = preg_replace('/<string>' . $item . ':*;*<\/string>/i',
			'<string notr="true">' . str_replace('\\', '', $item) . '</string>', $newUiFileContent);
	}

	// simple find and replace whole string
	$simpleReplace = array(
		array('find' => 'Type 4 Fold Value', 'replace' => 'Type 4 Fold value:'),
		array('find' => 'Type 5 Fold2 Value', 'replace' => 'Type 5 Fold2 value:'),
	);

	foreach ($simpleReplace as $item) {
		$newUiFileContent = str_replace($item['find'], $item['replace'], $newUiFileContent);
	}

	if ($newUiFileContent == $uiFileContent) {
		return true;
	}
	if (!isDryRun()) {
		file_put_contents($formula['uiFile'], $newUiFileContent);
	}
	$status[] = noticeString('ui file changed'); // . (' . basename($formula['uiFile']) . ')
	return true;
}

// generate opencl formulas
function generateFormulaOpenCLFiles($formula, &$status)
{
	// TODO add primitives

	$fileHeader = '/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.        ____                _______    
 * Copyright (C) ' . getModificationInterval($formula['openclFile']) . ' Mandelbulber Team   _>]|=||i=i<,     / __ \___  ___ ___  / ___/ /
 *                                        \><||i|=>>%)    / /_/ / _ \/ -_) _ \/ /__/ /__
 * This file is part of Mandelbulber.     )<=i=]=|=i<>    \____/ .__/\__/_//_/\___/____/
 * The project is licensed under GPLv3,   -<>>=|><|||`        /_/                       
 * see also COPYING file in this folder.    ~+{i%+++
 * ' . str_replace('/**', '', $formula['rawComment']) . PHP_EOL . PHP_EOL;

	$autogenLine = '/* ### This file has been autogenerated. Remove this line, to prevent override. ### */' . PHP_EOL . PHP_EOL;
	$openclContent = @file_get_contents($formula['openclFile']);
	$newOpenCLContent = $fileHeader . $autogenLine . PHP_EOL . PHP_EOL;
	$newOpenCLContent .= $formula['openclCode'];
	// $newOpenCLContent .= '#ifndef DOUBLE_PRECISION' . PHP_EOL . $formula['openclCode'] . PHP_EOL;
	// $newOpenCLContent .= '#else' . PHP_EOL . $formula['openclCodeDouble'] . PHP_EOL . '#endif' . PHP_EOL;

	// clang-format
	$filepathTemp = PROJECT_PATH . '/tools/.tmp.c';
	file_put_contents($filepathTemp, $newOpenCLContent);
	shell_exec(CLANG_FORMAT_EXEC_PATH . ' -i --style=file ' . escapeshellarg($filepathTemp));
	$newOpenCLContent = file_get_contents($filepathTemp);
	unlink($filepathTemp); // nothing to see here :)

	$newOpenCLContentWithoutDateLine = preg_replace('/Copyright\s\(C\)\s\d+/', '', $newOpenCLContent);
	$openclContentWithoutDateLine = preg_replace('/Copyright\s\(C\)\s\d+/', '', $openclContent);

	if ($newOpenCLContentWithoutDateLine == $openclContentWithoutDateLine) {
		return true;
	} else if (!empty($openclContent) && strpos($openclContent, $autogenLine) === false) {
		$status[] = warningString('opencl has been manually modified, will not override.');
		return true;
	}
	if (!isDryRun()) {
		file_put_contents($formula['openclFile'], $newOpenCLContent);
		// file_put_contents($formula['openclFile'] . '.orig', $formula['code']);
	}
	$status[] = noticeString('opencl file changed'); // (' . basename($formula['openclFile']) . ')
	return true;
}

// generate formula icons
function generateFormulaIcons($formula, &$status)
{
	// autogenerate missing formula and transform images
	$imgPath = PROJECT_PATH . 'formula/img/' . $formula['internalName'] . '.png';
	if (file_exists($imgPath)) {
		return true;
	}

	if (!isDryRun()) {
		if (generate_formula_icon($formula, $imgPath)) {
			$status[] = successString('image generated.');
			return true;
		} else {
			$status[] = noticeString('image not generated. Maybe Mandelbulber not found?');
			return true;
		}
	} else {
		$status[] = noticeString('image does not exist');
		return true;
	}
}

function checkOpenCLCompile($formula, &$status)
{
    $checkOpenCLCompileCmd = 'clang -c -S -emit-llvm -o test.ll -w -include clc/clc.h -Dcl_clang_storage_class_specifiers -x cl';
	$checkOpenCLCompileCmd .= '  -o /dev/null'; // -S -emit-llvm
	$checkOpenCLCompileCmd .= ' -DOPENCL_KERNEL_CODE -I' . PROJECT_PATH . 'opencl/';
	$checkOpenCLCompileCmd .= ' ' . $formula['openclFile'] . ' 2>&1';
	exec ($checkOpenCLCompileCmd, $output, $ret);
	if($ret != 0){
	    $status[] = errorString('formula opencl file broken! ' . (!isVerbose() ? 'see error with verbose mode' : ''));
		if(isVerbose()) $status[] = print_r($output);
		return false;
	}
	return true;
}

function getFormatCode($code)
{
	$cmd = "echo " . escapeshellarg($code);
	$cmd .= " | sed 's/	/  /g' "; // replace tab with double space
	$cmd .= " | highlight -O html --style seashell --inline-css --syntax cpp";
	return shell_exec($cmd);
}

function parseComment($c)
{
	$lines = explode(PHP_EOL, $c);
	$out = array();
	$tag = 'description';
	foreach ($lines as $l) {
		$line = trim($l);
		if (in_array($line, array('/**', '*/'))) continue;
		if (preg_match("/\*[\s]+@([\S]+)[\s]*(.*)/", $line, $match)) {
			$tag = $match[1];
			if (!array_key_exists($tag, $out)) $out[$tag] = array();
			$text = trim($match[2]);
			if ($text != '' || count($out[$tag]) > 0) {
				$out[$tag][] = $match[2];
			}
		} else if (preg_match("/\*[\s]+(.*)/", $line, $match)) {
			$out[$tag][] = $match[1];
		} else {
			$out[$tag][] = '';
		}
	}
	return $out;
}

function getFormulaExampleUsage()
{
	// get a lookup for every formula in which example files it is used
	$exampleBasePath = PROJECT_PATH . 'deploy/share/mandelbulber2/examples/';
	$exampleFileMasks[] = $exampleBasePath . '*.fract';
	$exampleFileMasks[] = $exampleBasePath . '*/*.fract';
	$formulaExampleUsage = array();
	foreach ($exampleFileMasks as $exampleFileMask) {
		foreach (glob($exampleFileMask) as $exampleFileName) {
			$exampleContent = file_get_contents($exampleFileName);
			$exampleContentLines = explode(PHP_EOL, $exampleContent);
			$pathOfFileFromExamples = str_replace($exampleBasePath, '', $exampleFileName);

			foreach ($exampleContentLines as $line) {
				$line = trim($line);
				if (preg_match("/^formula_\d+\s(\d+);$/", $line, $match)) {
					if (!array_key_exists($match[1], $formulaExampleUsage)) $formulaExampleUsage[$match[1]] = array();
					if (!in_array($pathOfFileFromExamples, $formulaExampleUsage[$match[1]])) {
						$formulaExampleUsage[$match[1]][] = $pathOfFileFromExamples;
					}
				}
			}
		}
	}
	return $formulaExampleUsage;
}

function getIndexIdLookUp()
{
	// get the integer id from fractal_list.hpp for the named index of each formula
	$fractalListHeaderContent = file_get_contents(PROJECT_PATH . 'src/fractal_list.hpp');
	$indexIdLookUp = array();
	$fractalListHeaderContentLines = explode(PHP_EOL, $fractalListHeaderContent);
	foreach ($fractalListHeaderContentLines as $line) {
		$line = trim($line);
		if (preg_match("/^(\w+)\s=\s(\d+),.*$/", $line, $match)) {
			$indexIdLookUp[$match[1]] = $match[2];
		}
	}
	return $indexIdLookUp;
}

function parseToOpenCL($code, $mode = 'single')
{
	// $fod = $mode == 'single' ? 'float' : 'double';
	$fod = 'REAL';
	$var = '-?[A-Za-z_][A-Za-z0-9\.\-\>_\[\]]*'; // regex for a var name
	$functionName = '[A-Za-z_][A-Za-z0-9\.\-\>_]*'; // regex for a function name
	$double = '(?:-?\d+\.?\d*(?:[eE][+-]?\d+)?|-?\d*\.?\d+(?:[eE][+-]?\d+)?)'; // regex for a double value
	$float = $double . 'f'; // regex for a float value
	$nb = "[^()]*"; // matches everything but braces
	$br = "(?:$functionName)?\($nb\)"; // regex for a simply braced expression with optional function invocation
	$br1_1 = "\($nb$br$nb\)"; // regex for a double braced expression (one inner brace)
	$br1_2 = "\($nb$br$nb$br$nb\)"; // regex for a double braced expression (two inner braces)
	$s = '[\n\r\s]+'; // any whitespace including new lines
	$all = '[\S\s]+?'; // anything including new lines as few as possible (mark end with next operator)
	$rval = "$br1_2|$br1_1|$br|$float|$var"; // any of those types can be an "assignable expression"
	$preF = "\s|\(|\{|-"; // these chars can occur befire a function
	$multChain = "(?:(?:$rval)$s\*$s)*(?:$rval)"; // a chain of multiplicated expressions
	// see here for all possible: https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/mathFunctions.html
	$cppToOpenCLReplaceLookup = array(
		array('find' => '/double/', 'replace' => $fod),                // all doubles to floats
		array('find' => "/([\s\(\{])($double)([\s\)\};,])/", 'replace' => '$1$2' . ($mode == 'single' ? 'f' : '') . '$3'),    // double literals to float literals
		array('find' => "/($preF)sin\(/", 'replace' => '$1native_sin('),          // native sin
		array('find' => "/($preF)cos\(/", 'replace' => '$1native_cos('),          // native cos
		array('find' => "/($preF)pow\(/", 'replace' => '$1native_powr('),         // native pow
		array('find' => "/($preF)sqrt\(/", 'replace' => '$1native_sqrt('),        // native sqrt
		array('find' => "/CVector3\(($multChain,$s$multChain,$s$multChain)\)/", 'replace' => '(' . $fod . '3) {$1}'),  // CVector3 to built in float3
		array('find' => "/CVector3\(\)/", 'replace' => '(' . $fod . '3) {}'),  // CVector3 default constructor to built in float3
		array('find' => "/CVector3\(($all)\);/", 'replace' => '(' . $fod . '3) {$1};'),  // CVector3 to built in float3
		array('find' => "/CVector3(\s)/", 'replace' => $fod . '3$1'),               // CVector3 to built in float3
		array('find' => "/CVector4\(\)/", 'replace' => '(' . $fod . '4) {}'),  // CVector3 default constructor to built in float3
		array('find' => "/CVector4\(($multChain,$s$multChain,$s$multChain,$s$multChain)\)/", 'replace' => '(' . $fod . '4) {$1}'),  // CVector3 to built in float3
		array('find' => "/CVector4\(($all)\);/", 'replace' => '(' . $fod . '4) {$1};'),  // CVector4 to built in float4
		array('find' => "/CVector4(\s)/", 'replace' => $fod . '4$1'),               // CVector4 to built in float4
		array('find' => "/($var)\.Length\(\)/", 'replace' => 'length($1)'),       // CVector3 Length() to built in length
		array('find' => "/($var)\.Dot\(/", 'replace' => 'dot($1, '),              // CVector3 Dot() to built in dot
		array('find' => "/($var)\.Cross\(/", 'replace' => 'cross($1, '),          // CVector3 Cross() to built in cross
		array('find' => "/($var)\.RotateVector\(/", 'replace' => 'Matrix33MulFloat4($1, '), // CRotationMatrix33 to custom rotation function
		array('find' => "/($var)\.RotateX\(/", 'replace' => '$1 = RotateX($1, '), // CRotationMatrix33 to custom rotation function
		array('find' => "/($var)\.RotateY\(/", 'replace' => '$1 = RotateY($1, '), // CRotationMatrix33 to custom rotation function
		array('find' => "/($var)\.RotateZ\(/", 'replace' => '$1 = RotateZ($1, '), // CRotationMatrix33 to custom rotation function
		array('find' => "/($var)\.RotateAroundVectorByAngle\(/", 'replace' => 'RotateAroundVectorByAngle4($1, '), // CVector3 to custom rotation function
		array('find' => "/($var)\.GetXYZ\(\)/", 'replace' => '$1.xyz'), // CVector4 getxyz to native accessor of float4
		array('find' => "/CRotationMatrix /", 'replace' => 'matrix33 '), // CRotationMatrix33 to matrix33
		array('find' => "/swap\(($var),\s($var)\);/", 'replace' => '{ ' . $fod . ' temp = $1; $1 = $2; $2 = temp; }'),// swap vals
		array('find' => "/($s|\()(-?\d+)f($s|;|\))/", 'replace' => '$1$2$3'),          // int vals should not have a "f" at the end
		// array('find' => "/sign\(($rval)\)$s\*$s($multChain)/", 'replace' => 'copysign($2, $1)'),// sign(x) * y => copysign(y, x) (this is wrong! probably copysign not usable at all)

		// from here on its getting messy
		array('find' => "/1.0f$s\/$s($rval)/", 'replace' => 'native_recip($1)'),        // native reciprocal
		array('find' => "/($rval)$s\/$s($rval)/", 'replace' => 'native_divide($1, $2)'),        // native division
		array('find' => "/($preF)native_recip\(native_sqrt($rval)\)/", 'replace' => '$1native_rsqrt$2'),        // native reciprocal sqrt
		array('find' => "/($preF)native_sqrt\(native_recip($rval)\)/", 'replace' => '$1native_rsqrt$2'),        // native reciprocal sqrt

		// mad (literally ;D )
		array('find' => "/\(($multChain)$s\*$s($rval)$s\+$s($multChain)\)/", 'replace' => '(mad($1, $2, $3))'),    // (a * b + c) ====> mad(a, b, c)
		array('find' => "/\(($multChain)$s\*$s($rval)$s\-$s($multChain)\)/", 'replace' => '(mad($1, $2, -$3))'),    // (a * b - c) ====> mad(a, b, -c)
		array('find' => "/([^*-]$s)($multChain)$s\*$s($rval)$s\+$s($multChain)(${'s'}[^*]|;)/", 'replace' => '$1mad($2, $3, $4)$5'), // a * b + c ====> mad(a, b, c)
		array('find' => "/([^*-]$s)($multChain)$s\*$s($rval)$s\-$s($multChain)(${'s'}[^*]|;)/", 'replace' => '$1mad($2, $3, -$4)$5'), // a * b - c ====> mad(a, b, -c)
		array('find' => "/\(($multChain)$s\+$s($rval)$s\*$s($multChain)\)/", 'replace' => '(mad($2, $3, $1))'),    // (c + a * b) ====> mad(a, b, c)
		array('find' => "/\(($multChain)$s\-$s($rval)$s\*$s($multChain)\)/", 'replace' => '(mad(-$2, $3, $1))'),    // (c - a * b) ====> mad(-a, b, c)
		array('find' => "/([^*-]$s)($multChain)$s\+$s($multChain)$s\*$s($rval)(${'s'}[^*]|;)/", 'replace' => '$1mad($4, $3, $2)$5'), // a * b + c ====> mad(a, b, c)
		array('find' => "/([^*-]$s)($multChain)$s\-$s($multChain)$s\*$s($rval)(${'s'}[^*]|;)/", 'replace' => '$1mad(-$4, $3, $2)$5'), // c - a * b ====> mad(-a, b, c)

		// formula specific replacements
		array('find' => "/^void(\s)/", 'replace' => $fod . '4 $1'), // mark void with inline void
		array('find' => "/" . $fod . "4 &z/", 'replace' => $fod . '4 z'), // no passing by reference
		array('find' => "/" . $fod . " &w/", 'replace' => $fod . ' *w'), // no passing by reference
		//array('find' => "/z\./", 'replace' => 'z->'),
		array('find' => "/" . $fod . "4 &z4D/", 'replace' => $fod . '4 *z4D'), // no passing by reference
		array('find' => "/z4D\./", 'replace' => 'z4D->'),
		array('find' => "/sExtendedAux &aux/", 'replace' => 'sExtendedAuxCl *aux'), // no passing by reference
		array('find' => "/const sFractal \*fractal/", 'replace' => '__constant sFractalCl *fractal'), // no passing by reference
		array('find' => "/aux\./", 'replace' => 'aux->'),
		array('find' => "/const(\s)/", 'replace' => '__constant$1'), // constant function parameter
		//array('find' => "/(\s)z\s=/", 'replace' => '$1*z ='), // z to pointer
		//array('find' => "/(\s)z\s(.)=/", 'replace' => '$1*z $2='), // z to pointer
		//array('find' => "/([\s\(-])z([,\);\s}])/", 'replace' => '$1*z$2'), // z to pointer
		array('find' => "/(\s)z4D\s=/", 'replace' => '$1*z4D ='), // z4D to pointer
		array('find' => "/(\s)z4D\s(.)=/", 'replace' => '$1*z4D $2='), // z4D to pointer
		array('find' => "/([\s\(-])z4D([,\);\s}])/", 'replace' => '$1*z4D$2'), // z4D to pointer
		array('find' => "/(\s)w\s=/", 'replace' => '$1*w ='), // w to pointer
		array('find' => "/(\s)w\s(.)=/", 'replace' => '$1*w $2='), // w to pointer
		array('find' => "/([\s\(-])w([,\);\s}])/", 'replace' => '$1*w$2'), // w to pointer
		array('find' => "/case ([a-zA-Z]+[a-zA-Z0-9_]+?[^l])(_[a-zA-Z0-9]+):/", 'replace' => 'case $1Cl$2:'), // replace enum switch cases with cl version
		array('find' => "/== ([a-zA-Z]+[a-zA-Z0-9_]+?[^l])(_[a-zA-Z0-9]+)\)/", 'replace' => '== $1Cl$2)'), // replace enum if comparison with cl version
		array('find' => "/($s)(enum[a-zA-Z0-9_]+?[^l])($s)/", 'replace' => '$1$2Cl$3'), // replace enum definitions with cl version
		array('find' => "/M_PI([\s\)\};,])/", 'replace' => ($mode == 'single' ? 'M_PI_F$1' : 'M_PI$1')), // replace Math constant
		array('find' => "/1e-061f/", 'replace' => ($mode == 'single' ? '1e-030f' : '1e-061')), // replace minimal double constant
		// TODO more replacements
	);

	foreach ($cppToOpenCLReplaceLookup as $item) {
		$code = preg_replace($item['find'], $item['replace'], $code);
		$code = preg_replace($item['find'], $item['replace'], $code); // regex sometimes overlap, so run twice!
	}
	$code = substr($code, 0, -1) . 'return z;' . PHP_EOL . '}';
	return $code;
}

function generate_formula_icon($formula, $imgPath)
{
	$formulaId = $formula['id'];
	if ($formula['type'] == 'transf') {
		$settings = '# Mandelbulber settings file
# version 2.09
# only modified parameters
[main_parameters]
ambient_occlusion_enabled true;
camera 1,079630935663999 -2,590704819137228 1,163428502911517;
camera_distance_to_target 7,000000000000004;
camera_rotation 23,56505117707793 -24,60154959902026 0;
camera_top -0,1664347300812756 0,3815883285622319 0,9092248501486611;
flight_last_to_render 0;
formula_1 7;
formula_2 ' . $formulaId . ';
hdr true;
hybrid_fractal_enable true;
image_height 256;
image_width 256;
keyframe_last_to_render 0;
target -1,464862100002681 3,2431066939054 -1,750709177730066;
[fractal_1]
[fractal_2]
info true;
mandelbox_folding_limit -0,5;
mandelbox_folding_value 0;
transf_addition_constant_0000 0,1 -0,5 0,1 -0,2;
transf_addition_constant_111 0,135135 1 1;
transf_function_enabledAz_false true;
transf_int_A 5;
transf_int_B 5;
transf_offset 0,7;
transf_offset_0 0,8;';
	} else {
		$settings = '# Mandelbulber settings file
# version 2.09
# only modified parameters
[main_parameters]
ambient_occlusion_enabled true;
camera 1,080915186153033 -1,903722297292515 1,202015809937754;
camera_distance_to_target 6,277124159763113;
camera_rotation 29,9681686749304 -29,10022837829406 0;
camera_top -0,2429354017345652 0,421316840960071 0,8737702845184721;
DE_factor 0,18197;
flight_last_to_render 0;
formula_1 ' . $formulaId . ';
hdr true;
hybrid_fractal_enable true;
image_height 256;
image_width 256;
keyframe_last_to_render 0;
mat1_coloring_palette_offset 1,6;
target -1,65882778581033 2,847745976777177 -1,850793618305075;
[fractal_1]
transf_function_enabled_false true;
[fractal_2]
info true;
mandelbox_folding_limit -0,5;
mandelbox_folding_value 0;
transf_addition_constant 1,935484 0 0;
transf_addition_constant_0000 0,1 -0,5 0,1 -0,2;
transf_constant_multiplier_111 2,934272 2,089201 1,877934;
transf_function_enabledy_false true;
transf_function_enabledz_false true;
transf_int_A 5;
transf_int_B 5;
transf_offset 0,7;
transf_offset_0 0,8;
transf_scale_0 0,034722;
transf_scale_2 1,079812;';
	}
	$tempFractPath = PROJECT_PATH . 'formula/img/' . $formula['internalName'] . '.fract';

	if (!file_exists($tempFractPath)) { // allow manual override
		file_put_contents($tempFractPath, $settings);
	}
	if (!file_exists(MANDELBULBER_EXEC_PATH)) return false;
	$cmd = MANDELBULBER_EXEC_PATH . " -n -f png16alpha -o '" . $imgPath . "' '" . $tempFractPath . "'";
	// echo PHP_EOL . $cmd . PHP_EOL;
	shell_exec($cmd);
	shell_exec("convert '" . $imgPath . "' -depth 8 '" . $imgPath . "'"); // save disk space with 8-bit png
	return true;
}

function upgradeInternalName($internalName, $internalNameNew)
{
	shell_exec('git mv'
		. ' \'' . PROJECT_PATH . 'formula/ui/fractal_' . $internalName . '.ui\''
		. ' \'' . PROJECT_PATH . 'formula/ui/fractal_' . $internalNameNew . '.ui\''
	);
	shell_exec('git mv'
		. ' \'' . PROJECT_PATH . 'formula/img/' . $internalName . '.png\''
		. ' \'' . PROJECT_PATH . 'formula/img/' . $internalNameNew . '.png\''
	);
	if (file_exists(PROJECT_PATH . 'formula/img/' . $internalName . '.fract')) {
		shell_exec('git mv'
			. ' \'' . PROJECT_PATH . 'formula/img/' . $internalName . '.fract\''
			. ' \'' . PROJECT_PATH . 'formula/img/' . $internalNameNew . '.fract\''
		);
	}
	$fractal_list_content = file_get_contents(PROJECT_PATH . 'src/fractal_list.cpp');
	$fractal_list_content = str_replace('"' . $internalName . '"', '"' . $internalNameNew . '"', $fractal_list_content);
	file_put_contents(PROJECT_PATH . 'src/fractal_list.cpp', $fractal_list_content);
}

function upgradeFunctionName($functionName, $functionNameNew)
{
	$replaceInFiles = array(
		PROJECT_PATH . 'src/compute_fractal.cpp',
		PROJECT_PATH . 'src/fractal_formulas.cpp',
		PROJECT_PATH . 'src/fractal_formulas.hpp',
		PROJECT_PATH . 'src/fractal_list.cpp',
	);
	foreach ($replaceInFiles as $replaceInFile) {
		$fileContent = file_get_contents($replaceInFile);
		$fileContent = str_replace($functionName, $functionNameNew, $fileContent);
		file_put_contents($replaceInFile, $fileContent);
	}
}

function from_camel_case($input)
{
	preg_match_all('@([A-Z][A-Z0-9]*(?=$|[A-Z][a-z0-9])|[A-Za-z][a-z0-9]+)@', $input, $matches);
	$ret = $matches[0];
	foreach ($ret as &$match) {
		$match = $match == strtoupper($match) ? strtolower($match) : lcfirst($match);
	}
	return implode('_', $ret);
}

?>

