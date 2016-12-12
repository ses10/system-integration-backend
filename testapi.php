<?php

require_once('path.inc');
require_once('get_host_info.inc');
require_once('rabbitMQLib.inc');


        $client = new rabbitMQClient("apiRabbitMQ.ini","testServer");

        $req = array();
        $req['type'] = "apiRequest";
        $req['param'] = array('year' => '2001', 'make' => 'ford', 'model' => 'focus'); //'model' => 'Focus');
	//$req['param'] = array();	

        $res = $client->send_request($req);

        echo( $res);



/*
$cl = new rabbitMQClient("testRabbitMQ.ini","testServer");
$request = array();
$request['type'] = "login";
$request['username'] = "bob100";
$request['password'] = "123";

$response = $cl->send_request($request);

print_r( $response);



*/

?>
