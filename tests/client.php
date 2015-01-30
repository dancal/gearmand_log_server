#!/usr/bin/php
<?php

error_reporting(E_ALL);
ini_set("display_errors", 1);

class DeliveryLogSendToGearman {

    protected $rServerIP;   
    protected $sPort;
    protected $gmClient;
    protected $isConnected;
    
    public function __construct( $rServerIP, $port, $timeout ) {
    
        $this->isConnected  = false;
        $this->rServerIP    = $rServerIP;
        $this->sPort        = $port;
    
        $this->gmClient     = new GearmanClient();
        if ( $this->gmClient ) {
        	$this->gmClient->addOptions(GEARMAN_CLIENT_NON_BLOCKING | GEARMAN_CLIENT_FREE_TASKS | GEARMAN_CLIENT_UNBUFFERED_RESULT);
            $this->gmClient->setTimeout($timeout);
            foreach ( $this->rServerIP as $idx => $sServer ) {
                $this->gmClient->addServer( $sServer, $port );
            }   
        }
    
		return $this;
    }

    public function LogSend( $job_name, $sLogJson ) {
        $nRet               = $this->gmClient->doHighBackground($job_name, $sLogJson);
        if ($this->gmClient->returnCode() != GEARMAN_SUCCESS) {
            return 0;
        }
        return 1; 
    }
    
}

$rServersIP			= array();
$rServersIP[]		= '127.0.0.1';
$server_port		= 4730;
$timeout			= 1000;
$jobs_name			= 'rtb_log_write';
$sLogJson			= 'test';

$logServer          = new DeliveryLogSendToGearman( $rServersIP, $server_port, $timeout );
$logServer->LogSend( $jobs_name, $sLogJson );

var_dump($logServer);
?>
