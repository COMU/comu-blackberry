$(document).ready(function(){
$("#userLoginForm").validationEngine({promptPosition:"topLeft",inlineValidation: false});
   $('#userLoginForm #email').focus(function(){
         $.validationEngine.closePrompt("#password")  ;    
         $.validationEngine.buildPrompt("#email","e-posta adresiniz","black")  ;
       
   });  
    $('#userLoginForm #password').focus(function(){
        $.validationEngine.closePrompt("#email")  ;    
        $.validationEngine.buildPrompt("#password","þifreniz","black")  ;

   });
     $('#userLoginForm #email').blur(function(){        $.validationEngine.closePrompt("#email")  ;  });   
     $('#userLoginForm #password').blur(function(){        $.validationEngine.closePrompt("#password")  ; });       
   

  
  

    
    $('#nedenuyelink').click(function(){
        jQuery.facebox({ div: '#nedenuyeolmalisiniz' });

      
    });
       	
	$('a.openCampaigns').click(function(){
		if ($('a.forgotPassword').hasClass("activeLink")) {
		
			$("div#forgetPassword").slideToggle("slow");
			$("#mainWrapper").animate({marginTop: "+=120px"}, 1000);
			$('a.forgotPassword').toggleClass("activeLink");
		}
		if ($('a.notMember').hasClass("activeLink")) {
		
			$("div#notAMember").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=200px"}, 1000);
			$('a.notMember').toggleClass("activeLink");
		}
		$(this).addClass("activeLinkFooter");
		$("div#aboutDiv").hide("fast");
		$("div#contactDiv").hide("fast");
		$("div#campaignsDiv").slideToggle("slow");
        $.validationEngine.closePrompt("body");
	});  
	
	$('a.openAbout').click(function(){
		if ($('a.forgotPassword').hasClass("activeLink")) {
		
			$("div#forgetPassword").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=120px"}, 1000);
			$('a.forgotPassword').toggleClass("activeLink");
		}
		if ($('a.notMember').hasClass("activeLink")) {
		
			$("div#notAMember").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=200px"}, 1000);
			$('a.notMember').toggleClass("activeLink");
         
		}
		$(this).addClass("activeLinkFooter");
		$("div#campaignsDiv").hide("fast");
		$("div#contactDiv").hide("fast");
		$("div#aboutDiv").slideToggle("slow");
        $.validationEngine.closePrompt("body")
	}); 
	
	$('div.patternOverlay').click(function(){
		$("div#campaignsDiv").hide();
		$("div#contactDiv").hide();
		$("div#aboutDiv").hide();
	});
    $('div#closecmplist').click(function(){
        alert('sss');
        $("div#campaignsDiv").hide();
        $("div#contactDiv").hide();
        $("div#aboutDiv").hide();
    });
    
     
	
	$('a.openContact').click(function(){
		if ($('a.forgotPassword').hasClass("activeLink")) {
		
			$("div#forgetPassword").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=120px"}, 1000);
			$('a.forgotPassword').toggleClass("activeLink");
		}
		if ($('a.notMember').hasClass("activeLink")) {
		
			$("div#notAMember").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=200px"}, 1000);
			$('a.notMember').toggleClass("activeLink");
		}
		$(this).addClass("activeLinkFooter");
		$("div#campaignsDiv").hide("fast");
		$("div#aboutDiv").hide("fast");
		$("div#contactDiv").slideToggle("slow"); 
        $.validationEngine.closePrompt("body");
	});             
	
	$('a.notMember2').click(function(){
        var position = 0;
		if ($('a.forgotPassword').hasClass("activeLink")) {
		
			$("div#forgetPassword").slideToggle("slow");
           /* alert('step1');
			$("#mainWrapper").animate({
				marginTop: "+=120px"}, 1000);*/
			$('a.forgotPassword').toggleClass("activeLink");
            position = 120;
		}
		if ($(this).hasClass("activeLink")) {
                         
			$(this).toggleClass("activeLink");

			$("#mainWrapper").animate({
				marginTop: "+=200px"}, 1000);
			$("div#notAMember").slideToggle("slow");
		}
		else {
			$(this).toggleClass("activeLink");
                        
             position = 200-position;
			$("#mainWrapper").animate({
				marginTop: "-="+position+"px"}, 1000);
			        $("div#notAMember").slideToggle("slow");
                    $("div#campaignsDiv").hide("slow");
                    $("div#contactDiv").hide("slow");
                    $("div#aboutDiv").hide("slow");
                      
		}
        $.validationEngine.closePrompt("body");
	});
	
	$('a.forgotPassword').click(function(){
		if ($('a.notMember').hasClass("activeLink")) {
		
			$("div#notAMember").slideToggle("slow");
			$("#mainWrapper").animate({
				marginTop: "+=200px"}, 1000);
			$('a.notMember').toggleClass("activeLink");
		}
		if ($(this).hasClass("activeLink")) {
			$(this).toggleClass("activeLink");
			$("#mainWrapper").animate({
				marginTop: "+=120px"}, 1000);
			$("div#forgetPassword").slideToggle("slow");
		}
		else {
			$(this).toggleClass("activeLink");
			$("#mainWrapper").animate({
				marginTop: "-=120px"}, 1000);
			$("div#forgetPassword").slideToggle("slow");
                                $("div#campaignsDiv").hide("slow");
                    $("div#contactDiv").hide("slow");
                    $("div#aboutDiv").hide("slow");
		}
             $.validationEngine.closePrompt("body");
		
	});
	
    $("a.clickForMembership").click(function(){
    
           $('a.notMember').click();
    });
	$("a.close2Btn").click(function(){
		$("#mainWrapper").animate({
			marginTop: "+=120px"}, 1000);
		$("div#forgetPassword").slideToggle("slow");
		$('a.forgotPassword').toggleClass("activeLink");
	});
	
	$("a.closeBtn").click(function(){
		$("#mainWrapper").animate({
			marginTop: "+=200px"}, 1000);
		$("div#notAMember").slideToggle("slow");
		$('a.notMember').toggleClass("activeLink");
	});
    $("#userIsNotAMember .button2").click(function(){         
        $("a.closeBtn").click();
    });
    $("#contactDiv .button2").click(function(){         
        $("div#contactDiv").hide(); 
    });
    
    
    
    
    
    $('.ajax_post').submit(function() {          
          var datas = $(this).serialize();
          var  o = $(this);
          var  res = $(this).find('.res');          
          res.html('<img src="/public/images/loading.gif">');     
          
          var action = $(this).attr("action");
          if(action == '') action = 'index.php';
         
            jQuery.ajax({
                type: 'POST',
                url: action,
                data: datas,
                error:  function(){ o.html("Bir hata algýlandý."); }, 
                success:function(veri) {
                     veri = JSON.parse(veri);
                     res.html(veri.msg);
                     res.css('background', 'Yellow');  
                }
            });      
          
        return false;
      });
    

});
$(window).load(function () {
    load_ajax_images();     
});
function load_ajax_images(){
    $('.ajax-image').each(function(){      
      var img = new Image();
      var o = $(this);
      $(img).load(function () {                
          $(this).hide();
          o.removeClass('loading').append(this);                
          $(this).fadeIn();
        })
        .error(function () { }).attr('src', o.attr('rel'));
    });
    
    $('#bg_img').each(function(){      
      var img = new Image();
      var o = $(this);
      $(img).load(function () {                
          $(this).hide();
          o.removeClass('loading').append(this);
          $(this).fadeIn();
        })
        .error(function () { }).attr('src', o.attr('rel')).attr('class','mainBg');
    });
    
    
    
    
    
    
}