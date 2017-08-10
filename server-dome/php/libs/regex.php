<?php
// ======== EXPRESSÕES REGULARES =======
//formata string para somente numeros
function somenteNumeros($string){
	$regex = "/([^0-9]*)/i";//^ -> nao
	return preg_replace($regex,"",$string);
}

//formata a string para ficar somente a data
function somenteData($string){
	$regex = "/([^0-9-]*)/i";//somente numero e "-"
	return preg_replace($regex,"",$string);
}

//formata string para hora
function somenteHora($string){
	$regex = "/([^0-9:]*)/i";//somente numero e ":"
	return preg_replace($regex,"",$string);
}

//formata string removendo caracteres suspeitos
function removeSuspeitos($string){
	$strSChar = "áàãâäéèêëíìîïóòõôöúùûüçÁÀÃÂÄÉÈÊËÍÌÎÏÓÒÕÖÔÚÙÛÜÇ";
	$regex = "/([^0-9a-zA-Z*@.:,?!+$() ".$strSChar."]*)/i";//ficam somente os listados aqui
	return preg_replace($regex,"",$string);
}
?>