<?php

/*
*	Class to send Email for clients
*/

class Mailer{

	var $quebra_linha;

	function __construct(){
		$this->quebra_linha = "\n";
	}

	function sendMail($emaildestinatario,$assunto,$mensagemHTML,$headers,$emailsender){
		if(!mail($emaildestinatario, $assunto, $mensagemHTML, $headers ,"-r".$emailsender)){ // Se for Postfix
    		$headers .= "Return-Path: " . $emailsender . $this->quebra_linha; // Se "não for Postfix"
    		mail($emaildestinatario, $assunto, $mensagemHTML, $headers );
		}
	}

	function testMailler(){
		$this->sendMail("joaopandolfi@gmail.com","teste de email","<b>teste da funcao de email</b>","","teste@teste.com");
	}
}

?>