<?php

require $config->get('modelsFolder') . 'WirelessModel.php';

class WirelessController extends ControllerBase{
	private $model;

	public function init(){
		$this->model = new WirelessModel();
	}
	
    //Accion index
    public function info(){
        $data['output'] = $this->model->getInfo();
        $this->_view->show('common/json.php', $data);
    }
    
    public function scan(){
    	$radio = $_GET['radio'];
    	$data['output'] = $this->model->getScan($radio);
    	$this->_view->show('common/json.php', $data);
    }
}