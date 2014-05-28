<?php
require_once '../application/libs/Error.php';

class FrontController{

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
        $old_error_handler = set_error_handler(array('FrontController', 'myErrorHandler'));
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
        
        if(! empty($_GET['lang']))
        	Session::add('lang', $_GET['lang']);
        
        $route = Route::getInstance();
        $route->load($config->get('configFolder') . 'routes.ini');
        $route->run();
    }
}