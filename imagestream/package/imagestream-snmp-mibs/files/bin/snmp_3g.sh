#!/usr/bin/php-cli
<?php
$stdout = fopen ("php://stdout","w");
$CACHE_TIME=25;
$ISIS_OID=".1.3.6.1.4.1.15425";
$BASE_OID=".1.3.6.1.4.1.15425.1.1.3.1";
$_3G_TABLE_OID="1";
$last_mib_update = 0;

class MIB_ITEM {
	public $type;
	public $value;
	public $writable;
	public $write_validate;
	public $write_handler;
	public $write_handler_args;

	function __construct($type, $value, $writable = false, $write_validate = false,
			$write_handler = false, $write_handler_args = false) {
		$this->type = $type;
		$this->value = $value;
		$this->writable = $writable;
		$this->write_validate = $write_validate;
		$this->write_handler = $write_handler;
		$this->write_handler_args = $write_handler_args;
	}
	function validate($value) {
		if ($this->write_validate !== false)
			return preg_match($this->write_validate, $value);
		return false;
	}
	function write($value) {
		if (($this->write_handler !== false) && is_callable($this->write_handler))
			return call_user_func($this->write_handler, $value, $this->write_handler_args);
	}
}

class SMS_MESSAGE {
	public $index;
	public $status;
	public $smsc;
	public $sender;
	public $date;
	public $message;

	function __construct($index, $status, $smsc, $sender, $date, $message) {
		$this->index = $index;
		$this->status = $status;
		$this->smsc = $smsc;
		$this->sender= $sender;
		$this->date = $date;
		$this->message = $message;
	}
}

function set_mib($ifnum, $offset, $type, $value, $writable = false,
		$write_validate = false, $write_handler = false, $write_handler_args = false)
{
	global $mib;
	global $ISIS_OID, $BASE_OID, $_3G_TABLE_OID;

	$ifnum = trim($ifnum);
	$value = trim($value);
	$OID = "$BASE_OID.$_3G_TABLE_OID.$offset.$ifnum";

	$mib["$OID"] = new MIB_ITEM($type, $value, $writable, $write_validate, $write_handler, $write_handler_args);
}

function set_sub_mib($ifnum, $offset, $sub_offset, $type, $value, $writable = false,
		$write_validate = false, $write_handler = false, $write_handler_args = false)
{
	global $mib;
	global $ISIS_OID, $BASE_OID, $_3G_TABLE_OID;

	$ifnum = trim($ifnum);
	$value = trim($value);
	$OID = "$BASE_OID.$_3G_TABLE_OID.$offset.$sub_offset.$ifnum";

	$mib["$OID"] = new MIB_ITEM($type, $value, $writable, $write_validate, $write_handler, $write_handler_args);
}

function get_sms_messages()
{
	$sms_messages = array();

	$tmp = explode("\n", shell_exec('man3g display-sms --machine'));

	if (!empty($tmp)) {
		while (($id = current($tmp)) !== false) {
			// Apparently the false check doesn't cut it...
			if (empty($id))
				break;
			$status = next($tmp);
			$smsc = next($tmp);
			$sender = next($tmp);
			$date = next($tmp);
			$length = next($tmp);
			$message = next($tmp);
			while(strlen($message) < $length) {
				if (($str = next($tmp)) === false)
					break;
				$message .= "\n". $str;
			}
			$sms_messages["$id"] = new SMS_MESSAGE($id, $status, $smsc, $sender, $date, $message);
			if ((next($tmp)) === false)
				break;
		}
	}
	return $sms_messages;
}

function create_mib()
{
	global $last_mib_update, $CACHE_TIME;
	global $mib;

	$timestamp=time();

	if (($last_mib_update+$CACHE_TIME) > $timestamp)
		return;

	$mib = array();

	$last_mib_update=$timestamp;
	$PID = posix_getpid();
	$tmp = explode("\n", shell_exec('man3g status --machine'));
	if (!empty($tmp)) {
		$carrier = current($tmp);
		$ifnum=`ip link show dev wwan0 | head -n 1 | cut -d ':' -f 1`;
		$tech = next($tmp);
		$mcc = next($tmp);
		$mnc = next($tmp);
		$signal_strength = next($tmp);
		$rx_bit_rate = next($tmp);
		$tx_bit_rate = next($tmp);
		$apn = next($tmp);
		$signal_quality = next($tmp);
		$data_bearer = next($tmp);
		$band_class = next($tmp);
		$registration_state = next($tmp);
		$data_state = next($tmp);
		$data_duration = next($tmp);
		$roaming = next($tmp);
		$mobile_number = next($tmp);

		$offset = 1;
		set_mib($ifnum, $offset++, "integer", $ifnum);
		set_mib($ifnum, $offset++, "string", $carrier);
		set_mib($ifnum, $offset++, "string", $tech);
		set_mib($ifnum, $offset++, "integer", $mcc);
		set_mib($ifnum, $offset++, "integer", $mnc);
		set_mib($ifnum, $offset++, "integer", $signal_strength);
		set_mib($ifnum, $offset++, "integer", $rx_bit_rate);
		set_mib($ifnum, $offset++, "integer", $tx_bit_rate);
		set_mib($ifnum, $offset++, "string", $apn);
		set_mib($ifnum, $offset++, "integer", $signal_quality);
		set_mib($ifnum, $offset++, "integer", $data_bearer);
		set_mib($ifnum, $offset++, "integer", $band_class);
		set_mib($ifnum, $offset++, "integer", $registration_state);
		set_mib($ifnum, $offset++, "integer", $data_state);
		set_mib($ifnum, $offset++, "timeticks", $data_duration);
		set_mib($ifnum, $offset++, "integer", $roaming);
		set_mib($ifnum, $offset++, "string", $mobile_number);

		// Leave some room
		$offset = 40;
		$sms_messages = get_sms_messages();
		$num_messages = count($sms_messages);
		set_mib($ifnum, $offset++, "integer", $num_messages);

		foreach ($sms_messages as $id => $message) {
			$sub_offset = 1;
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "integer", $id);
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "integer", $message->status);
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "string", $message->smsc);
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "string", $message->sender);
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "string", $message->date);
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "string", $message->message);
			// Leave some room
			$sub_offset = 10;
			// The next 2 items are writable to modify messages
			// Delete
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "integer", 0, true, '/^1$/', 'do_delete_sms', $id);
			// Mark as read
			set_sub_mib($ifnum, $offset, '1.' . $sub_offset++ . ".$id" , "integer", 0, true, '/^[01]$/', 'do_mark_sms', $id);
		}
	}
}

function do_delete_sms($value, $id)
{
	$ret = exec("man3g delete-sms --msgid $id", $output = array(), $retval);
	if ($retval != 0) {
		syslog(LOG_ERR, "Unable to delete SMS message $id! Error: $retval");
		do_output("inconsistent-value\n");
	}
	else
		do_output("DONE\n");
}

function do_mark_sms($value, $id)
{
	switch($value) {
		case "0":
			$msgtype = "read";
		break;
		default:
			$msgtype = "unread";
		break;
	}
	$ret = exec("man3g mark-sms --msgid $id --msgtype $msgtype", $output = array(), $retval);
	if ($retval != 0) {
		syslog(LOG_ERR, "Unable to mark SMS message $id as $msgtype! Error: $retval");
		do_output("inconsistent-value\n");
	}
	else
		do_output("DONE\n");
}

function do_output($line)
{
	global $stdout;

	fprintf($stdout, $line);
	fflush($stdout);
}

function do_get($oid)
{
	global $mib;

	if (isset($mib["$oid"])) {
		$value = $mib["$oid"];
		$type = $value->type;
		$val = $value->value;
		do_output("$oid\n");
		do_output("$type\n");
		do_output("$val\n");
	}
	else
		do_output("NONE\n");
}

function do_set($oid, $val)
{
	global $mib;

	if (isset($mib["$oid"])) {
		$entry = $mib["$oid"];
		$type = $entry->type;
		$writable = $entry->writable;
		if (!$writable) {
			do_output("not-writable\n");
			return;
		}
		$tmp = preg_split("/\s+/", $val);
		if ($tmp[0] != $type) {
			do_output("wrong-type\n");
			return;
		}
		if (!$entry->validate($tmp[1])) {
			do_output("wrong-value\n");
			return;
		}
		$entry->write($tmp[1]);
	}
	else
		do_output("wrong-length\n"); // Shouldn't happen!
}

function do_get_next($oid)
{
	global $mib;
	global $BASE_OID;
	// Current normally should point to oid during a walk
	
	if ($oid == $BASE_OID) {
		uksort($mib, 'strnatcasecmp');
		reset($mib);
	}
	else {
		$key = key($mib);
		if ($key != $oid) {
			uksort($mib, 'strnatcasecmp');
			reset($mib);
			while (list($key, $value) = each($mib)) {
				if ($key == $oid)
					break;
			}
		}
		else // Each above advanced the pointer, so must we...
			next($mib);

		// Original OID not found?
		if ($key != $oid) {
			do_output("NONE\n");
			return;
		}
	}

	$key = key($mib);
	$value = current($mib);
	$type = $value->type;
	$val = $value->value;
	do_output("$key\n");
	do_output("$type\n");
	do_output("$val\n");
}

$handle = fopen ("php://stdin","r");
while(!feof($handle)) {
$cmd = trim(fgets($handle));
	if ($cmd == "")
		break;
	switch($cmd) {
		case 'PING':
			do_output("PONG\n");
		break;
		case 'get':
			$OID = trim(fgets($handle));
			create_mib();
			do_get($OID);
		break;
		case 'getnext':
			$OID = trim(fgets($handle));
			create_mib();
			do_get_next($OID);
		break;
		case 'set':
			$OID = trim(fgets($handle));
			$VAL = trim(fgets($handle));
			create_mib();
			do_set($OID, $VAL);
		break;
	}
}
?>
