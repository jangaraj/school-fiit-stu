<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>Návrh prekladačov - zadanie</title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta name="robots" content="index,follow" />
  <meta name="googlebot" content="index,follow,snippet,archive" />
  <meta name="copyright" content="2007 Garaj, Fábik" />
  <link rel="stylesheet" type="text/css" href="./css/style.css" />
  <link rel="shortcut icon" href="./favicon.ico" />    
</head>
<body>
<div style="margin-left: 25px;">
<div style="_height: 500px; min-height: 500px;">
<img src="./top.jpg" alt="top obrazok" /><br />

<?php
/*
 * Navrh prekladacov
 * Semanticka a syntakticka analyza pseudojazyka
 * Jan Garaj & Pavol Fabik
 */
    
  define('MAXIMALNA_DLZKA_KLUCOVEHO_SLOVA',5);  //maximálna dĺžka kľúčového slova
  if(isset($_POST['debug'])) {
    $debug = true;
  }
  else 
    $debug = false;
    
  $Pocet = 0;
 
  if(isset($_POST['data']) && $_POST['data']!='') {
    //lexikalna analyza
    $LexikalneJednotky = array();
    $Pozicia = array();
    $HtmlLexikalnaAnalyza = LexikalnaAnalyza($_POST['data'], $LexikalneJednotky, $Pozicia, $Pocet, $debug);
    if(!empty($LexikalneJednotky)) {
      $HtmlLexikalneJednotka = 'Vyparsované lexikálne jednotky:<br>';
      foreach($LexikalneJednotky as $key=>$value) {
        $HtmlLexikalneJednotka .= " ".$value;
      }
      $HtmlLexikalnaAnalyza = $HtmlLexikalneJednotka."<br><br>".$HtmlLexikalnaAnalyza;
    }
    if( preg_match('/<font color="red">/', $HtmlLexikalnaAnalyza) ) {
      //semanticka analyza skoncila s chybami
      $HtmlLexikalnaAnalyza = "<b>Sematická analýza našla chyby.</b><br>".$HtmlLexikalnaAnalyza;
    }
    else {
      $HtmlLexikalnaAnalyza = "<b>Sematická analýza úspešná.</b><br>".$HtmlLexikalnaAnalyza;
      $Zasobnik = array();      
      $HtmlSyntaktickaAnalyza = SyntaktickaAnalyza($LexikalneJednotky, $Pozicia, $Zasobnik, $debug); 
    }    
    //semanticka analyza    
    echo "<a href=\"?\">Naspäť na zadávací formulár</a><br><br>";
    echo $HtmlLexikalnaAnalyza."<br><br>".$HtmlSyntaktickaAnalyza."<br><br>";
    
    //farebne vypisanie zdrojaka
    $Vystup = '';
    $Last = array();
    foreach($Pozicia as $key=>$value) {      
      if(!empty($Last)) {
       if(isset($Last['error'])) $Vystup .= "<font color=\"red\">";
       $Vystup .= str_replace(" ","&nbsp;", substr($_POST['data'], $Last['start'], $value['start']-$Last['start'] ));
       if(isset($Last['error'])) $Vystup .= "</font>";
      } 
      $Last = $value;
    }
    if(isset($Last['error'])) $Vystup .= "<font color=\"red\">";
    $Vystup .= substr($_POST['data'], $Last['start'], strlen($_POST['data'])-$Last['start'] );
    if(isset($Last['error'])) $Vystup .= "</font>";    
    $Vystup = str_replace(array("\n"), array("<br>"), $Vystup);
    echo $Vystup;

  }
  else {
    //generovanie vstupneho formulara
    echo "<div>";
    for($i=1;$i<11;$i++) {      
      echo '<a href="?file=input'.$i.'">Vstup'.$i.'</a>';
      if($i!=10) echo " | ";
    }
    echo "</div><form action=\".\" method=\"post\">";
    echo '<textarea name="data" rows="10" cols="150" style="width: 540px;">';
    if(isset($_GET['file']) && $_GET['file']!='') {
      echo @file_get_contents(str_replace(".","",$_GET['file']).".txt");
    }
    echo "</textarea><br />";
    echo "<input type=\"checkbox\" name=\"debug\" /> Debug výpis<br /><br />";
    echo '<input type="submit" value="Analyzovať" /></form>';
    
  } 
 
  $StackChars = array();

  function LexikalnaAnalyza($Data, &$LexikalnaJednotka, &$Pozicia, &$Pocet, $debug) {    
    $Riadok = 1;
    $Stlpec = 1;    
    $Pocet = 0; //počet nájdených lexikálnych jednotiek
    $Vystup = '';
    $DlzkaData = strlen($Data);
    $AktualnaPozicia = 0;
    $DlzkaJednotky = 0;
    while ($AktualnaPozicia != $DlzkaData) {
      $Znak = substr($Data, $AktualnaPozicia, 1);
      $AktualnaPozicia++;
    
      $MinPocet = $Pocet;
      if (JeToBielyZnak($Znak) == true) {
        $DlzkaJednotky = 1;  
        //oddeľovacie znaky - ignorujeme
        if($debug) $Vystup .= "Nájdený biely znak - ASCII číslo ".ord($Znak)."<br>";
        if (ord($Znak) == 10) {
          $Riadok++; $Stlpec = 1; 
        }
        else {
          $Stlpec++;
        }        
        continue;
      } 
      elseif (JeToPismeno($Znak) == true) {
        //kľúčové slovo / identifikátor            
        $Slovo = '';        
        $DlzkaSlova = 0;      
        while (JeToPismeno($Znak) == true || JeToCislica($Znak) == true) {        
          $Stlpec++;
          if ($DlzkaSlova <= MAXIMALNA_DLZKA_KLUCOVEHO_SLOVA) {
            $Slovo .= $Znak;            
            $DlzkaSlova++;
            $DlzkaJednotky++;
          }          
          
          $Znak = substr($Data, $AktualnaPozicia, 1); {          
          $AktualnaPozicia++;
          }
        }
        $AktualnaPozicia--; //aby som sa vratil naspat o jeden znak zo zistovania slova         
        $LexikalnaJednotka[$Pocet] = KlucoveSlovo($Slovo);        
        if ($LexikalnaJednotka[$Pocet] === false) {
          //nie je to kľúčové slovo
          $LexikalnaJednotka[$Pocet] = 'ident';
          if($debug) $Vystup .= "Najdený identifikátor ".$Slovo."<br>";
        }
        else {
          if($debug) $Vystup .= "Najdené kľúčové slovo ".$Slovo."<br>";
        }
        $Pocet++;
      } 
      elseif (JeToCislica($Znak) == true) {
        //číslo
        if($debug) $Vystup .= "Najdené číslo ";
        do { 
          $Stlpec++; 
          $DlzkaJednotky++;
          if($debug) $Vystup .= $Znak;
          $Znak = substr($Data, $AktualnaPozicia, 1); $AktualnaPozicia++; //$Znak = NacitajZnak(Subor);
        } while (JeToCislica($Znak) == true);
        $AktualnaPozicia--;
        if($debug) $Vystup .= "<br>";
        $LexikalnaJednotka[$Pocet] = 'number';
        $Pocet++;
      } 
      elseif ($Znak == ':') {   
        //priradenie
        $DlzkaJednotky++;
        $Stlpec++;
        $Znak = substr($Data, $AktualnaPozicia, 1); $AktualnaPozicia++; //Znak = NacitajZnak(Subor);
        if ($Znak != '=') {
          //chyba priradenia
          $Vystup .= "<font color=\"red\">Chyba priradenia na riadku ".$Riadok." $Pozícii ".$Stlpec.", očakávaný znak '='</font><br>";
        }
        else {
          if($debug) $Vystup .= "Nájdené priradenie :=<br>";          
          $LexikalnaJednotka[$Pocet] = ':=';
        }
        $Pocet++;
        $Stlpec++;
        $DlzkaJednotky++;        
      } 
      elseif (JeToSymbol($Znak)) {
        //špeciálny symbol       
        switch ($Znak)
        {
          case '+': 
            $Znak = '+';
            if($debug) $Vystup .= "Nájdený symbol +<br>"; 
            break;
          case '-': 
            $Znak = '-';
            if($debug) $Vystup .= "Nájdený symbol -<br>"; 
            break;
          case ';': 
            $Znak = ';'; 
            if($debug) $Vystup .= "Nájdený symbol ;<br>";
            break;
          case ',': 
            $Znak = ','; 
            if($debug) $Vystup .= "Nájdený symbol ,<br>";
            break;
          case '(': 
            $Znak = '('; 
            if($debug) $Vystup .= "Nájdený symbol (<br>";
            break;
          case ')': 
            $Znak = ')'; 
            if($debug) $Vystup .= "Nájdený symbol )<br>";
            break;
        }
        $LexikalnaJednotka[$Pocet] = $Znak;
        $Pocet++;
        $Stlpec++;
        $DlzkaJednotky++;
      } 
      else {   
        //chybný znak
        $Vystup .= "<font color=\"red\">Nájdený chybný znak ".$Znak." na riadku ".$Riadok." $Pozícii ".$Stlpec."</font><br>";
        $DlzkaJednotky++;
        //return false;
      }
  
      if ($MinPocet != $Pocet) {
        $Pozicia[$MinPocet] = array('X'=>$Stlpec, 'Y'=>$Riadok, 'start'=>$AktualnaPozicia-$DlzkaJednotky );
      }              
    }     
    return $Vystup;
  }

  function JeToBielyZnak($Znak) {
    if(preg_match('/\s/',$Znak))
      return true; 
    else 
      return false;
  }
  
  function JeToPismeno($Znak) {
    if (preg_match('/[A-Za-z]/',$Znak) )
      return true; 
    else
      return false;
  }  
  
  function JeToCislica($Znak) {
    if ( preg_match('/[0-9]/', $Znak) ) 
      return true; 
    else
      return false;
  }
  
  function JeToSymbol($Znak) {
    if ($Znak == '+' || $Znak == '-' || $Znak == ';' || $Znak == ',' || $Znak == '(' || $Znak == ')')
      return true; 
    else
      return false;
  }  
  
  function KlucoveSlovo($Slovo) {
    switch (substr(strtoupper($Slovo),0,1)) {
      case 'A':
        //možno AND
        if (preg_match('/^AND$/',$Slovo))
          return 'AND';
        break;
      case 'B':
        //možno BEGIN
        if (preg_match('/^BEGIN$/',$Slovo)) {        
          return 'BEGIN';
        }
        break;
      case 'E':
        //možno ELSE/END
        switch (strtoupper(substr($Slovo,1,1)))
        {
          case 'L':
            if (preg_match('/^ELSE$/',$Slovo))
              return 'ELSE';
            break;
          case 'N':
            if (preg_match('/^END$/',$Slovo))
              return 'END';
            break;
        }
        break;
      case 'F':
        //možno FALSE
        if (preg_match('/^FALSE$/',$Slovo))
          return 'FALSE';
        break;
      case 'I':
        //možno IF
        if (preg_match('/^IF$/',$Slovo))
          return 'IF';
        break;
      case 'N':
        //možno NOT
        if (preg_match('/^NOT$/',$Slovo))
          return 'NOT';
        break;
      case 'O':
        //možno OR
        if (preg_match('/^OR$/',$Slovo))
          return 'OR';
        break;
      case 'R':
        //možno READ
        if (preg_match('/^READ$/',$Slovo))
          return 'READ';
        break;
      case 'T':
        //možno THEN/TRUE
        switch (strtoupper(substr($Slovo, 1, 1)))
        {
          case 'H':
            if (preg_match('/^THEN$/',$Slovo))
              return 'THEN';
            break;
          case 'R':
            if (preg_match('/^TRUE$/',$Slovo))
              return 'TRUE';
            break;
        }
        break;
      case 'W':
        //možno WRITE
        if (preg_match('/^WRITE$/',$Slovo))
          return 'WRITE';
        break;
    }
    return false;
  }  

  function SyntaktickaAnalyza($LexikalnaJednotka, &$Pozicia, $Zasobnik, $debug) {
  //tabuľka prechodov
  $Prechody = array(
                               /* nič BEGIN END IF THEN ELSE                                                         READ WRITE AND OR                                    NOT TRUE FALSE                    ident number :=   +   -                                                   ;   ,   (   ) */
    'program'=>            array( 'nic'=>'CH', 'BEGIN'=>1,    'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'statement_list'=>     array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>2,    'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>2,    'WRITE'=>2,    'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>2,    'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'statement_list_ext'=> array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>4,    'IF'=>3,    'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>3,    'WRITE'=>3,    'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>3,    'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'statement'=>          array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>8,    'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>6,    'WRITE'=>7,    'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>5,    'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'statement_ext'=>      array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>10,   'THEN'=>'CH', 'ELSE'=>9,    'READ'=>10,   'WRITE'=>10,   'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>10,   'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>10,   ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'id_list'=>            array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>11,   'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'id_list_ext'=>        array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>12,   '('=>'CH', ')'=>13 ),
    'expr_list'=>          array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>14,   'number'=>14,   ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>14,   ')'=>'CH' ),
    'expr_list_ext'=>      array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>15,   '('=>'CH', ')'=>16 ),
    'expression'=>         array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>17,   'number'=>17,   ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>17,   ')'=>'CH' ),
    'expression_ext'=>     array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>18,   '-'=>18,   ';'=>19,   ','=>19,   '('=>'CH', ')'=>19 ),
    'factor'=>             array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>21,   'number'=>22,   ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>20,   ')'=>'CH' ),
    'op'=>                 array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>23,   '-'=>24,   ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>'CH' ),
    'bexpr'=>              array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>25,   'TRUE'=>25,   'FALSE'=>25,   'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>25,   ')'=>'CH' ),
    'bexpr_ext'=>          array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>27,   'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>26,   'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>27 ),
    'bterm'=>              array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>28,   'TRUE'=>28,   'FALSE'=>28,   'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>28,   ')'=>'CH' ),
    'bterm_ext'=>          array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>30,   'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>29,   'OR'=>30,   'NOT'=>'CH', 'TRUE'=>'CH', 'FALSE'=>'CH', 'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>'CH', ')'=>30 ),
    'bfactor'=>            array( 'nic'=>'CH', 'BEGIN'=>'CH', 'END'=>'CH', 'IF'=>'CH', 'THEN'=>'CH', 'ELSE'=>'CH', 'READ'=>'CH', 'WRITE'=>'CH', 'AND'=>'CH', 'OR'=>'CH', 'NOT'=>31,   'TRUE'=>33,   'FALSE'=>34,   'ident'=>'CH', 'number'=>'CH', ':='=>'CH', '+'=>'CH', '-'=>'CH', ';'=>'CH', ','=>'CH', '('=>32,   ')'=>'CH' )
  );
  
  //tabuľka pravidiel
  $Pravidla = array(
            array(                      ),
    /* 1*/  array(  2, 23,  1           ),
    /* 2*/  array( 24, 25,              ),
    /* 3*/  array( 23,                  ),
    /* 4*/  array(                      ),
    /* 5*/  array( 18, 31, 15, 13,      ),
    /* 6*/  array( 18, 21, 27, 20, 6,   ),
    /* 7*/  array( 18, 21, 29, 20, 7,   ),
    /* 8*/  array( 26, 25,  4, 35, 3,   ),    
    /* 9*/  array( 18, 25, 5,           ),
    /*10*/  array(                       ),
    /*11*/  array( 28, 13,              ),
    /*12*/  array( 27, 19,              ),
    /*13*/  array(                      ),
    /*14*/  array( 30, 31,              ),
    /*15*/  array( 29, 19,              ),
    /*16*/  array(                      ),
    /*17*/  array( 32, 33,              ),
    /*18*/  array( 32, 33, 34,          ),
    /*19*/  array(                      ),
    /*20*/  array( 21, 31, 20,          ),
    /*21*/  array( 13,                  ),
    /*22*/  array( 14,                  ),
    /*23*/  array( 16,                  ),
    /*24*/  array( 17,                  ),
    /*25*/  array( 36, 37,              ),
    /*26*/  array( 36, 37, 9,           ),
    /*27*/  array(                      ),
    /*28*/  array( 38, 39,              ),
    /*29*/  array( 38, 39,  8,          ),
    /*30*/  array(                      ),
    /*31*/  array( 39, 10,              ),
    /*32*/  array( 21, 35, 20,          ),
    /*33*/  array( 11,                  ),
    /*34*/  array( 12,                  )
  );
  
  
  //textová reprezentácia symbolov - kvôli výpisu
  $NazvySym = array(
    "Prazdny zasobnik",
  //terminály
    /* 1*/ "BEGIN",
    /* 2*/ "END",
    /* 3*/ "IF",
    /* 4*/ "THEN",
    /* 5*/ "ELSE",
    /* 6*/ "READ",
    /* 7*/ "WRITE",
    /* 8*/ "AND",
    /* 9*/ "OR",
    /*10*/ "NOT",
    /*11*/ "TRUE",
    /*12*/ "FALSE",
    /*13*/ "ident",
    /*14*/ "number",
    /*15*/ ":=",
    /*16*/ "+",
    /*17*/ "-",
    /*18*/ ";",
    /*19*/ ",",
    /*20*/ "(",
    /*21*/ ")",
  //neterminály
    /*22*/ "program",
    /*23*/ "statement_list",
    /*24*/ "statement_list_ext",
    /*25*/ "statement",
    /*26*/ "statement_ext",
    /*27*/ "id_list",
    /*28*/ "id_list_ext",
    /*29*/ "expr_list",
    /*30*/ "expr_list_ext",
    /*31*/ "expression",
    /*32*/ "expression_ext",
    /*33*/ "factor",
    /*34*/ "op",
    /*35*/ "bexpr",
    /*36*/ "bexpr_ext",
    /*37*/ "bterm",
    /*38*/ "bterm_ext",
    /*39*/ "bfactor"
  );  
  
  
    $Poz = 0;  //pozicia v lexikalnych jednotkach
    $Vstup;    //aktuálny vstupny znak
    $Zas;      //prvok zo zasobnika
    $Cinnost;  //cinnost ktora sa ide vykovat, 0-redukcia, >0 expanzia, <0 chyba
    $Vystup = '';
    $PocetLJ = sizeof($LexikalnaJednotka);
    array_push($Zasobnik, $NazvySym[0]);  //aby som mal dno zasobnika
    array_push($Zasobnik, 'program');  
    while ($Poz < $PocetLJ) {    
      $Vstup = $LexikalnaJednotka[$Poz];
      if($debug) $Vystup .= "<br><b>Vstup</b> ".$Vstup."<br>";

      do {  
        $Zas = array_pop($Zasobnik);
        if ($Zas == $Vstup) {
          //redukcia
          $Cinnost = 0;
          if($debug) $Vystup .= "<b>Redukcia</b><br>symbol ".$Vstup."<br>";           
        } 
        elseif( !isset($Prechody[$Zas]) || !isset($Prechody[$Zas][$Vstup]) ) {        
          //syntaktická chyba         
          $Cinnost = -150;
        } 
        else {
          //zistenie činnosti z tabuľky prechodov          
          $Cinnost = $Prechody[$Zas][$Vstup];
          if($Cinnost == "CH") $Cinnost = -150; //chyba                    
        }         
        if ($Cinnost > 0) {
          if($debug) $Vystup .= "<b>Expanzia:</b><br>stack1: ";
          $Expanzia = '';
          foreach($Zasobnik as $key=>$value) {
            $Expanzia = $value." ".$Expanzia;  
          }
          if($debug) $Vystup .= $Zas." ".$Expanzia."<br>";        
          //natlacim pravidla do zasobnika
          foreach($Pravidla[$Cinnost] as $key=>$value){
            array_push($Zasobnik, $NazvySym[$value]);
          }
          if($debug) $Vystup .= "stack2: ";
          $Expanzia = '';
          foreach($Zasobnik as $key=>$value) {
            $Expanzia = $value." ".$Expanzia;  
          }
          if($debug) $Vystup .= $Expanzia."<br>";                       
        }
  
        if ($Cinnost < 0) {
          $Vystup .= "<font color=\"red\">Syntaktická chyba na riadku čislo ".$Pozicia[$Poz]['Y'].", na pozícii ".$Pozicia[$Poz]['X']."</font><br>";
          $Pozicia[$Poz]['error'] = 1;
        }
  
      } while ($Cinnost > 0);
      $Poz++;
    }
  
    if ((sizeof($Zasobnik)!=1 && $Zasobnik[0]!=$NazvySym[0]) || preg_match('/<font color="red">/', $Vystup) ) {    
      //v zasobniku nemam iba dno zasobnika
      $Vystup .= "<font color=\"red\"><b>Syntaktická analýza neúspešná</b></font>";
    } 
    else {
      $Vystup .="<b>Syntaktická analýza úspešná</b>";
    }
    return $Vystup;
  }  

?>
</div>
<div style="color: black; font-size: 10px;">© GAFA studios 2007, Ján Garaj, Pavol Fábik | <a href="./index.phps" target="./blank">Zdrojový súbor</a> | <a href="./garaj-fabik-np.pdf" target="_blank">Dokumentácia</a></div>
</div>
</body>
</html>
