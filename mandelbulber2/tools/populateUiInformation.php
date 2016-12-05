#!/usr/bin/env php
#
# this file generates the information boxes in the ui from the source code
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
		'id' => $indexIdLookup[$index],
	);
	// print_r($formulas); exit;
}

foreach($formulas as $index => $formula){
	$formattedEscapedCode = getFormatCode($formula['code']);
	// remove hardcoded font-size, to use system defined font-size
	$formattedEscapedCode = str_replace ('font-size:10pt;', '', $formattedEscapedCode);

	// extract only body element, since html part and comment change between versions of highlight
	$startCode = mb_strpos($formattedEscapedCode, '<body');
	$endCode = mb_strpos($formattedEscapedCode, '</body>') + mb_strlen('</body>');
	$formattedEscapedCode = mb_substr($formattedEscapedCode, $startCode, $endCode - $startCode);

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
		$informationText .= '<b>Examples using this formula</b><br>';
		$exampleFilenames = $formulaExampleUsage[$formula['id']];		
		if(count($exampleFilenames) > 5){ // do not show more than 5 examples
			array_splice($exampleFilenames, 5);
			$exampleFilenames[] = '...';
		}
		$informationText .= implode('<br>', $exampleFilenames);
	}else{
		echo warningString('formula ' . $formula['name'] . ' is not used in any examples yet.') . PHP_EOL;
	}

	$informationText .= "<h3>Code</h3>" . PHP_EOL;
	$informationText .= $formattedEscapedCode;
	
	$uiFileContent = file_get_contents($formula['uiFile']);
	$regexInformation = '/(<widget class="MyGroupBox" name="groupCheck_info">[\s\S]+)(<widget class="QLabel"[\s\S]+?<\/widget>)/';
     
      	$replacement = '$1<widget class="QLabel" name="label_code_content">
        <property name="text">
         <string>' . htmlentities($informationText) . '</string>
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
	if($count == 0){
		echo errorString('Warning, could not replace code in ui file for index: ' . $index) . PHP_EOL;
		continue;	
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

?>

