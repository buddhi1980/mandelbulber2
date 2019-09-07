#!/usr/bin/env php
################################# WORK IN PROGRESS
# this file takes a mandelbulb3d formula file (m3f) and does the following:
# - detect code part from m3f file
# 	- convert code hex string to binary
# 	- convert binary to intel hex
# 	- decompile intel hex to python code
# 	- TODO: further processing
# - detect options
# - TODO
#

<?php
require_once(dirname(__FILE__) . '/common.inc.php');
printStart();
$retDecBinPath = '/usr/local/bin'; // from here: https://github.com/avast/retdec
$bin2hexPath = '/usr/local/bin'; // from here https://github.com/bialix/intelhex
$mb3dFolder = @$argv[1]; // retrieve from here: https://github.com/coast77777777777/mb3d, see folder M3Formulas
$targetFolder = dirname(__FILE__) . '/../formula/mb3dTemporaryFormulas';
if(substr($mb3dFolder, -1, 1) != '/') $mb3dFolder .= '/';
$i = 0;
if(!file_exists($targetFolder)) mkdir($targetFolder);
foreach (glob($mb3dFolder . "M3Formulas/*.m3f") as $filename) {
        // if(stripos($filename, 'quick') === false) continue; // test with simple formula: "quickdudley"
	echo "Handling: $filename" . PHP_EOL;
	portFormulaFromFile($filename);
	$i++;
}

function portFormulaFromFile($fileName)
{
    global $targetFolder;
    $contents = file_get_contents($fileName);
    $code = detectCodePY($contents);

    // $cFilePath = $targetFolder . '/' . str_ireplace('.m3f', '.py', basename($fileName));
    // file_put_contents($cFilePath, $code);

    $options = detectOptions($contents);
    preg_match('/def\s*function_0.*([\s\S]+?)# -------/', $code, $match);
    $codeePart = $match[1];

    $cCode = transpilePythonToC($codeePart);
    $headerFilePath = $targetFolder . '/' . str_ireplace('.m3f', '.h', basename($fileName));
    file_put_contents($headerFilePath, $cCode);

    // $constants = detectConstants(); // TODO
    // $description = detectDescription(); // TODO


    $cCode = detectCodeC($contents);
    $cFilePath = $targetFolder . '/' . str_ireplace('.m3f', '.c', basename($fileName));
    file_put_contents($cFilePath, $cCode);
 
    $asmCode = file_get_contents('/tmp/code.ll');
    $asmFilePath = $targetFolder . '/' . str_ireplace('.m3f', '.asm', basename($fileName));
    file_put_contents($asmFilePath, $asmCode);
}

function transpilePythonToC($code){
    // add semicolor at end of non empty lines
    $lines = explode(PHP_EOL, $code);
    foreach ($lines as &$line){
        if(trim($line) != ''){
			$line = $line .';';
        }
    }
    $code = implode(PHP_EOL, $lines);
    $code = str_replace('eax', 'z.x', $code);
    $code = str_replace('edx', 'z.y', $code);
    $code = str_replace('ecx', 'z.z', $code);
    $code = str_replace('arg1[24]', 'j.x', $code);
    $code = str_replace('arg1[32]', 'j.y', $code);
    $code = str_replace('arg1[40]', 'j.z', $code);
    
    $cCodeFormula = 'const inline void FormulaCode(CVector4 &z, const sFractal *fractal, sExtendedAux &aux) override
	{
        ' . $code . '
	}';
    return $cCodeFormula;
}

function detectCodePY($contents){
	global $bin2hexPath, $retDecBinPath;
	preg_match('/\[CODE\]([\s\S]+)\[END\]/', $contents, $match);
	$code = trim($match[1]);
	//echo 'Detected following code:' . PHP_EOL . $code;

	// echo 'Convert code to binary...' . PHP_EOL;
	file_put_contents('/tmp/code.hex', $code);
	shell_exec('xxd -r -p /tmp/code.hex >/tmp/code.bin');

	// echo 'Convert binary to intel hex...' . PHP_EOL;
	shell_exec($bin2hexPath . '/bin2hex.py /tmp/code.bin > /tmp/code.ihex');
	// echo 'Created following intelHex:' . file_get_contents('/tmp/code.ihex');

	// echo 'Decompile intel hex to python ...' . PHP_EOL;
	$cmdFlags = [];
	$cmdFlags[] = '-a x86'; // target x86-64 processors
	$cmdFlags[] = '-e little'; // set endianness to little (pascal in windows is source)
	//$cmdFlags[] = '--backend-no-symbolic-names'; // keep original names (local variables)
	$cmdFlags[] = '--backend-no-var-renaming'; // keep original names (registers)
	// $cmdFlags[] = '--backend-no-opts'; // needs to be optimized, keep this commented out
	// $cmdFlags[] = '--backend-aggressive-opts'; // breaks the code?
	// $cmdFlags[] = '--backend-keep-all-brackets'; // no, we dont want brackets
	$cmdFlags[] = '--backend-keep-library-funcs'; // why not?
	// $cmdFlags[] = '--backend-no-compound-operators'; // no effect?
	// $cmdFlags[] = '--backend-no-time-varying-info'; // no effect?
	// $cmdFlags[] = '--backend-strict-fpu-semantics'; // no effect?
	$cmdFlags[] = '-l py'; // set the target language

	$cmdFlags[] = '-o /tmp/code.res'; // set the output file
	$cmdDec = $retDecBinPath . '/retdec-decompiler.py ' . implode(' ', $cmdFlags) . ' /tmp/code.ihex';
	shell_exec($cmdDec);
	// echo $cmdDec;
	//echo 'Created following code:' . PHP_EOL;
	$code = file_get_contents('/tmp/code.res');
	//echo $code;
	// die($code);
	return $code;
}

function detectCodeC($contents){
    global $bin2hexPath, $retDecBinPath;
    preg_match('/\[CODE\]([\s\S]+)\[END\]/', $contents, $match);
    $code = trim($match[1]);
    //echo 'Detected following code:' . PHP_EOL . $code;
    
    // echo 'Convert code to binary...' . PHP_EOL;
    file_put_contents('/tmp/code.hex', $code);
    shell_exec('xxd -r -p /tmp/code.hex >/tmp/code.bin');
    
    // echo 'Convert binary to intel hex...' . PHP_EOL;
    shell_exec($bin2hexPath . '/bin2hex.py /tmp/code.bin > /tmp/code.ihex');
    // echo 'Created following intelHex:' . file_get_contents('/tmp/code.ihex');
    
    // echo 'Decompile intel hex to python ...' . PHP_EOL;
    $cmdFlags = [];
    $cmdFlags[] = '-a x86'; // target x86-64 processors
    $cmdFlags[] = '-e little'; // set endianness to little (pascal in windows is source)
    //$cmdFlags[] = '--backend-no-symbolic-names'; // keep original names (local variables)
    //$cmdFlags[] = '--backend-no-var-renaming'; // keep original names (registers)
    $cmdFlags[] = '--backend-no-opts'; // needs to be optimized, keep this commented out
    // $cmdFlags[] = '--backend-aggressive-opts'; // breaks the code?
    // $cmdFlags[] = '--backend-keep-all-brackets'; // no, we dont want brackets
    $cmdFlags[] = '--backend-keep-library-funcs'; // why not?
    // $cmdFlags[] = '--backend-no-compound-operators'; // no effect?
    // $cmdFlags[] = '--backend-no-time-varying-info'; // no effect?
    // $cmdFlags[] = '--backend-strict-fpu-semantics'; // no effect?
    $cmdFlags[] = '-l c'; // set the target language
    
    $cmdFlags[] = '-o /tmp/code.res'; // set the output file
    $cmdDec = $retDecBinPath . '/retdec-decompiler.py ' . implode(' ', $cmdFlags) . ' /tmp/code.ihex';
    shell_exec($cmdDec);
    // echo $cmdDec;
    //echo 'Created following code:' . PHP_EOL;
    $code = file_get_contents('/tmp/code.res');
    //echo $code;
    // die($code);
    return $code;
}

function detectOptions($content){
	$options = [];
	if(preg_match('/\[OPTIONS\]\n((?:[\.A-Za-z ]+\s*=\s*[\d.]+\n)*)/', $content, $match)){
		$optionsRaw = $match[1];
		$optionsLines = explode(PHP_EOL, $optionsRaw);
		foreach($optionsLines as $optionsLine){
			if(preg_match('/\.([A-Za-z ]+)\s*=\s*([\d\.]+)/', $optionsLine, $matchOption)){
				$options[$matchOption[1]] = $matchOption[2];
			}
		}
	}
	return $options;
}
printFinish();
exit;

?>

