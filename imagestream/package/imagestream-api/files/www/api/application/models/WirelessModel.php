<?php
class WirelessModel extends ModelBase{

	private function _cleanKey($key){
		$notAllowed = array(' ', '(', ')', ':');
		$replace = array('_', '', '', '');
		$key = trim($key);
		$key = str_replace($notAllowed, $replace, strtolower($key));
		return $key;
	}
	
    public function getInfo(){
    	$result = array();
    	$currentInterface = '';
    	
    	$output = shell_exec("/tmp/iwinfo");
    	$lines = explode("\n", $output);
    	
    	foreach($lines as $line){
    		//echo $line, '<br>';
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
    
    public function getScan($radio){
    	$result = array();
    	$output = array();
    	if($radio){
    		$output[$this->_cleanKey($radio)] = shell_exec("/tmp/iwinfo {$radio} scan");
    	}else{
    		$information = $this->getInfo();
    		if(count($information) > 0){
    			foreach($information as $info){
    				$output[$this->_cleanKey($info['name'])] = shell_exec("/tmp/iwinfo {$info['name']} scan");
    			}
    		}
    	}
    	
    	foreach($output as $radio => $_output){
    		$result[$radio] = array();
    		$lines = explode("\n", $_output);
    		$currentCell = '';
    		foreach($lines as $line){
    			//echo $line, '<br>';
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
    				$result[$radio][$currentCell] = $options;
    			}
    		}    		
    	}
    	return $result;
    }
}