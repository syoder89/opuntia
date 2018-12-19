<?php

$output = shell_exec("/usr/bin/iwinfo");
function cleanKey($key){
  $notAllowed = array(' ', '(', ')', ':');
    $replace = array('_', '', '', '');
      $key = trim($key);
        $key = str_replace($notAllowed, $replace, strtolower($key));
	  return $key;
	  }

	  $result = array();
	  $lines = explode("\n", $output);
	  $currentInterface = '';
	  foreach($lines as $line){
	    //echo $line, '<br>';
	      if(preg_match('/^(\S+)\b(\s+)(\S+)\b:(\s+)(\S+)/', $line, $matches)){
	          $currentInterface = $matches[1];
		      $options = array('name' => $currentInterface, cleanKey($matches[3]) => trim($matches[5], '"'));
		        }elseif(preg_match_all('/^(\s+)\b(.*?:)(.*?(\s\s))(.*?:)(.*)/', $line, $matches)){
			    $options[cleanKey($matches[2][0])] = trim($matches[3][0]);
			        $options[cleanKey($matches[5][0])] = trim($matches[6][0]);
				  }elseif(preg_match_all('/^(\s+)\b(.*?:)(.*)/', $line, $matches)){
				      $options[cleanKey($matches[2][0])] = trim($matches[3][0]);
				        }
					  if($currentInterface){
					      $result[$currentInterface] = $options;
					        }
						}
						//echo '<pre>';
						//print_r($result);
						//echo '</pre>';
						echo json_encode($result);


