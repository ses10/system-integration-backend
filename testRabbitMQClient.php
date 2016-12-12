#!/usr/bin/php
<?php
require_once('path.inc');
require_once('get_host_info.inc');
require_once('rabbitMQLib.inc');

$client = new rabbitMQClient("apiRabbitMQ.ini","testServer");
if (isset($argv[1]))
{
  $msg = $argv[1];
}
else
{
  $msg = "test message";
}

$request = array();
$request['type'] = "apiRequest";
//$request['param'] = array();
//$request['param'] = array("year" => "2001");
//$request['param'] = array("year" => "2001", "make" => "honda");
$request['param'] = array("year"=>"2001", "make" => "honda", "model" => "civic");
$request['username'] = "bob100";
//$request['password'] = "123";
//$request['message'] = $msg;
$response = $client->send_request($request);
//$response = $client->publish($request);

//echo "client received response: ".PHP_EOL;
print_r($response);
//echo "\n\n";

//echo $argv[0]." END".PHP_EOL;

