#!/usr/bin/php
<?php

require_once('databaseHelper.inc');


$dbHelper = new DatabaseHelper();

$dbHelper->connect();

$dbHelper->registerUser('test', '123', 't', 'w');

//print_r($dbHelper->getUserInfo('bob100', '123'));

//print_r($dbHelper->getCarInfo('2001', 'ford', 'focus'));

?>
