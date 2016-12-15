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
		{/*
			$host = "192.168.2.42";
			exec("ping -c 1 -w 1 " . $host, $output, $result);

			if($result == 0)
			{
			  $this->conn = new PDO('mysql:host=192.168.2.42;dbname=recall', 'test', 'abc123');
			}
			else
			{
			  $this->conn = new PDO('mysql:host=192.168.2.43;dbname=recall', 'test', 'abc123');
			}
*/
			//$this->conn = new PDO('mysql:host=192.168.2.20;dbname=it490', 'root', 'Clloyd20');
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

		
		//$stmt = $this->conn->prepare('SELECT year, make, model from userCar where username = :un');
		$stmt = $this->conn->prepare('SELECT id, year, make, model from userCar where username = :un');
		$stmt->bindParam(':un', $username);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();
	
		//$result += $stmt->fetchAll();
		$result['cars'] = $stmt->fetchAll();
		
		return $result;
	}

//	old version
	public function getCarRecalls($year, $make, $model)
	{
		$stmt = $this->conn->prepare('SELECT * FROM recall WHERE year = :yr AND model = :mdl AND make = :mke');
		//$stmt = $this->conn->prepare('SELECT year, make, model, manufacturer, campaignNum, reportReceived, component, summary, consequence, remedy, notes, lookupTime from recall WHERE year = :yr AND model = :mdl AND make = :mke');
		$stmt->bindParam(':yr', $year);
		$stmt->bindParam(':mdl', $model);
		$stmt->bindParam(':mke', $make);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();

		$result = $stmt->fetchAll();

		return $result;	
	}
	
	//returns recalls for a user's car
        public function getUserCarRecalls($userCarId)
        {
                //$stmt = $this->conn->prepare('select userCarRecall.checked, userCarRecall.uuid,recall.* FROM userCarRecall INNER JOIN recall ON recall.id=userCarRecall.recallId where userCarRecall.id = :id');
		$stmt = $this->conn->prepare('select userCarRecall.checked, userCarRecall.uuid,recall.year,recall.make,recall.model,recall.manufacturer,recall.campaignNum,recall.reportReceived,recall.component,recall.summary,recall.consequence,recall.remedy,recall.notes,recall.lookupTime FROM userCarRecall INNER JOIN recall ON recall.id=userCarRecall.recallId where userCarRecall.id = :id');

                $stmt->bindParam(':id', $userCarId);
                $stmt->setFetchMode(PDO::FETCH_ASSOC);
                $stmt->execute();

                $result = $stmt->fetchAll();

                return $result;
        }
	
	//adds new entries to UserCarRecall table
	public function addUserCarRecall($userCarId)
	{
		//get year make model of given id
		$stmt = $this->conn->prepare('SELECT year, make, model from userCar where id = :id');
		$stmt->bindParam(':id', $userCarId);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();		

		$res = $stmt->fetch();
		$year = $res['year'];
		$make = $res['make'];
		$model = $res['model'];

		//get all recall ids of given year,make,model
		$results = $this->getCarRecalls($year,$make,$model);

		//for each id insert row into table with userCarId
		foreach($results as $result)
		{
		   $recallId = $result['id'];
		   $this->conn->query("insert into userCarRecall (id, recallId, checked) values($userCarId,$recallId,0)");
		} 
	}

	public function removeUserCar($userCarId)
	{
		$stmt = $this->conn->prepare('DELETE FROM userCarRecall where id = :id');
		$stmt->bindParam(':id', $userCarId);
		$stmt->execute();

		$stmt = $this->conn->prepare('DELETE FROM userCar where id = :id');
		$stmt->bindParam(':id', $userCarId);
                $stmt->execute();
	}

	public function insertRecalls($recalls)
	{
		foreach($recalls as $recall)
		{


			//$stmt = $this->conn->prepare('INSERT INTO recall VALUES(:yr, :mke, :mdl, :manu, :camNum, :rep, :compon, :sum, :con, :rem, :notes, CURDATE())');

			$stmt = $this->conn->prepare('INSERT INTO recall (year, make, model, manufacturer, campaignNum, reportReceived, component, summary, consequence, remedy, notes, lookupTime) VALUES(:yr, :mke, :mdl, :manu, :camNum, :rep, :compon, :sum, :con, :rem, :notes, CURDATE())');

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
		$stmt = $this->conn->prepare('INSERT INTO userCar (username, year, make, model) VALUES(:un, :yr, :mke, :mdl)');
		$stmt->bindParam(':un', $username);
		$stmt->bindParam(':yr', $year);		
		$stmt->bindParam(':mke', $make);
		$stmt->bindParam(':mdl', $model);

		$stmt->execute();
		return $this->conn->lastInsertId();
	}

	public function toggleRecallCheck($uuid)
	{
		$stmt = $this->conn->prepare('SELECT * FROM userCarRecall WHERE uuid = :uuid');
		$stmt->bindParam(':uuid', $uuid);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();

		$check = $stmt->fetch()['checked'];

		if($check == 0)
		  $check = 1;
		else if($check == 1)
		  $check = 0;


		$stmt = $this->conn->query("UPDATE userCarRecall SET checked = $check WHERE uuid = $uuid");
		//$stmt->bindParam(':uuid', $uuid);
		
		return $check;


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
