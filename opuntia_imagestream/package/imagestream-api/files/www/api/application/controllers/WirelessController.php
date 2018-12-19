<?php

require $config->get('modelsFolder') . 'WirelessModel.php';

class WirelessController extends ControllerBase{
	private $model;

	public function init(){
		$this->model = new WirelessModel();
	}
	
    //Accion index
    public function getInfo(){
    	$interface = $_GET['interface'];
        $data['output'] = $this->model->getInfo($interface);
        $this->_view->show('common/json.php', $data);
    }
    
    public function getScan(){
    	$interface = $_GET['interface'];
    	$data['output'] = $this->model->getScan($interface);
    	$this->_view->show('common/json.php', $data);
    }
    
    public function getClients(){
    	$interface = $_GET['interface'];
    	$data['output'] = $this->model->getClients($interface);
    	$this->_view->show('common/json.php', $data);
    }
    
    public function getStats(){
    	$interface = $_GET['interface'];
    	$data['output'] = $this->model->getStats($interface);
    	$this->_view->show('common/json.php', $data);
    }

    public function getTxPower(){
    	$interface = $_GET['interface'];
    	$data['output'] = $this->model->getTxPower($interface);
    	$this->_view->show('common/json.php', $data);
    }
    
    public function getTxPowerList(){
    	$interface = $_GET['interface'];
    	$data['output'] = $this->model->getTxPowerList($interface);
    	$this->_view->show('common/json.php', $data);
    }

    public function setTxPower(){
    	$interface = $_GET['interface'];
    	$txPower = $_GET['txPower'];
    	$data['output'] = $this->model->setTxPower($interface, $txPower);
    	$this->_view->show('common/json.php', $data);
    }
}