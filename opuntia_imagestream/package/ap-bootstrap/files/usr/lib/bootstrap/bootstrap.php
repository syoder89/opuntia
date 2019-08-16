#!/usr/bin/php-cli

<?php

if (file_exists('/etc/wifi_bootstrap_host'))
	$wifi_bootstrap_host = trim(file_get_contents('/etc/wifi_bootstrap_host'));
else
	$wifi_bootstrap_host = 'wifi-bootstrap.imagestream.com';

$model = trim(shell_exec('. /etc/opuntia_release;  echo $DISTRIB_PRODUCT'));
if (substr($model, 0, 3) == 'AP3') {
	$yellow_led = "/sys/class/leds/wpj344\:green\:sig3";
	$red_led = "/sys/class/leds/wpj344\:green\:sig3";
	$green_led = "/sys/class/leds/wpj344\:green\:sig4";
}
else {
	$yellow_led = "/sys/class/leds/wpj344\:yellow\:sig2";
	$red_led = "/sys/class/leds/wpj344\:red\:sig1";
	$green_led = "/sys/class/leds/wpj344\:green\:sig3";
}

function led_on($led) {
//	shell_exec("echo timer > $led/trigger");
//	shell_exec("echo 255 > $led/brightness");
}

function led_off($led) {
//	shell_exec("echo none > $led/trigger");
//	shell_exec("echo 0 > $led/brightness");
}

led_off($red_led);
led_off($green_led);
led_on($yellow_led);

define("SCRIPT_FILE_GPG", "/tmp/bootstrap.sh.gpg");
define("SCRIPT_FILE", "/tmp/bootstrap.sh");
define("STATUS_FILE", "/tmp/bootstrap.status");
$HOMEDIR=dirname(__FILE__);

$status = 0;

function shutdown() {
	global $red_led, $yellow_led, $green_led;
	global $status;
	led_off($yellow_led);
	led_off($green_led);
	if ($status == 0)
		led_on($red_led);
	else
		led_off($red_led);
}

register_shutdown_function('shutdown');

function log_err($err) {
	syslog(LOG_ERR, "bootstrap: $err");
	exit(1);
}

function log_out($out) {
	syslog(LOG_INFO, "bootstrap: $out");
}

function log_status($out, $color='green') {
	file_put_contents(STATUS_FILE, "<div align=center><font color=$color>$out</font></div>");
}

$url = 'http://169.254.169.254/openstack/latest/meta_data.json';
$try = 1;
while ($try <= 2) {
	if ($try == 1)
		log_status("Checking for OpenStack metadata service...");
	else
		log_status("Checking for OpenStack metadata service (attempt $try / 10)", 'yellow');
	log_out("Attempt $try Fetching URL: $url");
	$ch = curl_init("$url");
	curl_setopt($ch,CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
	curl_setopt($ch, CURLOPT_TIMEOUT, 5);
	$response = curl_exec($ch);
	curl_close($ch);
	log_out("Got response: $response");
	$decoded = json_decode($response);
	if (isset($decoded->meta)) {
		if (isset($decoded->meta->wifi_bootstrap_host)) {
			$wifi_bootstrap_host = trim($decoded->meta->wifi_bootstrap_host);
			log_out("Using host: $wifi_bootstrap_host");
			log_status("Using host: $wifi_bootstrap_host");
		}
		if (isset($decoded->meta->wifi_bootstrap_ip)) {
			$wifi_bootstrap_ip = trim($decoded->meta->wifi_bootstrap_ip);
			log_out("Using ip: $wifi_bootstrap_ip");
			log_status("Using ip: $wifi_bootstrap_ip");
			system("grep $wifi_bootstrap_host /etc/hosts", $ret);
			if ($ret == 0)
				shell_exec("sed -i \"/$wifi_bootstrap_host/ s/.*/$wifi_bootstrap_ip\t$wifi_bootstrap_host/g\" /etc/hosts");
			else
				shell_exec("echo $wifi_bootstrap_ip $wifi_bootstrap_host >> /etc/hosts");
		}
		if (isset($decoded->meta->hostname)) {
			shell_exec("uci set system.@system[0].hostname='".$decoded->meta->hostname."'");
			shell_exec("uci commit system");
		}
		break;
	}
	else log_out("Not found...");
	$try++;
}

$mac = trim(strtoupper(file_get_contents('/sys/class/net/eth0/address')));
define("REGISTER_SCRIPT_FILE", "/tmp/register.sh");
$url = "https://$wifi_bootstrap_host/getRegistrationScript/$mac";
$try = 1;
while ($try <= 10) {
	if ($try == 1)
		log_status("Connecting to the Cloud...");
	else
		log_status("Connecting to the Cloud (attempt $try / 10)", 'yellow');
	log_out("Attempt $try Fetching URL: $url");
	$ch = curl_init("$url");
	curl_setopt($ch,CURLOPT_RETURNTRANSFER, true);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
	curl_setopt($ch, CURLOPT_TIMEOUT, 30);
	$response = curl_exec($ch);
	curl_close($ch);
	log_out("Got response: $response");
	$decoded = json_decode($response);
	if (isset($decoded->response->status) && $decoded->response->status == 'ERROR')
    		log_err($decoded->response->errormessage);
	if (isset($decoded->result) && $decoded->result == 'false')
		log_err("Registration returned error: ".$decoded->error);
	if (isset($decoded->script)) {
		file_put_contents(REGISTER_SCRIPT_FILE, base64_decode($decoded->script));

		log_status("Getting scripts...");
		log_out("Running script");
		led_off($yellow_led);
		led_on($green_led);
		led_on($red_led);
		chmod(REGISTER_SCRIPT_FILE, 0700);
		system(REGISTER_SCRIPT_FILE." > /tmp/register_script.log 2>&1", $result);
		if ($result != 0) {
			log_out(file_get_contents("/tmp/register_script.log"));
			$log = file("/tmp/register_script.log");
			if (count($log))
				$line = $log[count($log)-1];
			else
				$line = '';
			log_err("Registration Script failed with error $result!<br>".$line);
		}
		else {
			log_out("Registration script successful!");
			log_status("Success");
			$status = 1;
			exit(0);
		}
	}
	else log_out("Error fetching URL: $response");
	sleep(10);
	$try++;
}
log_err("Unable to connect to the Cloud!");
exit(1);
?>
