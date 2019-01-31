<?php 
require_once("db.php");
if (isset($_GET['coor'])) {
    $device_id = $_GET['device_id'];
    $coor = $_GET['coor'];
    $active = 1;
    
    if ($device_id == '0000') {
        $active = 0;
    }

    $query = "UPDATE Device SET POSITION_INFO = '$coor', ACTIVE_ID = '$active' WHERE DEVICE_ID = '$device_id'";

    $result = mysqli_query($connection, $query);
    
    $active = 1;

    if (!$result) {
        die('Query FAILED').mysql_error();
    } else {
        echo "UPDATED";
    }
}
?>