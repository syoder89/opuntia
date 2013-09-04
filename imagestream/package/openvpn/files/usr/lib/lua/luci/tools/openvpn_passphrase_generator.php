<?php

if (isset($argv)) {
	$secret_passwd = $argv[1];
} else {
	$secret_passwd = $_GET['phrase'];
}

$key = "#\n";
$key .= "# 2048 bit OpenVPN static key \n";
$key .= "#\n";
$key .= "-----BEGIN OpenVPN Static key V1 -----\n";
$line = '';
$key_size = 2048;

// Check for hex string. If not, md5 it
if (!(preg_match('/^[0-9a-fA-F]+$/', $secret_passwd)))
		$secret_passwd = md5($secret_passwd);
for ($i=0, $src=str_split($secret_passwd);$i<(($key_size/8)*2);$i++) {
		if ($i > 0 && (($i % 32) == 0)) {
				$key .= $line . "\n";
				$line = '';
		}
		if (empty($src))
				$src=str_split($secret_passwd);
		$line .= array_shift($src);
}
if (!empty($line))
		$key .= $line . "\n";
$key .= "-----END OpenVPN Static key V1-----\n";
echo $key;
