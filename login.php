<html>
    <body>
	    <form method=”POST”>
		    <input type=”text” name=”name”>
		    <input type=”psw” name=”psw”
		    <input type=”submit”>
	    </form>
    </body>
</html>
<?php
    $name=$_POST[‘name’];
    $psw=$_POST[‘psw’];
    if ($name == “ubuntu” and $psw == “ubuntu”) {
	    ob_clean();
		header(‘Location: https://zapatopi.net/treeoctopus/’, true, 302);
    	die();
    }
    else {
    	echo “Sorry, Invalid login.”;
    }
?>
