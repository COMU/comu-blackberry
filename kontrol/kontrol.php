<?php
if(isset($_POST['kullanici']) && isset($_POST['sifre'])) { // form gönderilmiþ mi
   $kullanici = $_POST['kullanici'];
   $sifre = $_POST['sifre'];
 
   if(empty($kullanici) || empty($sifre)) { // gönderilenler boþ mu
      echo 'Lütfen boþ býrakmayýn';
   } else {
      if($kullanici == 'ozden' && $sifre == '123456') { // bilgiler doðru mu
         session_start();
         $_SESSION['kullanici'] = 'ozden';
         $_SESSION['sifre'] = '123456';
         echo 'Giriþ yaptýnýz!';
      } else {
         echo 'Yanlýþ kullanýcý adý ya da þifre';
      }
   }
} else {
   echo 'Lütfen formu kullanýn';
}
?>