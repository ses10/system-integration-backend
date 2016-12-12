#!/usr/bin/php
<?php
require_once('path.inc');
require_once('get_host_info.inc');
require_once('rabbitMQLib.inc');
function carData($request)
{
  $reqCount = count($request['param']);
  if ($reqCount == 0)
  {
     echo("Year request");
     return(file_get_contents("http://www.nhtsa.gov/webapi/api/Recalls/vehicle?format=json"));
  }
  else if ($reqCount == 1)
  {
     echo("Make request, given year");
     return(file_get_contents("http://www.nhtsa.gov/webapi/api/Recalls/vehicle/modelyear/".($request['param']['year'])."?format=json"));
  }
  else if ($reqCount == 2)
  {
     echo("Model request, given year and make");
     return(file_get_contents("http://www.nhtsa.gov/webapi/api/Recalls/vehicle/modelyear/".($request['param']['year'])."/make/".($request['param']['make'])."?format=json"));
  }
  else if ($reqCount == 3)
  {
     echo("Recall information, given year, make, model");
     return(file_get_contents("http://www.nhtsa.gov/webapi/api/Recalls/vehicle/modelyear/".($request['param']['year'])."/make/".($request['param']['make'])."/model/".($request['param']['model'])."?format=json"));
  }
//http://www.nhtsa.gov/webapi/api/Recalls/vehicle/modelyear/2000/make/saturn/model/LS?format=json
  return ($reqCount);
}
function requestProcessor($request)
{
  echo "\r\n\r\nreceived request".PHP_EOL;
// var_dump($request);
  if(!isset($request['type']))
  {
    return "ERROR: unsupported message type";
  }
  switch ($request['type'])
  {
    case "apiRequest":
      return carData($request);
  }
  return array("returnCode" => '0', 'message'=>"Server received request and processed");
}
$server = new rabbitMQServer("testRabbitMQ.ini","testServer");
$server->process_requests('requestProcessor');
exit();
?>
