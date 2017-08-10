<?php
error_reporting(0);
session_start();

define("MAX_SIZE","9000");

function getExtension($str) {

    $i = strrpos($str,".");

    if (!$i) {
        return "";
    }

    $l = strlen($str) - $i;

    $ext = substr($str,$i+1,$l);

    return $ext;
}

$valid_formats = array("jpg", "png", "gif", "bmp","jpeg");

if (isset($_POST) and $_SERVER['REQUEST_METHOD'] == "POST") {

    $uploaddir = "uploads/"; // directoria que vai receber os ficheiros

    foreach ($_FILES['photos']['name'] as $name => $value) {

        $filename = stripslashes($_FILES['photos']['name'][$name]);

        $size=filesize($_FILES['photos']['tmp_name'][$name]);

        /* Recolhe extensão do ficheiro em caixa baixa (lowercase)
         */
        $ext = getExtension($filename);
        $ext = strtolower($ext);

        if (in_array($ext,$valid_formats)) {
            if ($size < (MAX_SIZE*1024)) {
                $image_name=time().$filename;
                $newname=$uploaddir.$image_name;
                if (move_uploaded_file($_FILES['photos']['tmp_name'][$name], $newname)) {
                    /* ficheiro carregado com sucesso,
                     * envia HTML com imagem para apresentar ao visitante
                     */
                    echo "{success:1,file:'".$uploaddir.$image_name."',name:'".$image_name."'";
                } else {
                    echo '{success:0,error:3,message:"Ficheiro não foi carregado!"}';
                }
            } else {
                echo '{success:0,error:2,message:"Limite de tamanho atingido!"}';
            }
        } else {
            return '{success:0,error:1,message:"Extensão do ficheiro desconhecida!"}';
        }
    }

}

?>