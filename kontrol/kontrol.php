<?php
if(isset($_POST['kullanici']) && isset($_POST['sifre'])) { // form g�nderilmi� mi
   $kullanici = $_POST['kullanici'];
   $sifre = $_POST['sifre'];
 
   if(empty($kullanici) || empty($sifre)) { // g�nderilenler bo� mu
      echo 'L�tfen bo� b�rakmay�n';
   } else {
      if($kullanici == 'ozden' && $sifre == '123456') { // bilgiler do�ru mu
         session_start();
         $_SESSION['kullanici'] = 'ozden';
         $_SESSION['sifre'] = '123456';
         echo 'Giri� yapt�n�z!';
      } else {
         echo 'Yanl�� kullan�c� ad� ya da �ifre';
      }
   }
} else {
   echo 'L�tfen formu kullan�n';
}
?>