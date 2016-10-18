#!/usr/bin/php
<?php
require_once('path.inc');
require_once('get_host_info.inc');
require_once('rabbitMQLib.inc');
require_once('databaseHelper.inc');

function doLogin($username,$password)
{
    $dbHelper = new DatabaseHelper();    

    if(!$dbHelper->connect())
    {	
	return array("returnCode" => '1', 'message'=>"Error connecting to server");
    }

//print_r($dbHelper);

    $info = $dbHelper->getUserInfo($username, $password);
    
    if($info)
    {
	//return array('returnCode' => '0', 'message' => 'Server received request and processed', 
	//	'firstname' => $info['first_name'], 'lastname' => $info['last_name']);
	
	return (array('returnCode' => '0', 'message' => 'Server received request and processed') + $info);
    }else
    {
	return array("returnCode" => '1', 'message'=>"Login unsuccessful");
    }

}

function doRegister($request)
{
    $dbHelper = new DatabaseHelper();
    
    if($dbHelper->registerUser($request['username'], $request['password'], $request['firstName'], $request['lastName']))
    {
	return array("returnCode" => '1', 'message'=>"Registration successful");
    }

    return array("returnCode" => '0', 'message'=>"Registration unsuccessful");
}

function logMessage($request)
{
	$logFile = fopen("log.txt", "a");

	fwrite($logFile, $request['message'] .'\n\n');

	return true;
}

function requestProcessor($request)
{
  echo "received request".PHP_EOL;
  var_dump($request);
  echo 'after dump';
  if(!isset($request['type']))
  {
    return "ERROR: unsupported message type";
  }
  switch ($request['type'])
  {
    case "register":    
      return doRegister($request);
    case "login":
      return doLogin($request['username'],$request['password']);
    case "validate_session":
      return doValidate($request['sessionId']);
    case "log":
      return logMessage($request);
  }
  return array("returnCode" => '0', 'message'=>"Server received request and processed");
}

$server = new rabbitMQServer("testRabbitMQ.ini","testServer");

$server->process_requests('requestProcessor');
exit();
?>

