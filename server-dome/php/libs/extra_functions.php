<?php
	/*
		Arquivo contem funcoes extras
	*/
	define("SEED",10354532);
	date_default_timezone_set("America/Sao_Paulo");

	/*
	* Gera a API_KEY do usuario
	* @receives email
	* @receives id
	* @returns api_key -> base64
	*/
	function generateAPIKey($email,$id){
		return base64_encode("-".$email." -||-".$id."-");
	}

	/*
	* Recupera a data atual no formato MYSQL
	* @returns currentDate -> String
	*/
	function getCurrentDataMYSQLFormat(){
		return date_create()->format('Y-m-d H:i:s');
	}

	/*
	* Retorna a data de controle no formato utilizado atualmente
	*/
	function currentDate(){
		return date("Ymd"); 
	}

	/*
	* Redireciona para a pagina de login
	*/
	function gotoLogin($especialidade){
		header("header:/login/".$especialidade."/");
		echo '<meta http-equiv="refresh" content="0; url=/login/'.$especialidade.'"/ />';
	}


	/*
	* Formata data inserida dd/mm/aaaa -> aaaa-mm-dd
	*/
	function formateDate($date){
		$arr_date = explode("/",$date);
		return $arr_date[2]."-".$arr_date[1]."-".$arr_date[0];
	}

	/*
	* Redireciona para 404
	*/
	function goto404(){
		header("header:/404.html");
		echo '<meta http-equiv="refresh" content="0; url=/404.html"/ />';	
	}

	/*
	* Verifica se a rota pode ser usada
	* Basicamente verifica se o usuário está logado
	* @returns bool {true: safe, false: unsafe}
	*/
	function checkRestSecurity(){
		if(isset($_SESSION[sessionLogged])){
			if($_SESSION[sessionLogged]["expiration"] < currentDate() || $_SESSION[sessionLogged]["logged"] == 0){
				echo "{Error:404}";
				return false;
			}
		}
		else{
			echo "{Error:404}";
			return false;
		}
		return true;
	}

	/*
	* Calcula idade
	*/
	function calcAge($date_param){
		$date =strtotime($date_param);// data de nascimento
		$now = strtotime("now"); // data definida
		return floor((((($now - $date) / 60) / 60) / 24) / 365.25);
	}

	/*
	* Calcula tempo que passou em horas
	*/
	function calcHourPast($date_param){
		$date =strtotime($date_param);// data de nascimento
		$now = strtotime("now"); // data definida
		return floor(((($now - $date) / 60) / 60));
	}

?>