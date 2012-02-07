(function($) {
	$.fn.validationEngineLanguage = function() {};
	$.validationEngineLanguage = {
		newLang: function() {
			$.validationEngineLanguage.allRules = 	{"required":{    			// Add your regex rules here, you can take telephone as an example
						"regex":"none",
						"alertText":"* Bu alan� doldurmak zorunlu",
						"alertTextCheckboxMultiple":"* L�tfen bir se�im yap�n�z",
						"alertTextCheckboxe":"* Onayl�yorsan�z, l�tfen onay kutusunu i�aretleyiniz"},
					"length":{
						"regex":"none",
						"alertText":"* ",
						"alertText2":" ve ",
						"alertText3": " karakter aras� uzunlukta olmal�"},
					"maxCheckbox":{
						"regex":"none",
						"alertText":"* se�enek s�n�r� a��ld�"},	
					"minCheckbox":{
						"regex":"none",
						"alertText":"* ",
						"alertText2":" Se�enek belirtmelisiniz."},	
					"confirm":{
						"regex":"none",
						"alertText":"* �ifre e�le�miyor"},		
					"telephone":{
						"regex":"/^[0-9\-\(\)\ ]+$/",
						"alertText":"* Ge�ersiz telefon numaras�"},	
					"email":{
						"regex":"/^[a-zA-Z0-9_\.\-]+\@([a-zA-Z0-9\-]+\.)+[a-zA-Z0-9]{2,4}$/",
						"alertText":"* Ge�ersiz e-posta adresi"},	
					"date":{
                         "regex":"/^[0-9]{4}\-\[0-9]{1,2}\-\[0-9]{1,2}$/",
                         "alertText":"* Ge�ersiz tarih, format : YYYY-MM-DD"},
					"onlyNumber":{
						"regex":"/^[0-9\ ]+$/",
						"alertText":"* Sadece numara"},	
					"noSpecialCaracters":{
						"regex":"/^[0-9a-zA-Z]+$/",
						"alertText":"* �zel karakter kullanmay�n�z"},	
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
						"alertText":"* Sadece harflerden olu�abilir"}
					}	
		}
	}
})(jQuery);

$(document).ready(function() {	
	$.validationEngineLanguage.newLang()
});