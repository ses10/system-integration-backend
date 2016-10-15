<?php

class DatabaseHelper
{
	private $conn;

	public function __construct()
	{
		try
		{
			$this->conn = new PDO('mysql:host=192.168.2.40;dbname=it490', 'ds646', 'abc123');
		}
		catch(PDOException $e)
		{
			echo 'Error: ' . $e->getMessage();
		}
	}

	public function getUserInfo($username, $password)
	{
		$stmt = $this->conn->prepare('SELECT first_name, last_name FROM user WHERE username = :un AND password = :pw');
		$stmt->bindParam(':un', $username);
		$stmt->bindParam(':pw', $password);

		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$stmt->execute();
		return $stmt->fetch();
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
