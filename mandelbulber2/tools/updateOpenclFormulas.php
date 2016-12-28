<?php

// under construction ...

$code = '// if (aux.r < 1e-21) aux.r = 1e-21;
	double th0 = asin(z.z / aux.r) + fractal->bulb.betaAngleOffset;
	double ph0 = atan2(z.y, z.x) + fractal->bulb.alphaAngleOffset;
	double rp = pow(aux.r, fractal->bulb.power - 1.0);
	double th = th0 * fractal->bulb.power;
	double ph = ph0 * fractal->bulb.power;
	double cth = cos(th);
	aux.r_dz = rp * aux.r_dz * fractal->bulb.power + 1.0;
	rp *= aux.r;
	z = CVector3(cth * cos(ph), cth * sin(ph), sin(th)) * rp;
';

// regex for a var name
$var = '[A-Za-z][A-Za-z0-9\.\-\>]*';

$cppToOpenCLReplaceLookup = array(
	array('find' => '/double/', 'replace' => 'float'),				// all doubles to floats
	array('find' => "/($var)\s\/\s($var)/", 'replace' => 'native_divide($1, $2)'),	// native division
	array('find' => "/(\s)sin\(/", 'replace' => '$1native_sin('),			// native sin
	array('find' => "/(\s)cos\(/", 'replace' => '$1native_cos('),			// native cos
	array('find' => "/(\s)pow\(/", 'replace' => '$1native_powr('),			// native pow
	array('find' => "/CVector3\((.*)\)/", 'replace' => '(float3) {$1}'),		// CVector3 to built in float3
	array('find' => "/([\s\(\{])([0-9\.]+)([\s\)\};])/", 'replace' => '$1$2f$3'),	// double literals to float literals
);

foreach($cppToOpenCLReplaceLookup as $item){
	$code = preg_replace($item['find'], $item['replace'], $code);
}

echo $code;
