<?php
require_once('path.inc');
require_once('get_host_info.inc');
require_once('rabbitMQLib.inc');


class DatabaseHelper
{
	private $conn;

	public function connect()
	{
		try
		{
			//$this->conn = new PDO('mysql:host=192.168.2.40;dbname=it490', 'ds646', 'abc123');
			$this->conn = new PDO('mysql:host=localhost;dbname=it490', 'root', 'qwerty');
			return true;
		}
		catch(PDOException $e)
		{
			$message =  'Error: ' . $e->getMessage();
			
			//log the error 
			$client = new rabbitMQClient("testRabbitMQ.ini","testServer");			
			$request = array();
			$request['type'] = "log";
			$request['message'] = $message;
			$response = $client->publish($request);
		}
		
		return false;
	}

	public function getUserInfo($username, $password)
	{
		$stmt = $this->conn->prepare('SELECT first_name, last_name FROM user WHERE username = :un AND password = :pw');
		$stmt->bindParam(':un', $username);
		$stmt->bindParam(':pw', $password);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();
		
		$result = $stmt->fetch();

		
		$stmt = $this->conn->prepare('SELECT year, make, model from userCar where username = :un');
		$stmt->bindParam(':un', $username);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();
	
		//$result += $stmt->fetchAll();
		$result['cars'] = $stmt->fetchAll();
		
		return $result;
	}

	
	public function getCarRecalls($year, $make, $model)
	{
		$stmt = $this->conn->prepare('SELECT * from recall WHERE year = :yr AND model = :mdl AND make = :mke');
		$stmt->bindParam(':yr', $year);
		$stmt->bindParam(':mdl', $model);
		$stmt->bindParam(':mke', $make);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();

		$result = $stmt->fetchAll();

		return $result;	
	}

	public function insertRecalls($recalls)
	{
		foreach($recalls as $recall)
		{


			$stmt = $this->conn->prepare('INSERT INTO recall VALUES(:yr, :mke, :mdl, :manu, :camNum, :rep, :compon, :sum, :con, :rem, :notes, CURDATE())');

			$stmt->bindParam(':yr', $recall['ModelYear']);
			$stmt->bindParam(':mke', $recall['Make']);
			$stmt->bindParam(':mdl', $recall['Model']);
			$stmt->bindParam(':manu', $recall['Manufacturer']);
			$stmt->bindParam(':camNum', $recall['NHTSACampaignNumber']);
			$stmt->bindParam(':rep', $recall['ReportReceivedDate']);
			$stmt->bindParam(':compon', $recall['Component']);
			$stmt->bindParam(':sum', $recall['Summary']);
			$stmt->bindParam(':con', $recall['Conequence']);
			$stmt->bindParam(':rem', $recall['Remedy']);
			$stmt->bindParam(':notes', $recall['Notes']);

			$stmt->execute();
		}
	}
	
	public function addUserCar($username, $year, $make, $model)
	{
		$stmt = $this->conn->prepare('INSERT INTO userCar VALUES(:un, :yr, :mke, :mdl)');
		$stmt->bindParam(':un', $username);
		$stmt->bindParam(':yr', $year);		
		$stmt->bindParam(':mke', $make);
		$stmt->bindParam(':mdl', $model);

		$stmt->execute();
	}
		
	//return true if successful
	public function registerUser($username, $password, $firstname, $lastname)
	{

		//insert query to user table
		$stmt = $this->conn->prepare('INSERT INTO user VALUES(:un, :pw, :fname, :lname)');
		$stmt->bindParam(':un', $username);
		$stmt->bindParam(':pw', $password);
		$stmt->bindParam('fname', $firstname);
		$stmt->bindParam('lname', $lastname);

		return $stmt->execute();
	}
	
	public function checkCredentials($username, $password)
	{	
		$stmt = $this->conn->prepare('SELECT password FROM user WHERE username = :un');
		$stmt->bindParam(':un', $username);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);	
		$stmt->execute();

		if( $password == $stmt->fetch()['password'] )
			return true;
		else
			return false;
	}
	
	public function __destruct()
	{
		$this->conn = null;
	}
}

?>
