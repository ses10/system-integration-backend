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

    if( $dbHelper->checkCredentials($username, $password) )
    {
	$info = $dbHelper->getUserInfo($username, $password);	
	return (array('returnCode' => '0', 'message' => 'Server received request and processed') + $info);
    }
    
//$info = $dbHelper->getUserInfo($username, $password);
    
    /*if($info)
    {
	
	return (array('returnCode' => '0', 'message' => 'Server received request and processed') + $info);
    }*/
    else
    {
	return array("returnCode" => '1', 'message'=>"Login unsuccessful");
    }

}

function doRegister($request)
{
    $dbHelper = new DatabaseHelper();
   
    if(!$dbHelper->connect())
    {
        return array("returnCode" => '1', 'message'=>"Error connecting to server");
    }
 
    if($dbHelper->registerUser($request['username'], $request['password'], $request['firstName'], $request['lastName']))
    {
	return array("returnCode" => '1', 'message'=>"Registration successful");
    }

    return array("returnCode" => '0', 'message'=>"Registration unsuccessful");
}

function logMessage($request)
{
	echo 'a log has been recieved';
	echo $request['message'];

	$logFile = fopen("log.txt", "a");

	fwrite($logFile, $request['message'] ."\n");

	return true;
}

function handleApiRequest($request)
{
	
	$client = new rabbitMQClient("apiRabbitMQ.ini","testServer");
	
	$req = array();
	$req['type'] = "apiRequest";
	//$req['param'] = array();

	if( count($request['param']) == 0)
	{
		$req['param'] = array();
	}
	else if( count($request['param']) == 1  )
	{
		$req['param'] = array('year' => $request['param']['year']);
	}
	else if( count($request['param']) == 2 )
	{
		$req['param'] = array('year' => $request['param']['year'], 'make' => $request['param']['make']);
	}
	//user is adding car to their list
	else if( count($request['param']) == 3 )
	{
		
		$req['param'] = array('year' => $request['param']['year'], 'make' => $request['param']['make'], 'model' => $request['param']['model']);
		
		$dbHelper = new DatabaseHelper();
	
		if(!$dbHelper->connect())
    		{
        		return array("returnCode" => '1', 'message'=>"Error connecting to server");
    		}
		
		//add the new usercar and get id
		$id = $dbHelper->addUserCar($request['username'], $request['param']['year'], $request['param']['make'], $request['param']['model']);

		//check db for any recalls of recent added car
		$recalls = $dbHelper->getCarRecalls($request['param']['year'], $request['param']['make'], $request['param']['model']);
		//hit the api for recalls if no results
		if( empty($recalls) )
		{
			//no recall in db so go to api
			$response = $client->send_request($req);

			$response = json_decode($response, true);
			$recalls = $response['Results'];	
	
			//insert recalls into db
			$dbHelper->insertRecalls($recalls);
			
		}

		//add the new car and its recalls to UserCarRecalls
		$dbHelper->addUserCarRecall($id);		
		
		return json_encode(array("returnCode" => '0', 'message'=>"Server received request and processed"));
	}
	
	$response = $client->send_request($req);

	
	return $response;
}

function getRecalls($request)
{

    $dbHelper = new DatabaseHelper();

    if(!$dbHelper->connect())
    {
        return array("returnCode" => '1', 'message'=>"Error connecting to server");
    }
	
    //$res = $dbHelper->getCarRecalls($request['param']['year'], $request['param']['make'], $request['param']['model']);
    $res = $dbHelper->getUserCarRecalls($request['id']);
	//print_r($res);
    return json_encode($res);	
}

function toggleRecallCheck($request)
{
    $dbHelper = new DatabaseHelper();
    
    if(!$dbHelper->connect())
    {   
        return array("returnCode" => '1', 'message'=>"Error connecting to server");
    } 

    return $dbHelper->toggleRecallCheck($request['uuid']);
}

function deleteUserCar($request)
{
    $dbHelper = new DatabaseHelper();

    if(!$dbHelper->connect())
    {
        return array("returnCode" => '1', 'message'=>"Error connecting to server");
    }
    
    $dbHelper->removeUserCar($request['id']);
}

function requestProcessor($request)
{
  echo "received request".PHP_EOL;
  //var_dump($request);
//  echo 'after dump';
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
    case "apiRequest":
      return handleApiRequest($request);
    case "getRecalls": 
      return getRecalls($request);
    case "toggleRecall":
      return toggleRecallCheck($request);
    case "deleteUserCar":
      deleteUserCar($request);
  }
  return array("returnCode" => '0', 'message'=>"Server received request and processed");
}

$server = new rabbitMQServer("testRabbitMQ.ini","testServer");

$server->process_requests('requestProcessor');
exit();
?>

