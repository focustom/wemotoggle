<?PHP

  if (!isset($_GET['cmd'])) die();
  if (!isset($_GET['nam'])) die();
  if (!isset($_GET['who'])) die();
  if (!isset($_GET['msg'])) die();

  $cmd = $_GET['cmd'];
  $nam = $_GET['nam'];
  $who = $_GET['who'];
  $msg = $_GET['msg'];

  $sub = "#".$nam."-".$msg;

///////////  personalize this part  ///////////
  $frm = “focustom@me.com";
///////////////////////////////////////////////

  $hdr = "From: ".$frm;

  $sentOK = 0;
  if (strcmp($cmd, "send") == 0) {
      if (mail($who, $sub, $msg, $hdr)) { $sentOK = 1; }
  } else {
      if (strcmp($cmd, "test") == 0) {
       print "<b>To:</b> ".$who."<br><b>From:</b> ".$frm."<br><b>Subject:</b> ".$sub."<br><b>Msg:</b> ".$msg."<br><br>";
    }
  }
  $result[1] = array();
  $result[1] = array('I', $sentOK);
  print json_encode($result);

?>