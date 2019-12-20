(function ($) {
  $("[data-toggle='collapse-show']").click(function () {
    $($(this).attr("data-target")).addClass("show");
    //$($(this).attr("data-target")).collapse("show");//this don't work well because not interuptible
  })
  $("[data-toggle='collapse-hide']").click(function () {
    $($(this).attr("data-target")).removeClass("show");
    //$($(this).attr("data-target")).collapse("hide");//this don't work well because not interuptible
  })

  $(window).scroll(function () {
    if ($(window).scrollTop() > 10) {
      $('#nav-menu').addClass('floatingNav');
    } else {
      $('#nav-menu').removeClass('floatingNav');
    }
  });

  function populateWithJsonData(jsonData){
    $("input[name='appSsid']").val(jsonData["appSsid"]);
    $("input[name='appPasswd']").val(jsonData["appPasswd"]);
    $("input[name='availableNetworkAppSsid']").val(jsonData["availableNetworkAppSsid"]);
    $("input[name='availableNetworkAppPasswd']").val(jsonData["availableNetworkAppPasswd"]);
    $("input[name='wifiShutdownDelayMinutes']").val(jsonData["wifiShutdownDelayMinutes"]);
    $("input[name='deepSleepDelayMinutes']").val(jsonData["deepSleepDelayMinutes"]);
    $("input[name='batteryLowVoltage']").val(jsonData["batteryLowVoltage"]);
    $("input[name='batteryNominalVoltage']").val(jsonData["batteryNominalVoltage"]);
    $("input[name='batteryShutdownVoltage']").val(jsonData["batteryShutdownVoltage"]);

    $("input[name='connectToNetworkIfAvailable']").prop('checked', false).parent().removeClass("active");
    $("input[name='connectToNetworkIfAvailable'][value='" + jsonData["connectToNetworkIfAvailable"] + "']").prop('checked', true).parent().addClass("active").click();

    $("input[name='useBatteryProtection']").prop('checked', false).parent().removeClass("active");
    $("input[name='useBatteryProtection'][value='" + jsonData["useBatteryProtection"] + "']").prop('checked', true).parent().addClass("active").click();

    $("input[name='useActiveBreaking']").prop('checked', false).parent().removeClass("active");
    $("input[name='useActiveBreaking'][value='" + jsonData["useActiveBreaking"] + "']").prop('checked', true).parent().addClass("active").click()

    let checkedIfEqual = function(number1,number2){
      return Number(number1) == Number(number2)?"checked":""
    }
    let activeIfEqual = function(number1,number2){
      return Number(number1) == Number(number2)?"active":""
    }
    let fireModesHtml = "";
    for (fireModeKey in jsonData["fireModes"]) {
      let fireMode = jsonData["fireModes"][fireModeKey];

      fireModesHtml +=
      '<div class="card bg-dark mb-3">'+
        '<h5 class="card-header" data-toggle="collapse" data-target="#collapseMode'+fireModeKey+'" aria-expanded="false" aria-controls="collapseMode'+fireModeKey+'">Mode '+(Number(fireModeKey)+1)+'</h5>'+
        '<div class="card-body collapse show" id="collapseMode'+fireModeKey+'">'+
          '<div class="row">'+

            '<div class="col-md-12 mb-3">'+
              '<label for="inputbBurstMode'+fireModeKey+'">Mode de rafale</label><br>'+
              '<div class="btn-group btn-group-toggle d-flex" role="group" id="inputBurstMode'+fireModeKey+'" data-toggle="buttons">'+
                '<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(1,fireMode["burstMode"])+'" data-toggle="collapse-show">'+
                '<input type="radio" name="burstMode'+fireModeKey+'" value="1" '+checkedIfEqual(1,fireMode["burstMode"])+'>interruptible'+
              '</label>'+
                '<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(0,fireMode["burstMode"])+'" data-toggle="collapse-hide">'+
                '<input type="radio" name="burstMode'+fireModeKey+'" value="0" '+checkedIfEqual(0,fireMode["burstMode"])+'>normal'+
              '</label>'+
                '<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(2,fireMode["burstMode"])+'" data-toggle="collapse-hide">'+
                '<input type="radio" name="burstMode'+fireModeKey+'" value="2" '+checkedIfEqual(2,fireMode["burstMode"])+'>extensible'+
              '</label>'+
              '</div>'+
            '</div>'+
            
            '<div class="form-group col-md-6">'+
              '<label for="inputBurstLength'+fireModeKey+'">Nombre de coups</label>'+
              '<input type="number" value="'+fireMode["burstLength"]+'" name="burstLength'+fireModeKey+'" step="1" min="1" max="10" class="form-control bg-dark text-light" id="inputBurstLength'+fireModeKey+'" aria-describedby="inputBurstLengt'+fireModeKey+'hHelp">'+
              '<small id="inputBurstLength'+fireModeKey+'Help" class="form-text text-muted">attention à ne pas mettre trop</small>'+
            '</div>'+

            '<div class="form-group col-md-6">'+
              '<label for="inputTimeBetweenShots_ms'+fireModeKey+'">Délai minimum entre chaque coup</label>'+
              '<div class="input-group">'+
                '<input type="number" value="'+fireMode["timeBetweenShots_ms"]+'" step="1" class="form-control bg-dark text-light" name="timeBetweenShots_ms'+fireModeKey+'" id="inputTimeBetweenShots_ms'+fireModeKey+'" aria-describedby="inputTimeBetweenShots_ms'+fireModeKey+'Help">'+
                '<div class="input-group-append">'+
                  '<span class="input-group-text" id="inputTimeBetweenShots_ms'+fireModeKey+'-unit">ms</span>'+
                '</div>'+
              '</div>'+
              '<small id="inputTimeBetweenShots_ms'+fireModeKey+'Help" class="form-text text-muted">pour limiter la cadence ou pour une réplique DMR (n\'affecte pas la réactivité du tir)</small>'+
            '</div>'+

            '<div class="form-group col-md-6">'+
              '<label for="inputPrecockDuration_ms'+fireModeKey+'">Durée de précocking</label>'+
              '<div class="input-group">'+
                '<input type="number" value="'+fireMode["precockDuration_ms"]+'" step="1" class="form-control bg-dark text-light" id="inputPrecockDuration_ms'+fireModeKey+'" name="precockDuration_ms'+fireModeKey+'" aria-describedby="inputPrecockDuration_ms'+fireModeKey+'Help">'+
                '<div class="input-group-append">'+
                  '<span class="input-group-text" id="inputPrecockDuration_ms'+fireModeKey+'-unit">ms</span>'+
                '</div>'+
              '</div>'+
              '<small id="inputPrecockDuration_ms'+fireModeKey+'Help" class="form-text text-muted">0 = pas de précocking</small>'+
            '</div>'+

            '<div class="form-group col-md-6">'+
              '<label for="inputMotorPower'+fireModeKey+'">Puissance du moteur</label>'+
              '<div class="input-group">'+
                '<input type="number" value="'+fireMode["motorPower"]+'" step="1" class="form-control bg-dark text-light" id="inputMotorPower'+fireModeKey+'" name="motorPower'+fireModeKey+'" aria-describedby="inputMotorPower'+fireModeKey+'Help">'+
                '<div class="input-group-append">'+
                  '<span class="input-group-text" id="inputMotorPower'+fireModeKey+'-unit">%</span>'+
                '</div>'+
              '</div>'+
              '<small id="inputMotorPower'+fireModeKey+'Help" class="form-text text-muted">peut affecter la réactivité</small>'+
            '</div>'+

          '</div>'+
        '</div>'+

      '</div>'
    }
    $("#panel-firemodes").html(fireModesHtml);
  }


  $("#mainForm").submit(function(event){
    event.preventDefault(); //prevent default action
    var form_data = $(this).serialize(); //Encode form elements for submission
    $("#mainSubmit .spinner-border").show();
    $("#mainSubmit .buttonText").hide();
    $.ajax({
      url : "/json",
      type: "POST",
      data : form_data
    }).done(function(response){ //
      populateWithJsonData(response);
      $("#mainSubmit .spinner-border").hide();
      $("#mainSubmit .buttonText").show();
    });
  });

  //initialize values
   jQuery.getJSON("/cfg.json", function (data) {
    populateWithJsonData(data);
  });

  //add a bottom margin so the save button doesn't hides content
  $("#mainContainer").css("margin-bottom",$("#saveContainer").height());
  $( window ).resize(function() {
    $("#mainContainer").css("margin-bottom",$("#saveContainer").height());
  });
  

})(jQuery);