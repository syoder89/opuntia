<?php
require_once '../application/libs/Error.php';

class FrontControllerSimple{

    public static function myErrorHandler($errno, $errstr, $errfile, $errline){
    	global $config;
        switch ($errno) {
            case E_USER_ERROR:
            case E_ERROR:
                $err = "<b>My ERROR</b> [$errno] $errstr<br />\n";
                $err .= "  Fatal error on line $errline in file $errfile";
                new Error($err);
                header("location: error");
                exit();
        }

        return true;
    }

    public static function main(){
    	global $config;
        $old_error_handler = set_error_handler(array('FrontControllerSimple', 'myErrorHandler'));
        require_once '../application/libs/Config.php';
        require_once '../application/libs/Db.php';
        require_once '../application/libs/Translator.php';
        require_once '../application/libs/ControllerBase.php';
        require_once '../application/libs/ModelBase.php';
        require_once '../application/libs/View.php';
		require_once '../application/config/config.php';
        require_once '../application/libs/Session.php';
        require_once '../application/libs/Log.php';
        require_once '../application/libs/Route.php';
        require_once '../application/libs/FlashMessenger.php';
        require_once '../application/libs/Utils.php';

        $sessionName = $config->get('sessionName') ? $config->get('sessionName') : 'wifi_ap';
        Session::setName($sessionName);
        Session::start();

        $config->set('flashMessenger', new FlashMessenger());        
        $config->set('log', new Log());
        
        if(! empty($_GET['lang'])){
        	Session::add('lang', $_GET['lang']);
        }
        
        if(! empty($_GET['controller'])){
            $controllerName = $_GET['controller'];
            $controllerName[0] = strtoupper($controllerName[0]);
            $controllerName = $controllerName . 'Controller';
        }else{
            $controllerName = "IndexController";
        }
        
        if(! empty($_GET['action'])){
        	$actionName = $_GET['action'];
        }else{
        	$actionName = "index";
        }

        $controllerPath = $config->get('controllersFolder') . $controllerName . '.php';
        
        if(is_file($controllerPath)){
            require_once $controllerPath;
        }else{
            die("El controlador {$controllerName} no existe - 404 not found");
        }
        
        if(is_callable(array($controllerName, $actionName)) == false){
            trigger_error($controllerName . '->' . $actionName . '` no existe', E_USER_NOTICE);
            return false;
        }
        
        $controller = new $controllerName();
        
        if(method_exists($controller, 'init')){
            $controller->init();            
        }
        
        $controller->$actionName();
    }
}
