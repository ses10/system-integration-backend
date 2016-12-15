#!/usr/bin/php
<?php

require_once('databaseHelper.inc');


$dbHelper = new DatabaseHelper();

$dbHelper->connect();

//$dbHelper->registerUser('test', '123', 't', 'w');

//print_r($dbHelper->getUserInfo('bob100', '123'));

//print_r($dbHelper->getCarRecalls('2001', 'ford', 'focus'));
//print_r($dbHelper->getUserCarRecalls('1'));
//if($dbHelper->checkCredentials('bob100', '123'))
//	echo 'yes';
//else
//	echo 'no';

//echo($dbHelper->addUserCar('bob100', '2001', 'ford', 'focus'));
//print_r($dbHelper->addUserCarRecall(10));

//print_r($dbHelper->toggleRecallCheck(13));

$dbHelper->removeUserCar(14);
?>
