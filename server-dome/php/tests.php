<?php

/*
* Classe responsavel por testes automaticos das rotas
* Raproveitada do projeto: NossaTiop
* By João Pandolfi
* (c) 2017
*/

class Tester{
	var $passeds = 0;
	var $faileds = 0;

	function test($name, $lambda, $expected){

		$result = $lambda();
		if($result == $expected){
			$this->passeds += 1;
			echo "PASSED TEST: ".$name."<BR>";
		}
		else {
			echo "FAILED TEST: ".$name." --- [EXPECTED: $expected] -- [RETURNED: $result] <BR>";	
			$this->faileds++;
		}
	}

	function printResult(){
		echo "<BR>PASSEDS: ".$this->passeds."<br>";
		echo "FAILEDS: ".$this->faileds."<BR>";
	}

}

function postData($url, $arrayData){
	$postdata = http_build_query($arrayData);

	$opts = array('http' =>
    	array(
        	'method'  => 'POST',
        	'header'  => 'Content-type: application/x-www-form-urlencoded',
        	'content' => $postdata
    	)
	);

	$context  = stream_context_create($opts);

	return file_get_contents($url, false, $context);
}

$t = new Tester();

echo "=================== TDD ======================<BR>";

$t->test("login Success",function(){
	$user = base64_encode("jordan");
	$pass = "MTIz";

	$r = file_get_contents("http://restfull.hol.es/les/user/login/".$user."/".$pass);
	$json = json_decode($r,true);

	return $json["data"]["user_id"];

}, "5");


$t->test("login Success - leo",function(){
	$user = base64_encode("leo");
	$pass = "MTIz";

	$r = file_get_contents("http://restfull.hol.es/les/user/login/".$user."/".$pass);
	$json = json_decode($r,true);

	return $json["success"];

}, "1");

$t->test("My trips - ID 1",function(){
	$id = "1";

	$r = file_get_contents("http://restfull.hol.es/les/user/trips/my/".$id);
	$json = json_decode($r,true);

	return $json["data"][1]["id_trip"];

}, "1");


$t->test("My trips - ID 10",function(){
	$id = "10";

	$r = file_get_contents("http://restfull.hol.es/les/user/trips/my/".$id);
	$json = json_decode($r,true);

	return count($json["data"]);

}, "1");



$t->test("Place Search 'a'",function(){
	$search = "a";

	$r = file_get_contents("http://restfull.hol.es/les/place/search/".$search);
	$json = json_decode($r,true);

	return $json["data"]["name_place"];

}, "Lugar teste");



echo "=================== BDD ======================<BR>";


$t->test("[Usuario digita login e senha] -- [usuario busca suas viagens]",function(){
	$user = base64_encode("leo");
	$pass = "MTIz";

	$r = file_get_contents("http://restfull.hol.es/les/user/login/".$user."/".$pass);
	$json = json_decode($r,true);

	$id =  $json["data"]["user_id"];

	$r = file_get_contents("http://restfull.hol.es/les/user/trips/my/".$id);
	$json = json_decode($r,true);

	return $json["data"][1]["id_trip"];

}, "1");

$t->test("[Usuario faz comentario] -- [Busca comentario]",function(){
	$user_id = 1;
	$user_hash = "LWpvcmRhbiAtfHwtMTAzNTQ1MzIt";
	$trip_id= 1;

	$r = postData("http://restfull.hol.es/les/trip/comment/".$trip_id."/".$user_id."/".$user_hash,array("comments"=> "Comentario bosta"));
	$json = json_decode($r,true);
	

	$r = file_get_contents("http://restfull.hol.es/les/trip/show/".$trip_id);
	$json = json_decode($r,true);

	$last_comment = array_pop($json["data"]["comments"]);

	return $last_comment["comments"];

}, "Comentario bosta");

$t->test("[Usuario Cria Trip] -- [Busca Trip]",function(){
	$user_id = 1;
	$user_hash = "LWpvcmRhbiAtfHwtMTAzNTQ1MzIt";
	$trip_id= 0;

	$r = postData("http://restfull.hol.es/les/user/trips/new/".$user_id."/".$user_hash,
		array("title"=> "TRIP TESTE",
			"short_route" => "short route - teste",
			"description" => "Essa trip é só teste",
			"tags" => "ppk;triplouca;comprebaton"));
	$json = json_decode($r,true);
	
	$trip_id = $json["data"]["id_trip"];
	$r = file_get_contents("http://restfull.hol.es/les/trip/show/".$trip_id);
	$json = json_decode($r,true);

	return $json["data"]["title"];

}, "TRIP TESTE");



echo "==========================================<br>";
$t->printResult();
?>