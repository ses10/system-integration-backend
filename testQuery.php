#!/usr/bin/php
<?php

require_once('databaseHelper.inc');


$dbHelper = new DatabaseHelper();

$dbHelper->connect();

print_r($dbHelper->getUserInfo('bob100', '123'));

?>
