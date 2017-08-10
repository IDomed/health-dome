<?php 
	/*
		Created by João Carlos Pandolfi Santana
		
		This file is responsible to control All routes in Server

		System libs useds:
			Using json_decode() for decode json strings
			Using json_encode() for encode json arrays
		
		Personalized libs useds:
			Using bd_manip for control database
	*/
	define("SRC","/home/u284261513/public_html/les/");

	//Constants
	include_once(SRC."constants/routes_constants.php");

	//Basic
	include_once(SRC."libs/bd_manip.php");

	//Controller
	include_once(SRC."controller/base_controller.php");
	include_once(SRC."controller/user.php");
	include_once(SRC."controller/trip.php");
	include_once(SRC."controller/route.php");
	include_once(SRC."controller/place.php");
	include_once(SRC."controller/error.php");

	//Getting Route code
	$code = $_REQUEST["r_code"];
	$type = $_REQUEST["r_type"];
	$post_data = array();
	parse_str(file_get_contents('php://input'),$post_data);
	$data = array_merge($_REQUEST,$post_data);

	$controller = "null";
	
	//Working
	switch ($type) {
		case $route_types["user"]:
				$controller = new User();			
			break;
		
		case $route_types["trip"]:
				$controller = new Trip();
			break;
		
		case $route_types["route"]:
				$controller = new Route();
			break;
		
		case $route_types["place"]:
				$controller = new Place();
			break;
		
		default:
				$controller = new Error();
			break;
	}

	echo $controller->delegateRoute($code,$route_codes,$data);

?>