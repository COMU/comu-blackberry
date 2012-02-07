(function($) {
	$.fn.validationEngineLanguage = function() {};
	$.validationEngineLanguage = {
		newLang: function() {
			$.validationEngineLanguage.allRules = 	{"required":{    			// Add your regex rules here, you can take telephone as an example
						"regex":"none",
						"alertText":"* Bu alaný doldurmak zorunlu",
						"alertTextCheckboxMultiple":"* Lütfen bir seçim yapýnýz",
						"alertTextCheckboxe":"* Onaylýyorsanýz, lütfen onay kutusunu iþaretleyiniz"},
					"length":{
						"regex":"none",
						"alertText":"* ",
						"alertText2":" ve ",
						"alertText3": " karakter arasý uzunlukta olmalý"},
					"maxCheckbox":{
						"regex":"none",
						"alertText":"* seçenek sýnýrý aþýldý"},	
					"minCheckbox":{
						"regex":"none",
						"alertText":"* ",
						"alertText2":" Seçenek belirtmelisiniz."},	
					"confirm":{
						"regex":"none",
						"alertText":"* Þifre eþleþmiyor"},		
					"telephone":{
						"regex":"/^[0-9\-\(\)\ ]+$/",
						"alertText":"* Geçersiz telefon numarasý"},	
					"email":{
						"regex":"/^[a-zA-Z0-9_\.\-]+\@([a-zA-Z0-9\-]+\.)+[a-zA-Z0-9]{2,4}$/",
						"alertText":"* Geçersiz e-posta adresi"},	
					"date":{
                         "regex":"/^[0-9]{4}\-\[0-9]{1,2}\-\[0-9]{1,2}$/",
                         "alertText":"* Geçersiz tarih, format : YYYY-MM-DD"},
					"onlyNumber":{
						"regex":"/^[0-9\ ]+$/",
						"alertText":"* Sadece numara"},	
					"noSpecialCaracters":{
						"regex":"/^[0-9a-zA-Z]+$/",
						"alertText":"* Özel karakter kullanmayýnýz"},	
					"ajaxUser":{
						"file":"validateUser.php",
						"extraData":"name=eric",
						"alertTextOk":"* This user is available",	
						"alertTextLoad":"* Loading, please wait",
						"alertText":"* This user is already taken"},	
					"ajaxName":{
						"file":"validateUser.php",
						"alertText":"* This name is already taken",
						"alertTextOk":"* This name is available",	
						"alertTextLoad":"* Loading, please wait"},		
					"onlyLetter":{
						"regex":"/^[a-zA-Z\ \']+$/",
						"alertText":"* Sadece harflerden oluþabilir"}
					}	
		}
	}
})(jQuery);

$(document).ready(function() {	
	$.validationEngineLanguage.newLang()
});