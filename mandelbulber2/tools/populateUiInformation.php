#!/usr/bin/env php
#
# this file takes the formulas in src/fractal_formulas.cpp and does a couple of things:
# - generates the information boxes in the ui from the source code
# - generates opencl formulas
#
# requires highlight package and php (apt-get install highlight php5-cli)
# 
# on default this script runs dry, 
# it will try to parse all formulas and show which ui files would be modified
# this should always be run first, to see if any issues occur
# if you invoke this script with "nondry" as cli argument it will write changes to ui files
#

<?php
define('PROJECT_PATH', realpath(dirname(__FILE__)) . '/../');

$formulas = array();

$fractalListContent = file_get_contents(PROJECT_PATH .'src/fractal_list.cpp');
$fractalFunctionLookup = file_get_contents(PROJECT_PATH .'src/compute_fractal.cpp');
$fractalFunctionContent = file_get_contents(PROJECT_PATH .'src/fractal_formulas.cpp');
$fractalFunctionContentFormulasOnly = substr($fractalFunctionContent, 
	strpos($fractalFunctionContent, 'using namespace fractal;') + strlen('using namespace fractal;'));
$fractalFunctionContentExploded = explode(PHP_EOL . '}' . PHP_EOL, $fractalFunctionContentFormulasOnly);
$formulaExampleUsage = getFormulaExampleUsage();
$indexIdLookup = getIndexIdLookUp();

// parse fractal_list data
$regexFormulas = '/sFractalDescription\(([\s\S]*?)\)\);/';
preg_match_all($regexFormulas, $fractalListContent, $formula_matches);

foreach($formula_matches[0] as $key => $formulaMatch){
	if($key == 0) continue; // skip formula "none"
	
	// read index and name from fractal_list	
	preg_match('/"([a-zA-Z0-9 _\'-\^]+)"[\s\S]*?"([a-zA-Z0-9 _-]+)"[\s\S]*?([a-zA-Z0-9 _-]+)/', $formulaMatch, $matches);
	if(count($matches) < 4) die('could not read index for formula : ' . $formulaMatch);
	$index = trim($matches[3]);
	$internalName = trim($matches[2]);
	$name = trim($matches[1]);

	// find associated functions
	$functionNameMatchString = '/case ' . $index . '\s*:[\s\S]+?{[\s\S]*?\n[\s]+([a-zA-Z0-9]+)\(/';
	preg_match($functionNameMatchString, $fractalFunctionLookup, $matchFunctionName);
	if(count($matchFunctionName) < 2){
		echo errorString('Warning, could not read function name for index: ' . $index) . PHP_EOL;		
		continue;
		// die('could not read function name for index: ' . $index);
	}
	$functionName = trim($matchFunctionName[1]);

	// read function contents
	$functionContentMatchString = '/(\/\*\*[\s\S]+?\*\/)[\s\S]*(void ' . $functionName . '\([\s\S]*)/';
	$functionContentFound = false;
	foreach($fractalFunctionContentExploded as $functionKey => $functionValue){
		if(preg_match($functionContentMatchString, $functionValue, $matchFunctionContent)){
			$functionContentFound = true;
			$rawComment = $matchFunctionContent[1];
			$comment = parseComment($matchFunctionContent[1]);
			$code = $matchFunctionContent[2] . PHP_EOL . '}';
		}
	}

	if(!$functionContentFound){
		echo errorString('Warning, could not read code for index: ' . $index) . PHP_EOL;
		continue;
		// die('could not read code for index: ' . $index);
	}
	
	$formulas[$index] = array(
		'uiFile' => PROJECT_PATH . 'qt_data/fractal_' . $internalName . '.ui',
		'name' => $name,
		'internalName' => $internalName,
		'functionName' => $functionName,
		'code' => $code,
		'comment' => $comment,
		'rawComment' => $rawComment,
		'id' => $indexIdLookup[$index],
		'openclFile' => PROJECT_PATH . 'opencl/formula/fractal_' . $internalName . '.cl',
		'openclCode' => parseToOpenCL($code),
		'type' => (strpos($internalName, 'transf_') !== false ? 'transf' : 'formula'),
	);
	// print_r($formulas); exit;
}

// information boxes in the ui
foreach($formulas as $index => $formula){
	$formattedEscapedCode = getFormatCode($formula['code']);
	// remove hardcoded font-size, to use system defined font-size
	$formattedEscapedCode = str_replace ('font-size:10pt;', '', $formattedEscapedCode);

	// extract only body element, since html part and comment change between versions of highlight
	$startCode = strpos($formattedEscapedCode, '<body');
	$endCode = strpos($formattedEscapedCode, '</body>') + strlen('</body>');
	$formattedEscapedCode = substr($formattedEscapedCode, $startCode, $endCode - $startCode);

	$comment = $formula['comment'];
	if(!empty($comment['description'])){
		$informationText = '<p>' . implode('<br>', $comment['description']) . '</p>';
	}
	$informationText .= "<table>" . PHP_EOL;
	// $informationText .= "<tr><th>Name</th><td>" . $formula['name'] . "</td></tr>" . PHP_EOL;

	if(!empty($comment['reference'])){
		$informationText .= '<tr><th>Reference</th><td>';
		foreach($comment['reference'] as $ref){
			$informationText .= "<a href=\"" . $ref . "\">" . $ref . "<br>" . PHP_EOL;
		}
		$informationText .= '</td></tr>' . PHP_EOL;
	}
	if(!empty($comment['author'])){
		$informationText .= '<tr><th>Author</th><td>';
		foreach($comment['author'] as $author){
			$informationText .= $author . "<br>" . PHP_EOL;
		}
		$informationText .= '</td></tr>' . PHP_EOL;
	}	
	$informationText .= "</table>" . PHP_EOL;
	if(array_key_exists($formula['id'], $formulaExampleUsage)){
		$informationText .= '<br><b>Examples using this formula</b><br>';
		$exampleFilenames = $formulaExampleUsage[$formula['id']];		
		if(count($exampleFilenames) > 5){ // do not show more than 5 examples
			array_splice($exampleFilenames, 5);
			$exampleFilenames[] = '...';
		}
		$informationText .= implode('<br>', $exampleFilenames);
	}else{
		if(isWarning()){
			echo warningString('formula ' . $formula['name'] . ' is not used in any examples yet.') . PHP_EOL;
		}
	}

	$informationText .= "<h3>Code</h3>" . PHP_EOL;
	$informationText .= $formattedEscapedCode;
	
	$uiFileContent = file_get_contents($formula['uiFile']);
	$regexInformation = '/(<widget class="MyGroupBox" name="groupCheck_info">[\s\S]+)(<widget class="QLabel"[\s\S]+?<\/widget>)/';
     
      	$replacement = '$1<widget class="QLabel" name="label_code_content">
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
       </widget>';
	$newUiFileContent = preg_replace($regexInformation, $replacement, $uiFileContent, -1, $count);
	$newUiFileContent = preg_replace('/<class>.+<\/class>/', '<class>' . $formula['internalName'] . '</class>', $newUiFileContent, 1);
	$postitionOfWindowTitle = strpos($newUiFileContent, 'windowTitle ');
	$postitionOfWindowTitleStringStart = strpos($newUiFileContent, '<string', $postitionOfWindowTitle);
	$postitionOfWindowTitleStringEnd = strpos($newUiFileContent, '</string>', $postitionOfWindowTitle) + strlen('</string>');
	$newUiFileContent = substr_replace($newUiFileContent, '<string notr="true">' . $formula['internalName'] . '</string>', 
		$postitionOfWindowTitleStringStart, $postitionOfWindowTitleStringEnd - $postitionOfWindowTitleStringStart);	
	if($count == 0){
		echo errorString('Warning, could not replace code in ui file for index: ' . $index) . PHP_EOL;
		continue;	
	}

  $replaceFormulaLookup = array(
	  array('find' => '/\>\s*([^<]+?)\s*<\/string>/', 'replace' => '>$1</string>'), // whitespace fix 1
		array('find' => '/\>\s*([^<]+?)\s*:\s*<\/string>/', 'replace' => '>$1:</string>'), // whitespace fix 2
		array('find' => '/<string>(.*?)\s(\s.*?)<\/string>/', 'replace' => '<string>$1$2</string>'), // whitespace fix 3
		array('find' => '/<string>fabs\s*\(\s*(.*?)\s*\)\s*<\/string>/', 'replace' => '<string>fabs($1)</string>'),
		array('find' => '/<string>abs\s*\(\s*(.*?)\s*\)\s*<\/string>/', 'replace' => '<string>abs($1)</string>'),
		array('find' => '/<string>(.*?)::(.*?)<\/string>/', 'replace' => '<string>$1:$2</string>'),
		array('find' => '/<string>Menger Scale<\/string>/', 'replace' => '<string>Menger Scale:</string>'),
		array('find' => '/<string>Menger offsets:<\/string>/', 'replace' => '<string>Menger Offset:</string>'),
		array('find' => '/<string>DE tweak temp<\/string>/', 'replace' => '<string>DE tweak temp:</string>'),
		array('find' => '/<string>Offset<\/string>/', 'replace' => '<string>Offset:</string>'),
		array('find' => '/Rotation;<\/string>/', 'replace' => 'Rotation</string>'),
		array('find' => '/>Multiplier<\/string>/', 'replace' => '>Multiplier:</string>'),
	);
	foreach($replaceFormulaLookup as $item){
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

  foreach($notrRemoveDoublePointCI as $item){
	  $newUiFileContent = preg_replace('/<string>'. $item .':*;*<\/string>/i',
		  '<string notr="true">' . str_replace('\\', '', $item) . '</string>', $newUiFileContent);
	}
 
  // simple find and replace whole string
	$simpleReplace = array(
	  array('find' => 'Type 4 Fold Value', 'replace' => 'Type 4 Fold value:'),
		array('find' => 'Type 5 Fold2 Value', 'replace' => 'Type 5 Fold2 value:'),
	);

  foreach($simpleReplace as $item){
	  $newUiFileContent = str_replace($item['find'], $item['replace'], $newUiFileContent);
	}

	if($newUiFileContent == $uiFileContent){
		if(isVerbose()){
			echo noticeString('formula ' . $formula['name'] . ' has not changed.') . PHP_EOL;
		}
		continue;
	}
	if(!isDryRun()){
		file_put_contents($formula['uiFile'], $newUiFileContent);
	}
	echo successString('formula ' . $formula['name'] . ' changed.') . PHP_EOL;
}

// opencl formulas
foreach($formulas as $index => $formula){
  // TODO: autoport structs from fractal.h and add start conditions
	// for each formula add maxiter / bailout, orbittrap, distance calculation, coloring
	// add primitives
	// add more expressions to $cppToOpenCLReplaceLookup to make all kernels executable and add all possible native functions
	// TO DISCUSS: put whole kernel into one file for each formula, 
	// or put common logic (eg. orbittrap) into one opencl snippet
	// and concat kernel on the fly like in mandelbulber 1.*?

	$fileHeader = '/**
 * Mandelbulber v2, a 3D fractal generator  _%}}i*<.
 * Copyright (C) ' . date('Y') . ' Mandelbulber Team   _>]|=||i=i<,
 *                                        \><||i|=>>%)
 * This file is part of Mandelbulber.     )<=i=]=|=i<>
 * The project is licensed under GPLv3,   -<>>=|><|||`
 * see also COPYING file in this folder.    ~+{i%+++
 * ' . str_replace('/**', '', $formula['rawComment']) . PHP_EOL . PHP_EOL;
	$autogenLine = '/* ### This file has been autogenerated. Remove this line, to prevent override. ### */' . PHP_EOL . PHP_EOL;
	$openclContent = @file_get_contents($formula['openclFile']);
	$newOpenCLContent = $fileHeader . $autogenLine;
	$newOpenCLContent .= $formula['openclCode'];

	// clang-format
	$filepathTemp = $formula['openclFile'] . '.tmp.c';
	file_put_contents($filepathTemp, $newOpenCLContent);
	shell_exec('clang-format -i --style=file ' . escapeshellarg($filepathTemp));
	$newOpenCLContent = file_get_contents($filepathTemp);
	unlink($filepathTemp); // nothing to see here :)

	$newOpenCLContentWithoutDateLine = preg_replace('/Copyright\s\(C\)\s\d+/', '', $newOpenCLContent);
	$openclContentWithoutDateLine = preg_replace('/Copyright\s\(C\)\s\d+/', '', $openclContent);

	if($newOpenCLContentWithoutDateLine == $openclContentWithoutDateLine){
		if(isVerbose()){
			echo noticeString('opencl ' . $formula['name'] . ' has not changed.') . PHP_EOL;
		}
		continue;
	}
	else if(!empty($openclContent) && strpos($openclContent, $autogenLine) === false){
		if(isVerbose()){
			echo noticeString('opencl ' . $formula['name'] . ' has been manually modified, will not override.') . PHP_EOL;
		}
		continue;
	}
	if(!isDryRun()){
	  file_put_contents($formula['openclFile'], $newOpenCLContent);
		// file_put_contents($formula['openclFile'] . '.orig', $formula['code']);
	}
	echo successString(basename($formula['openclFile']) . ' changed.') . PHP_EOL;
}

// fractal h
$fractalHContent = file_get_contents(PROJECT_PATH . 'src/fractal.h');
$fractalHReplaceLookup = array(
    array('find' => '/(\s)int(\s)/', 'replace' => '$1cl_int$2'),
    array('find' => '/(\s)bool(\s)/', 'replace' => '$1cl_int$2'),
    array('find' => '/(\s)double(\s)/', 'replace' => '$1cl_float$2'),
    array('find' => '/(\s)CVector3(\s)/', 'replace' => '$1cl_float3$2'),
    array('find' => '/(\s)CVector4(\s)/', 'replace' => '$1cl_float4$2'),
    array('find' => '/(\s)CRotationMatrix(\s)/', 'replace' => '$1matrix33$2'),
    array('find' => '/struct\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})/', 'replace' => "typedef struct\n$2$3 sCl$1"),
    array('find' => '/enum\s([a-zA-Z0-9_]+)\n(\s*)({[\S\s]+?\n\2})/', 'replace' => "typedef enum\n$2$3 eCl$1"),
);
foreach($fractalHReplaceLookup as $item){
	$fractalHContent = preg_replace($item['find'], $item['replace'], $fractalHContent);
}
// clang-format
$filepathTemp = PROJECT_PATH . 'opencl/fractalCl.h' . '.tmp.c';
file_put_contents($filepathTemp, $fractalHContent);
shell_exec('clang-format -i --style=file ' . escapeshellarg($filepathTemp));
$fractalHContent = file_get_contents($filepathTemp);
unlink($filepathTemp); // nothing to see here :)
file_put_contents(PROJECT_PATH . 'opencl/fractalCl.h', $fractalHContent);


// formula icons
foreach($formulas as $index => $formula){
	// autogenerate missing formula_and_transform_images
        $imgPath = PROJECT_PATH .'qt_data/formula_and_transform_images/' . $formula['internalName'] . '.png';
        if(file_exists($imgPath)){
            if(isVerbose()){
                    echo noticeString('image ' . $imgPath . ' already exists.') . PHP_EOL;
            }
            continue;
        }
	echo noticeString('image ' . $imgPath . ' does not exist, will generate...') . PHP_EOL;
        generate_formula_icon($formula, $imgPath);
	echo successString('image ' . $imgPath . ' generated.') . PHP_EOL;
}



if(isDryRun()){
	echo 'This is a dry run.' . PHP_EOL;
	echo 'If you want to apply the changes, execute with populateUiInformation.php nondry' . PHP_EOL;
}
else{
	echo 'Changes applied' . PHP_EOL;
}

function getFormatCode($code){
	$cmd = "echo " . escapeshellarg($code);
	$cmd .= " | sed 's/	/  /g' "; // replace tab with double space
	$cmd .= " | highlight -O html --style moria --inline-css --syntax cpp";
	return shell_exec($cmd);
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


function parseComment($c){
	$lines = explode(PHP_EOL, $c);
	$out = array();
	$tag = 'description';
	foreach($lines as $l){
		$line = trim($l);
		if(in_array($line, array('/**', '*', '*/'))) continue;
		if (preg_match("/\*[\s]+@([\S]+)[\s]*(.*)/", $line, $match)) {
			$tag = $match[1];
			if(!array_key_exists($tag, $out)) $out[$tag] = array();
			$text = trim($match[2]);
			if($text != '' || count($out[$tag]) > 0){
				$out[$tag][] = $match[2];
			}
		}else if (preg_match("/\*[\s]+(.*)/", $line, $match)) {
			$out[$tag][] = $match[1];
		}
	}
	return $out;
}

function getFormulaExampleUsage(){
	// get a lookup for every formula in which example files it is used
	$exampleBasePath = PROJECT_PATH .'deploy/share/mandelbulber2/examples/';
	$exampleFileMasks[] = $exampleBasePath . '*.fract';
	$exampleFileMasks[] = $exampleBasePath . '*/*.fract';
	$formulaExampleUsage = array();
	foreach($exampleFileMasks as $exampleFileMask){	
		foreach (glob($exampleFileMask) as $exampleFileName) {
			$exampleContent = file_get_contents($exampleFileName);
			$exampleContentLines = explode(PHP_EOL, $exampleContent);
			$pathOfFileFromExamples = str_replace($exampleBasePath, '', $exampleFileName);

			foreach ($exampleContentLines as $line) {
				$line = trim($line);
				if (preg_match("/^formula_\d+\s(\d+);$/", $line, $match)) {
					if(!array_key_exists($match[1], $formulaExampleUsage)) $formulaExampleUsage[$match[1]] = array();		
					if(!in_array($pathOfFileFromExamples, $formulaExampleUsage[$match[1]])){
						$formulaExampleUsage[$match[1]][] = $pathOfFileFromExamples;
					}
				}
			}
		}
	}
	return $formulaExampleUsage; 
}

function getIndexIdLookUp(){
	// get the integer id from fractal_list.hpp for the named index of each formula 
	$fractalListHeaderContent = file_get_contents(PROJECT_PATH .'src/fractal_list.hpp');
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

function parseToOpenCL($code){
	
	$var = '-?[A-Za-z_][A-Za-z0-9\.\-\>_]*'; // regex for a var name
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
	    array('find' => '/double/', 'replace' => 'float'),				// all doubles to floats
			array('find' => "/([\s\(\{])($double)([\s\)\};,])/", 'replace' => '$1$2f$3'),	// double literals to float literals
			array('find' => "/($preF)sin\(/", 'replace' => '$1native_sin('),          // native sin
			array('find' => "/($preF)cos\(/", 'replace' => '$1native_cos('),          // native cos
			array('find' => "/($preF)pow\(/", 'replace' => '$1native_powr('),         // native pow
			array('find' => "/($preF)sqrt\(/", 'replace' => '$1native_sqrt('),        // native sqrt
			array('find' => "/CVector3\(($all)\);/", 'replace' => '(float3) {$1};'),  // CVector3 to built in float3
			array('find' => "/CVector3(\s)/", 'replace' => 'float3$1'),               // CVector3 to built in float3
			array('find' => "/CVector4\(($all)\);/", 'replace' => '(float4) {$1};'),  // CVector4 to built in float4
			array('find' => "/CVector4(\s)/", 'replace' => 'float4$1'),               // CVector4 to built in float4
			array('find' => "/($var)\.Length\(\)/", 'replace' => 'length($1)'),       // CVector3 Length() to built in length
			array('find' => "/($var)\.Dot\(/", 'replace' => 'dot($1, '),              // CVector3 Dot() to built in dot
			array('find' => "/($var)\.Cross\(/", 'replace' => 'cross($1, '),          // CVector3 Cross() to built in cross
			array('find' => "/swap\(($var),\s($var)\);/", 'replace' => '{ float temp = $1; $2 = $1; $1 = temp; }'),// swap vals
			array('find' => "/($s|\()(\d+)f($s|;)/", 'replace' => '$1$2$3'),          // int vals should not have a "f" at the end
			array('find' => "/sign\(($rval)\)$s\*$s($multChain)/", 'replace' => 'copysign($2, $1)'),// sign(x) * y => copysign(y, x)

      // from here on its getting messy
			array('find' => "/1.0f$s\/$s($rval)/", 'replace' => 'native_recip($1)'),		// native reciprocal
			array('find' => "/($rval)$s\/$s($rval)/", 'replace' => 'native_divide($1, $2)'),		// native division
			array('find' => "/($preF)native_recip\(native_sqrt($rval)\)/", 'replace' => '$1native_rsqrt$2'),		// native reciprocal sqrt
			array('find' => "/($preF)native_sqrt\(native_recip($rval)\)/", 'replace' => '$1native_rsqrt$2'),		// native reciprocal sqrt

      // mad (literally ;D )
			array('find' => "/\(($multChain)$s\*$s($rval)$s\+$s($multChain)\)/", 'replace' => '(mad($1, $2, $3))'), 	// (a * b + c) ====> mad(a, b, c)
			array('find' => "/\(($multChain)$s\*$s($rval)$s\-$s($multChain)\)/", 'replace' => '(mad($1, $2, -$3))'), 	// (a * b - c) ====> mad(a, b, -c)
			array('find' => "/([^*]$s)($multChain)$s\*$s($rval)$s\+$s($multChain)(${'s'}[^*]|;)/", 'replace' => '$1mad($2, $3, $4)$5'), // a * b + c ====> mad(a, b, c)
			array('find' => "/([^*]$s)($multChain)$s\*$s($rval)$s\-$s($multChain)(${'s'}[^*]|;)/", 'replace' => '$1mad($2, $3, -$4)$5'), // a * b + c ====> mad(a, b, -c)
			array('find' => "/\(($multChain)$s\+$s($rval)$s\*$s($multChain)\)/", 'replace' => '(mad($2, $3, $1))'), 	// (c + a * b) ====> mad(a, b, c)
			array('find' => "/\(($multChain)$s\-$s($rval)$s\*$s($multChain)\)/", 'replace' => '(mad(-$2, $3, $1))'), 	// (c - a * b) ====> mad(-a, b, c)
			array('find' => "/([^*]$s)($multChain)$s\+$s($multChain)$s\*$s($rval)(${'s'}[^*]|;)/", 'replace' => '$1mad($4, $3, $2)$5'), // a * b + c ====> mad(a, b, c)
			array('find' => "/([^*]$s)($multChain)$s\-$s($multChain)$s\*$s($rval)(${'s'}[^*]|;)/", 'replace' => '$1mad(-$4, $3, $2)$5'), // c - a * b ====> mad(-a, b, c)

			// formula specific replacements
			array('find' => "/^void(\s)/", 'replace' => 'inline void$1'), // mark void with inline void
			array('find' => "/float3 &z/", 'replace' => 'global float3 *z'), // no passing by reference
			array('find' => "/sExtendedAux &aux/", 'replace' => 'global sExtendedAux *aux'), // no passing by reference
			array('find' => "/z\./", 'replace' => 'z->'),
			array('find' => "/aux\./", 'replace' => 'aux->'),
			array('find' => "/const(\s)/", 'replace' => '__constant$1'), // constant function parameter
			// TODO more replacements
		);

	foreach($cppToOpenCLReplaceLookup as $item){
		$code = preg_replace($item['find'], $item['replace'], $code);
		$code = preg_replace($item['find'], $item['replace'], $code); // regex sometimes overlap, so run twice!
	}
	return $code;
}


function generate_formula_icon($formula, $imgPath){
	$formulaId = $formula['id'];
	if($formula['type'] == 'transf'){
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
	}else{
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
        $tempFractPath = PROJECT_PATH .'qt_data/formula_and_transform_images/' . $formula['internalName'] . '.fract';
        
        if(!file_exists($tempFractPath)){ // allow manual override
            file_put_contents($tempFractPath, $settings);
        }
        $cmd = PROJECT_PATH ."Debug/mandelbulber2 -n -f png16alpha -o '" . $imgPath . "' '" . $tempFractPath . "'";
        echo PHP_EOL . $cmd . PHP_EOL;
        shell_exec($cmd);
        shell_exec("convert '" . $imgPath . "' -depth 8 '" . $imgPath . "'"); // save disk space with 8-bit png
}

?>

