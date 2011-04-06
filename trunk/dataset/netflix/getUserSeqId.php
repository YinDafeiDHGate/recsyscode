<?php
/* Copyright (C) 2011 Lv Hongliang. All Rights Reserved.
 * please maitain the copyright information completely when you redistribute the code.
 * 
 * If there are some bugs, please contact me via email honglianglv@gmail.com or submit the bugs 
 * in the google code project http://code.google.com/p/recsyscode/issues/list
 * 
 * my blog: http://lifecrunch.biz
 * my twitter: http://twitter.com/honglianglv
 * my google profile:https://profiles.google.com/honglianglv/about?hl=en
 *
 * It is free software; you can redistribute it and/or modify it under 
 * the license GPLV3.
 *
 * getUserSeqId.php has two functions：
 * (1) transfer the userIds to a sequential ones 
 * (2) store the map of old userId and new userId
 */

$dirBase = './training_set/';
$dstBase = './transfer_set/';
//第一步给所有的用户编成一个连续的号码，然后将文件重新保存

$userIds = array();
$num = 0;
$userNum = 1;
//文件名example：mv_0000023.txt
$posStart = 0;
$posEnd = 0;
$movieId = 0;
$rateContent = $ret = '';
foreach( glob($dirBase.'*.txt') as $filename)
{
	//针对每一个文件，提取出movie的id
	
	$posStart = strrpos($filename,'_');
	$posEnd = strrpos($filename,'.');
	$movieId = substr($filename,$posStart+1);
	//echo $movieId;die;
	//下面给每一个用户编号，先读取文件内容，然后用正则表达式匹配
	//$rateContent = file_get_contents($filename);
	//$rateArray = explode("\n",$rateContent);
	$handler = fopen($dstBase.$movieId,'w');
	$rateHandler = fopen($filename,'r');
	while (!feof($rateHandler)) 
	{
		$buffer = fgets($rateHandler, 4096);
		$details = explode(',',$buffer);
		$userId = '';
		if(count($details)< 2)continue;
		if(!isset($userIds[$details[0]]))
		{
			$userIds[$details[0]] = $userNum; //给用户编号
			$userId = $userNum;
			++$userNum;
		}
		else
		{
			$userId = $userIds[$details[0]];
		}
		fwrite($handler, $userId.','.$details[1].','.$details[2]);
	}
	fclose($handler);
	fclose($rateHandler);
	echo $filename." is dealed!\n";
}
$userIdStr  = '';
foreach($userIds as $key=>$userId)
{
	$userIdStr .= $key."\t".$userId."\n";
}
file_put_contents('userIds',$userIdStr);
?>
