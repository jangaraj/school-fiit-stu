<?php
if(!isset($_GET['x1']) || !is_numeric($_GET['x1'])) {
  echo "Value x1 is not valid\n";
  die;
}
if(!isset($_GET['x2']) || !is_numeric($_GET['x2'])) {
  echo "Value x2 is not valid\n";
  die;
}
if(!isset($_GET['y1']) || !is_numeric($_GET['y1'])) {
  echo "Value y1 is not valid\n";
  die;
}
if(!isset($_GET['y2']) || !is_numeric($_GET['y2'])) {
  echo "Value y2 is not valid\n";
  die;
}
passthru("java -jar W:/vinf/java/Sesameclient.jar ".$_GET['x1']." ".$_GET['y1']." ".$_GET['x2']." ".$_GET['y2']);
?>