<?php
class bd_manip{
	/*
		Created by João Carlos Pandolfi Santana 12-02-2016
		Mysqli Connection simple framework
		Example to use:
			$bd = new bd_manip();
			$bd->setTable("test");
			$bd->setKey("id=1");
			$bd->setOrder("id")
			$bd->connectDB();
			$result = $bd->consultAllByType();
			$bd->closeConnection();

		Config:
			Edit de function ConnectDB() with your credentials to acess DB
			[server,user,password,database]
	*/

	var $dados;	  /* $dados = ["valor"],["valor2"] */
	var $campos;  /* $campos = ["campo"],["campo2"] */
	var $tabela;  /* nome da tabela */
	var $colunas; /* Colunas especificas para busca SQL */
	var $t_dados; /* tamanho da lista dados */
	var $chave;   /* chave usada para busca */	
	var $ordem;	  /* ordem de busca */	
	var $sql;	  /* sql para busca personalizada */
	var $con;	  /* Conexao */
	var $join;	  /* Variavel que armazena os joins */
	var $lenJoin; /* Tamanho do join */

	function __construct(){
		$this->join = array();
		$this->lenJoin = 0;
		$this->chave = "";
	}

	//server,login,senha,tabela
	function manualConnectDB($bd){
		$this->con = mysqli_connect($bd[0], $bd[1], $bd[2],$bd[3]) or print (mysqli_connect_error());
		mysqli_set_charset($this->con,"utf8");
	}

	function connectDB(){
		$this->con = mysqli_connect("127.0.0.1","user","pass","table") or print (mysqli_connect_error());
		mysqli_set_charset($this->con,"utf8");
	}

	function closeConnection(){
		mysqli_close($this->con);
	}

	
	function consultAll(){
		$cont = 0;
		$result = array();
		$sql = "SELECT ".(empty($this->colunas)?"*":$this->colunas) ." FROM ".$this->tabela." order by ".$this->ordem;
		$query = mysqli_query($this->con,$sql);
		while($sql = mysqli_fetch_array($query,MYSQLI_ASSOC)){
			$result[$cont] = $sql;
			$cont++;
		}
		return $result;	
	}	
		
	function consultAllByType(){
		$cont = 0;

		$result = array();
		$sql = "SELECT ".(empty($this->colunas)?"*":$this->colunas) ." FROM ".$this->tabela." WHERE ".$this->chave." order by ".$this->ordem;
		$query = mysqli_query($this->con,$sql);
		while($sql = mysqli_fetch_array($query, MYSQLI_ASSOC)){
			$result[$cont] = $sql;
			$cont++;
		}
		return $result;	
	}
	
	function consult(){
		$result = array();
		$sql = "SELECT ".(empty($this->colunas)?"*":$this->colunas) ." FROM ".$this->tabela." where ".$this->chave;
		$query = mysqli_query($this->con,$sql);
		while($sql = mysqli_fetch_array($query,MYSQLI_ASSOC))
			$result = $sql;
		return $result;
	}	
	
	function consultAllWithSql(){
		$cont = 0;
		$result = array();
		$sql = $this->sql;
		$query = mysqli_query($this->con,$sql);
		while($sql = mysqli_fetch_array($query,MYSQLI_ASSOC)){
			$result[$cont] = $sql;
			$cont++;
		}
		return $result;	
	}	

	function remove(){
		$sql = "DELETE FROM ".$this->tabela." WHERE ".$this->chave;
		mysqli_query($this->con,$sql);
	}		
	
	
	function forceRequest(){
		mysqli_query($this->con,"SET FOREIGN_KEY_CHECKS=0;");
	}

	function update(){
		$sql = "UPDATE ".$this->tabela." SET ";
		$campos = $this->campos[0]."='".$this->dados[0]."'";
		for($i=1;$i<$this->t_dados;$i++)
			$campos = $campos.",".$this->campos[$i]."='".$this->dados[$i]."'";
		$sql = $sql.$campos." WHERE ".$this->chave;
		mysqli_query($this->con,$sql);
		return $sql;
	}
	
	private function makeSqlInsert(){
		$sql = "INSERT INTO ".$this->tabela." ";
		$campos = "(".$this->campos[0];
		for($i=1;$i<$this->t_dados;$i++)
			$campos = $campos.",".$this->campos[$i];
		$campos= $campos.")";
		$sql = $sql.$campos." VALUES ";	
		$valores = "('".$this->dados[0]."'";
		for($i=1;$i<$this->t_dados;$i++)
			$valores = $valores.",'".$this->dados[$i]."'";
		$valores= $valores.");";
		$sql = $sql.$valores;
		return $sql;
	}

	function forceInsert(){
		mysqli_query($this->con,"SET FOREIGN_KEY_CHECKS=0;");
		mysqli_query($this->con,$this->makeSqlInsert());
	}

	function insert(){
		$query  = mysqli_query($this->con,$this->makeSqlInsert());
	}

	function buildInnerJoin(){
		$this->sql = "SELECT ".$this->colunas." FROM ".$this->tabela." ";
		for ($i=0; $i < $this->lenJoin; $i++) { 
			$this->sql .= $this->join[$i];
		}
		$this->sql .= " WHERE ".$this->chave;
	}

//Setters

	function setDados($p_dados){
		$this->dados = array();
		$this->campos = array();
		$this->dados = array_values($p_dados);	
		$this->campos = array_keys($p_dados);
		$this->t_dados = count($p_dados)-1;
	}

	function setSafeData($p_dado){
		$this->setData($p_dado);
		$this->removeSuspects($this->campos);
		$this->removeSuspects($this->dados);
	}
	function setSql($p_sql){
		$this->sql = $p_sql;	
	}
	function setTable($p_tab){
		$this->tabela = $p_tab;	
	}
	function setKey($p_chave) {
		$this->chave = $p_chave;
	}
	function setOrder($p_ordem) {
		$this->ordem = $p_ordem;
	}
	function setManualData($p_campos,$p_dados){
		$this->dados  = $p_dados;
		$this->campos = $p_campos;
		$this->t_dados = count($p_dados);
	}
	function setColumns($p_colunas){
		$this->colunas = $p_colunas;
	}

	function setArrayKey($p_operador,$p_chave,$p_arrayDados){
		$tamArray = count($p_arrayDados);
		$this->chave .=" (";
		for ($i=0; $i < $tamArray-1; $i++) { 
			$this->chave .= " ".$p_chave." = '".$this->removeSuspectsFromString($p_arrayDados[$i])."' ".$p_operador;
		}
		$this->chave .= " ".$p_chave." = '".$p_arrayDados[$i]."') ";
	}

	function setNewKey($p_chave){
		$this->chave .= " ".$p_chave;
	}

	function setNewInnerJoin($p_tabela,$p_coluna1,$p_coluna2){
		$this->join[$this->lenJoin] = " INNER JOIN ".$p_tabela." ON ".$p_coluna1." = ".$p_coluna2;
		$this->lenJoin++;
	}

	function flushSql(){
		$this->sql = "";
		$this->lenJoin = 0;
		$this->join = array();
	}

//Regex
	function removeSuspects($array){
		$tam = count($array);
		for($i=0;$i<$tam;$i++)
			$array[$i] = $this->removeSuspectsFromString($array[$i]);
	}
	
	function removeSuspectsFromString($string){
		$string = str_replace("'", "\'", $string);
		$string = str_replace('"', '\"', $string);
		return $string;

		/*
		$strSChar = "áàãâäéèêëíìîïóòõôöúùûüçÁÀÃÂÄÉÈÊËÍÌÎÏÓÒÕÖÔÚÙÛÜÇ";
		$regex = "/([^0-9a-zA-Z*@.:,?!+$() ".$strSChar."]*)/i";//ficam somente os listados aqui
		return preg_replace($regex,"",$string);
		*/
	}
}
?>