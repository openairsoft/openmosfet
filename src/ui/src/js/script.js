(function() {

	// autoupdate
	document.querySelector("#autoUpdateFromGithub").addEventListener("click" , function(event){
		event.preventDefault(); //prevent default action
		document.querySelector("#autoUpdateFromGithub .buttonText").textContent = "Récupération des informations";
		document.querySelector("#autoUpdateFromGithub .spinner-border").style.display='inline-block';
		document.querySelector("#autoUpdateFromGithub").disabled = true;
		
		//todo: something if fail or something
		fetch('/api/core', { method: 'PATCH' })
		.then(response => response.json())
		.then(jsonResponse => {
		});
	});
	if (!!window.EventSource) {
		var source = new EventSource('/events');

		source.addEventListener('open', function(e) {
			console.log("Events Connected");
		}, false);

		source.addEventListener('error', function(e) {
			if (e.target.readyState != EventSource.OPEN) {
			console.log("Events Disconnected");
			}
		}, false);

		source.addEventListener('updateStarted', function(e) {
			document.querySelector("#autoUpdateFromGithub .buttonText").textContent = "Mise à jour en cours";
		}, false);

		source.addEventListener('updateSuccessful', function(e) {
			document.querySelector("#autoUpdateFromGithub .buttonText").textContent = "Mise à jour terminée, redémarrage";
			new Promise(resolve => setTimeout(resolve, 4000)).then(() => document.location.reload());
		}, false);

		source.addEventListener('updateFailed', function(e) {
			document.querySelector("#autoUpdateFromGithub .buttonText").textContent = "Echec de la mise à jour, réessayer";
			document.querySelector("#autoUpdateFromGithub .buttonText").style.display='inline-block';
			document.querySelector("#autoUpdateFromGithub .buttonText").disabled = false;
		}, false);
	}

	document.querySelector("[data-toggle='collapse-show']").addEventListener("click" , function(event){
		document.querySelector(this.dataset.target).classList.add("show");
	});

	document.querySelector("[data-toggle='collapse-hide']").addEventListener("click" , function(event){
		document.querySelector(this.dataset.target).classList.remove("show");
	});


	// pas sur de garder ça 
	// $(window).scroll(function () {
	// 	if ($(window).scrollTop() > 10) {
	// 	$('#nav-menu').addClass('floatingNav');
	// 	} else {
	// 	$('#nav-menu').removeClass('floatingNav');
	// 	}
	// });

	//initialize values
	fetch('/cfg.json')
	.then(response => response.json())
	.then(data => {
		populateWithJsonData(data);
	});

	document.querySelector("#mainForm").addEventListener("submit", function(event){
		event.preventDefault(); //prevent default action
		document.querySelector("#mainSubmit .spinner-border").style.display='inline-block';
		let config = {
			"appSsid" : document.querySelector("input[name='appSsid']").value,
			"appPasswd" : document.querySelector("input[name='appPasswd']").value,
			"availableNetworkAppSsid" : document.querySelector("input[name='availableNetworkAppSsid']").value,
			"availableNetworkAppPasswd" : document.querySelector("input[name='availableNetworkAppPasswd']").value,
			"wifiShutdownDelayMinutes" : parseInt(document.querySelector("input[name='wifiShutdownDelayMinutes']").value),
			"deepSleepDelayMinutes" : parseInt(document.querySelector("input[name='deepSleepDelayMinutes']").value),
			"batteryLowVoltage" : parseFloat(document.querySelector("input[name='batteryLowVoltage']").value),
			"batteryNominalVoltage" : parseFloat(document.querySelector("input[name='batteryNominalVoltage']").value),
			"batteryShutdownVoltage" : parseFloat(document.querySelector("input[name='batteryShutdownVoltage']").value),
			"connectToNetworkIfAvailable" : document.querySelector("input[name='connectToNetworkIfAvailable']:checked").value == 'true',
			"useBatteryProtection" : document.querySelector("input[name='useBatteryProtection']:checked").value == 'true',
			"useActiveBreaking" : document.querySelector("input[name='useActiveBreaking']:checked").value == 'true',
			"fireModes" : []
		}

		document.querySelectorAll(".firemode").forEach((firemodeElement) => {
			let firemode = {
				"burstMode" : parseInt(firemodeElement.querySelector("input.burstModeInput:checked").value),
				"burstLength" : parseInt(firemodeElement.querySelector("input[name='burstLength']").value),
				"timeBetweenShots_ms" : parseInt(firemodeElement.querySelector("input[name='timeBetweenShots_ms']").value),
				"precockDuration_ms" : parseInt(firemodeElement.querySelector("input[name='precockDuration_ms']").value),
				"motorPower" : parseInt(firemodeElement.querySelector("input[name='motorPower']").value)
			}
			config["fireModes"].push(firemode);
		});

		console.log("config avant envoi : ")
		console.log(config)

		fetch('/api/config', { 
			method: 'POST',
			headers: {
				'Accept': 'application/json',
				'Content-Type': 'application/json'
			},
			body: JSON.stringify(config)
		})
		.then(response => response.json())
		.then(jsonResponse => {
			populateWithJsonData(jsonResponse);
			document.querySelector("#mainSubmit .spinner-border").style.display='none';
		});
	});

	//add a bottom margin so the save button doesn't hides content
	document.querySelector("#mainContainer").style.marginBottom = document.querySelector("#saveContainer").offsetHeight + "px";
	window.addEventListener('resize', function(){
		document.querySelector("#mainContainer").style.marginBottom = document.querySelector("#saveContainer").offsetHeight + "px"
	});
 })();

 function populateWithJsonData(jsonData){
	document.querySelector("input[name='appSsid']").value = jsonData["appSsid"];
	document.querySelector("input[name='appPasswd']").value = jsonData["appPasswd"];
	document.querySelector("input[name='availableNetworkAppSsid']").value = jsonData["availableNetworkAppSsid"];
	document.querySelector("input[name='availableNetworkAppPasswd']").value = jsonData["availableNetworkAppPasswd"];
	document.querySelector("input[name='wifiShutdownDelayMinutes']").value = jsonData["wifiShutdownDelayMinutes"];
	document.querySelector("input[name='deepSleepDelayMinutes']").value = jsonData["deepSleepDelayMinutes"];
	document.querySelector("input[name='batteryLowVoltage']").value = jsonData["batteryLowVoltage"];
	document.querySelector("input[name='batteryNominalVoltage']").value = jsonData["batteryNominalVoltage"];
	document.querySelector("input[name='batteryShutdownVoltage']").value = jsonData["batteryShutdownVoltage"];

	document.querySelector("input[name='connectToNetworkIfAvailable']").checked = false;
	document.querySelector("input[name='connectToNetworkIfAvailable']").parentElement.classList.remove("active");
	document.querySelector("input[name='connectToNetworkIfAvailable'][value='" + jsonData["connectToNetworkIfAvailable"] + "']").checked = false;
	document.querySelector("input[name='connectToNetworkIfAvailable'][value='" + jsonData["connectToNetworkIfAvailable"] + "']").parentElement.classList.add("active");
	document.querySelector("input[name='connectToNetworkIfAvailable'][value='" + jsonData["connectToNetworkIfAvailable"] + "']").click();

	document.querySelector("input[name='useBatteryProtection']").checked = false;
	document.querySelector("input[name='useBatteryProtection']").parentElement.classList.remove("active");
	document.querySelector("input[name='useBatteryProtection'][value='" + jsonData["useBatteryProtection"] + "']").checked = false;
	document.querySelector("input[name='useBatteryProtection'][value='" + jsonData["useBatteryProtection"] + "']").parentElement.classList.add("active");
	document.querySelector("input[name='useBatteryProtection'][value='" + jsonData["useBatteryProtection"] + "']").click();

	document.querySelector("input[name='useActiveBreaking']").checked = false;
	document.querySelector("input[name='useActiveBreaking']").parentElement.classList.remove("active");
	document.querySelector("input[name='useActiveBreaking'][value='" + jsonData["useActiveBreaking"] + "']").checked = false;
	document.querySelector("input[name='useActiveBreaking'][value='" + jsonData["useActiveBreaking"] + "']").parentElement.classList.add("active");
	document.querySelector("input[name='useActiveBreaking'][value='" + jsonData["useActiveBreaking"] + "']").click();

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
		'<div class="card bg-dark mb-3 firemode">'+
			'<h5 class="card-header" data-toggle="collapse" data-target="#collapseMode'+fireModeKey+'" aria-expanded="false" aria-controls="collapseMode'+fireModeKey+'">Mode '+(Number(fireModeKey)+1)+'</h5>'+
			'<div class="card-body collapse show" id="collapseMode'+fireModeKey+'">'+
			'<div class="row">'+

				'<div class="col-md-12 mb-3">'+
				'<label for="inputbBurstMode'+fireModeKey+'">Mode de rafale</label><br>'+
				'<div class="btn-group btn-group-toggle d-flex" role="group" id="inputBurstMode'+fireModeKey+'" data-toggle="buttons">'+
					'<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(1,fireMode["burstMode"])+'">'+
					'<input type="radio" class="burstModeInput" value="1" '+checkedIfEqual(1,fireMode["burstMode"])+' />interruptible'+
					'</label>'+
					'<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(0,fireMode["burstMode"])+'">'+
					'<input type="radio" class="burstModeInput" value="0" '+checkedIfEqual(0,fireMode["burstMode"])+' />normal'+
					'</label>'+
					'<label class="btn btn-outline-primary align-middle w-100 '+activeIfEqual(2,fireMode["burstMode"])+'">'+
					'<input type="radio" class="burstModeInput" value="2" '+checkedIfEqual(2,fireMode["burstMode"])+' />extensible'+
					'</label>'+
				'</div>'+
				'</div>'+
				
				'<div class="form-group col-md-6">'+
				'<label for="inputBurstLength'+fireModeKey+'">Nombre de coups</label>'+
				'<input type="number" value="'+fireMode["burstLength"]+'" name="burstLength" step="1" min="1" max="10" class="form-control bg-dark text-light" id="inputBurstLength'+fireModeKey+'" aria-describedby="inputBurstLengt'+fireModeKey+'hHelp">'+
				'<small id="inputBurstLength'+fireModeKey+'Help" class="form-text text-muted">attention à ne pas mettre trop</small>'+
				'</div>'+

				'<div class="form-group col-md-6">'+
				'<label for="inputTimeBetweenShots_ms'+fireModeKey+'">Délai minimum entre chaque coup</label>'+
				'<div class="input-group">'+
					'<input type="number" value="'+fireMode["timeBetweenShots_ms"]+'" step="1" class="form-control bg-dark text-light" name="timeBetweenShots_ms" id="inputTimeBetweenShots_ms'+fireModeKey+'" aria-describedby="inputTimeBetweenShots_ms'+fireModeKey+'Help">'+
					'<div class="input-group-append">'+
					'<span class="input-group-text" id="inputTimeBetweenShots_ms'+fireModeKey+'-unit">ms</span>'+
					'</div>'+
				'</div>'+
				'<small id="inputTimeBetweenShots_ms'+fireModeKey+'Help" class="form-text text-muted">pour limiter la cadence ou pour une réplique DMR (n\'affecte pas la réactivité du tir)</small>'+
				'</div>'+

				'<div class="form-group col-md-6">'+
				'<label for="inputPrecockDuration_ms'+fireModeKey+'">Durée de précocking</label>'+
				'<div class="input-group">'+
					'<input type="number" value="'+fireMode["precockDuration_ms"]+'" step="1" class="form-control bg-dark text-light" id="inputPrecockDuration_ms'+fireModeKey+'" name="precockDuration_ms" aria-describedby="inputPrecockDuration_ms'+fireModeKey+'Help">'+
					'<div class="input-group-append">'+
					'<span class="input-group-text" id="inputPrecockDuration_ms'+fireModeKey+'-unit">ms</span>'+
					'</div>'+
				'</div>'+
				'<small id="inputPrecockDuration_ms'+fireModeKey+'Help" class="form-text text-muted">0 = pas de précocking</small>'+
				'</div>'+

				'<div class="form-group col-md-6">'+
				'<label for="inputMotorPower'+fireModeKey+'">Puissance du moteur</label>'+
				'<div class="input-group">'+
					'<input type="number" value="'+fireMode["motorPower"]+'" step="1" class="form-control bg-dark text-light" id="inputMotorPower'+fireModeKey+'" name="motorPower" aria-describedby="inputMotorPower'+fireModeKey+'Help">'+
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
	document.querySelector("#panel-firemodes").innerHTML = fireModesHtml;
	BSN.initCallback();//refresh bootstrap native
}