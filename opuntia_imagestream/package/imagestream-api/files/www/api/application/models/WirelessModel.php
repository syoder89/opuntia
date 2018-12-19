<?php
class WirelessModel extends ModelBase{
	
	const BIN_PATH = '/usr/bin/';
	const SBIN_PATH = '/sbin/';
	
	private function _cleanKey($key){
		$notAllowed = array(' ', '(', ')', ':');
		$replace = array('_', '', '', '');
		$key = trim($key);
		$key = str_replace($notAllowed, $replace, strtolower($key));
		return $key;
	}
	
    public function getInfo($interface){
    	$result = array();
    	$currentInterface = '';

        if($interface){
    		$output = shell_exec(self::BIN_PATH . "iwinfo {$interface} info");
    	}else{
    		$output = shell_exec(self::BIN_PATH . 'iwinfo');
    	}
    	
    	$lines = explode("\n", $output);
    	
    	foreach($lines as $line){
    		if(preg_match('/^(\S+)\b(\s+)(\S+)\b:(\s+)(\S+)/', $line, $matches)){
    			$currentInterface = $matches[1];
    			$options = array('name' => $currentInterface, $this->_cleanKey($matches[3]) => trim($matches[5], '"'));
    		}elseif(preg_match_all('/^(\s+)\b(.*?:)(.*?(\s\s))(.*?:)(.*)/', $line, $matches)){
    			$options[$this->_cleanKey($matches[2][0])] = trim($matches[3][0]);
    			$options[$this->_cleanKey($matches[5][0])] = trim($matches[6][0]);
    		}elseif(preg_match_all('/^(\s+)\b(.*?:)(.*)/', $line, $matches)){
    			$options[$this->_cleanKey($matches[2][0])] = trim($matches[3][0]);
    		}
    		if($currentInterface){
    			$result[$currentInterface] = $options;
    		}
    	}
    	return $result; 
    }
    
    public function getScan($interface){
    	$result = array();
    	$output = array();
    	if($interface){
    		$output[$this->_cleanKey($interface)] = shell_exec(self::BIN_PATH . "iwinfo {$interface} scan");
    	}else{
    		$interfaces = $this->getInfo();
    		if(count($interfaces) > 0){
    			foreach($interfaces as $info){
    				$output[$this->_cleanKey($info['name'])] = shell_exec(self::BIN_PATH . "iwinfo {$info['name']} scan");
    			}
    		}
    	}
    	
    	foreach($output as $interface => $scan){
    		$result[$interface] = array();
    		$lines = explode("\n", $scan);
    		$currentCell = '';
    		foreach($lines as $line){
    			if(preg_match('/^(\S+\s\S+)\s-\s(\S+)\b:(\s+)(\S+)/', $line, $matches)){
    				$currentCell = $this->_cleanKey($matches[1]);
    				$options = array('name' => $currentCell, $this->_cleanKey($matches[2]) => trim($matches[4]));
    			}elseif(preg_match_all('/^(\s+)\b(.*?:)(.*?(\s\s))(.*?:)(.*)/', $line, $matches)){
    				$options[$this->_cleanKey($matches[2][0])] = trim($matches[3][0]);
    				$options[$this->_cleanKey($matches[5][0])] = trim($matches[6][0]);
    			}elseif(preg_match_all('/^(\s+)\b(.*?:)(.*)/', $line, $matches)){
    				$options[$this->_cleanKey($matches[2][0])] = trim(trim($matches[3][0]), '"');
    			}
    			if($currentCell){
    				$result[$interface][$currentCell] = $options;
    			}
    		}    		
    	}
    	return $result;
    }
    
    public function getClients($interface){
    	$result = array();
    	$output = array();
    	
    	if($interface){
    		$output[$this->_cleanKey($interface)] = shell_exec(self::BIN_PATH . "iw {$interface} station dump");
    	}else{
    		$interfaces = $this->getInfo();
    		if(count($interfaces) > 0){
    			foreach($interfaces as $info){
    				$output[$this->_cleanKey($info['name'])] = shell_exec(self::BIN_PATH . "iw {$info['name']} station dump");
    			}
    		}
    	}
    	 
    	foreach($output as $interface => $dump){
    		$result[$interface] = array();
    		$lines = explode("\n", $dump);
    		$currentStation = '';
    		foreach($lines as $line){
    			if(preg_match('/^Station\s(\S+)\s\(on\s(\S+)\)/', $line, $matches)){
    				$currentStation = 's' . $this->_cleanKey($matches[1]);
    				$options = array('macaddr' => $matches[1], 'interface' => $matches[2]);
    			}elseif(preg_match_all('/^(\s+)\b(.*):\s+(.*)$/', $line, $matches)){
    				$options[$this->_cleanKey($matches[2][0])] = trim($matches[3][0]);
    			}
    			if($currentStation){
    				$result[$interface][$currentStation] = $options;
    			}
    		}    		
    	}
    	return $result;
    }
    
    public function getStats($interface){
    	$result = array();
    	$output = array();

    	if($interface){
    		$output[$this->_cleanKey($interface)] = shell_exec("devstatus {$interface}");
    	}else{
    		$interfaces = $this->getInfo();
    		if(count($interfaces) > 0){
    			foreach($interfaces as $info){
    				$output[$this->_cleanKey($info['name'])] = shell_exec("devstatus {$info['name']}");
    			}
    		}
    	}

    	foreach($output as $interface => $stats){
    		$result[$interface] = (array)json_decode($stats);
    	}
    	return $result;
    }

    public function getTxPower($interface){
    	$result = array();
    	$output = array();

    	if($interface){
    		$output[$this->_cleanKey($interface)] = shell_exec(self::BIN_PATH . "iwinfo {$interface} txpowerlist");
    	}else{
    		$interfaces = $this->getInfo();
    		if(count($interfaces) > 0){
    			foreach($interfaces as $info){
    				$output[$this->_cleanKey($info['name'])] = shell_exec(self::BIN_PATH . "iwinfo {$info['name']} txpowerlist");
    			}
    		}
    	}

    	foreach($output as $interface => $dump){
    		$result[$interface] = array();
    		$lines = explode("\n", $dump);
    		$currentTxPower = '';
    		foreach($lines as $line){
    			if(preg_match('/^\*(.+)/', $line, $matches)){
    				$result[$interface] = trim($matches[1]);
    			}
    		}    		
    	}
    	return $result;
    }

    public function getTxPowerList($interface){
    	$result = array();

    	if(!$interface){
    		return 'Error: <interface> is missing';
    	}
    	
		$output = shell_exec(self::BIN_PATH . "iwinfo {$interface} txpowerlist");
		$lines = explode("\n", $output);
		foreach($lines as $line){
			if($line){
				$result[] = trim($line);
			}
    	}
    	return $result;
    }

    public function setTxPower($interface, $txPower){
    	$result = array();
    	$validTxPower = false;

        if(!$interface){
    		return array('success' => false, 'error' => '<interface> is missing');
    	}

		if($txPower == ''){
			return array('success' => false, 'error' => '<txPower> is missing');
    	}
    	
    	$txPowerList = $this->getTxPowerList($interface);
    	if(count($txPowerList) > 0){
    		foreach($txPowerList as $txPowerValue){
    		    if(preg_match("/(.*){$txPower} dBm/", $txPowerValue)){
    				$validTxPower = true;
    				break;
    			}
    			
    		}
    	}
    	
    	if(!$validTxPower){
    		return array('success' => false, 'error' => '<txPower> is not valid');
    	}
    	
    	$infoInterface = $this->getInfo($interface);
    	$radio = str_replace('phy', 'radio', $infoInterface[$interface]['phy_name']);

    	shell_exec("uci set wireless.{$radio}.txpower={$txPower}");
    	shell_exec("uci commit");
    	shell_exec(self::SBIN_PATH . "wifi reload {$radio}");
    	sleep(2);
    	$newTxPower = $this->getTxPower($interface);
		return array('success' => true, 'txPower' => $newTxPower[$interface]);
    }
    
}