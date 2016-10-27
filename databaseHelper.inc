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
		
		$stmt = $this->conn->prepare('SELECT * from userCar where username = :un');
		$stmt->bindParam(':un', $username);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();
	
		//$result += $stmt->fetchAll();
		$result['cars'] = $stmt->fetchAll();
		
		return $result;
	}

	public function getCarInfo($year, $make, $model)
	{
		$stmt = $this->conn->prepare('SELECT * from recall WHERE year = :yr AND model = :mdl AND make = :mke');
		$stmt->bindParam(':yr', $year);
		$stmt->bindParam(':mdl', $model);
		$stmt->bindParam(':mke', $make);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();

		$result = $stmt->fetch();

		return $result;	
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

	public function __destruct()
	{
		$this->conn = null;
	}
}

?>
