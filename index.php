<head>
    <link type="text/css" rel="stylesheet" href="http://cgi.cse.unsw.edu.au/~z5019263/revuemail/stylesheet.css"/>
    <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js"></script>  
    <script src="https://code.jquery.com/jquery-1.10.2.js"></script>
    <script src="./index.js"></script>  
    
    <!-- Latest compiled and minified CSS -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

<!-- Optional theme -->
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">

<!-- Latest compiled and minified JavaScript -->
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>
		<title>Roster</title>
</head>

<body>
  
  

  <div class="container" style="width:50%">
    <div class="row" style="padding:5px">
      <label class="col-md-3"> Upload text file </label>
    </div>
    <div class="row" style="padding:5px">
      <input class="col-md-6" type="file" id="fileName"> </input>
    </div>
    <div class="row" style="padding:5px">
      <button class="pull-left" id="upload">Upload</button>
    </div>
    <div class="row" style="padding:5px">
      <button class="pull-left" id="run">Run</button>
    </div>
  

  
    <div class="row" style="padding:5px">
      <label> 
        <?php
          if(!empty($_POST['data'])) {
            echo "Ready!";
          }
        ?>
    </div>
    <div class="row" style="padding:5px">
      <textarea class="col-md-6">
        <?php 

          if (isset($_POST['run'])) {
            echo "RUNNING";
            run();
          } 
        ?>
      </textarea> 
     </div class="row" style="padding:5px">
  </div>

</body>
<?php
  #unset the library path so c++ code can run properly
  putenv("LD_LIBRARY_PATH");
  #print_r($_POST);
  if(!empty($_POST['data'])){
     
    $data = $_POST['data'];
    $fname = "uploaded.txt";
    
    $file = fopen($fname, 'w');//creates new file
    fwrite($file, $data);
    fclose($file);
  }
  
  function run() {
    exec('./Step 4 uploaded.txt 2>&1', $output, $return_value);
    echo ($return_value == 0 ? 'OK' : 'Error: status code ' . $return_value) . PHP_EOL;
    echo 'Output: ' . PHP_EOL . implode(PHP_EOL, $output);
  }

  function output() {
    $myfile = fopen("output.txt", "r") or die("Unable to open file!");
    echo fread($myfile,filesize("output.txt"));
    fclose($myfile);
  }
  
  function createRoster() {
    echo "alert()";
    
  }

?>



