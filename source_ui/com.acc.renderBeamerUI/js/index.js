/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, window, location, CSInterface, SystemPath, themeManager*/

data_struct = "";
parsed = "";
currentRowId = "";

// top nav buttons toggle functionality
function settingsButtonToggle() {
    document.getElementById("settingsBtn").classList.toggle("button-selected");
    document.getElementById("assetsBtn").classList.remove("button-selected");
    document.getElementById("settings").classList.remove("hide");
    document.getElementById("assets").classList.add("hide");  
}
function assetsButtonToggle() {
    document.getElementById("settingsBtn").classList.remove("button-selected");
    document.getElementById("assetsBtn").classList.toggle("button-selected");
    document.getElementById("settings").classList.add("hide");
    document.getElementById("assets").classList.remove("hide");
}

function setPixelFormat(PFvalidIds) {
	var pixelFormatParent = document.getElementById("pixelFormat-value").pixelFormatValue.parentElement.getElementsByClassName("item");
	
	for(var x=0; x<pixelFormatParent.length; x++){          
        if(PFvalidIds.includes(x)){			
            if(pixelFormatParent[x].classList.contains("hide")){   
                pixelFormatParent[x].classList.remove("hide");
            } 
        }
        else if(pixelFormatParent[x].classList.contains("hide")==false){			
			pixelFormatParent[x].classList.add("hide");        
        }
    }	
}
function setProfileParent(PvalidIds) {
	var profileParent = document.getElementById("profile-value").profileValue.parentElement.getElementsByClassName("item");
	
    for(var x=0; x<profileParent.length; x++){            
        if(PvalidIds.includes(x)){
            if(profileParent[x].classList.contains("hide")){   
                profileParent[x].classList.remove("hide");
            }
        }
        else if(profileParent[x].classList.contains("hide")==false){   
			profileParent[x].classList.add("hide");
        }
    }	
}
function setPixelAndProfileValues(PFvalidIds, PvalidIds, AddHide)
{
	var pixelFormatValue = document.getElementById("pixelFormat-value");
	var profileValue = document.getElementById("profile-value");
	
	if(profileValue.parentElement.parentElement.classList.contains("hide")){
		profileValue.parentElement.parentElement.classList.remove("hide");
	}
	else if(AddHide == true) {
		profileValue.parentElement.parentElement.classList.add("hide");
	}
    if(dropdownName=="encoder"){
		pixelFormatValue.innerHTML = pixelFormatParent[PFvalidIds[0]].innerHTML;
		if(AddHide == false) {
			profileValue.innerHTML = profileParent[PvalidIds[0]].innerHTML;
		}
	}
}
function setVideoEncoder(PFvalidIds, PvalidIds, AddHide) {	
	setPixelFormat(PFvalidIds);
	if(AddHide == false) {
		setProfileParent(PvalidIds);
	}	
	setPixelAndProfileValues(PFvalidIds, PvalidIds, AddHide);
}

// setting up video settings dropdowns elements based on selected encoder 
function setVideoSettings(args){
    var encoderValue = document.getElementById("encoder-value");;
	var dropdownName = "";
    if(args[2] == true){
        if(parsed[args[3]-1].encoder != ""){
            encoderValue = parsed[args[3]-1].encoder;
        }      
    }
    else{        
        var dropParent = args[0].parentElement.parentElement.parentElement;
        if(dropParent.getElementsByClassName("label").length == 0){
           dropdownName = dropParent.parentElement.parentElement.getElementsByClassName("label")[0].innerHTML;
        }
        else{
            dropdownName = dropParent.getElementsByClassName("label")[0].innerHTML;
        }
    }
   
    var PFvalidIds = [];
    var PvalidIds =  [];
	var AddHide = false;
       
    if(encoderValue.innerHTML == "H265/HEVC"){
		PFvalidIds = [0,3,5,10,11,12,13,14,15,16,17,18,20,21,22,23];
		PvalidIds = [1,2,3,4,5,6,7,8,9,10,11,14,15,16];
    }
    if(encoderValue.innerHTML == "H264/AVC"){
		PFvalidIds = [0,1,3,4,5,6,7,8,9,11,12,14,15,20,21,22,23];
		PvalidIds = [0,1,17,18,19,20];
    }
    if(encoderValue.innerHTML == "VC3/DNxHD"){
		PFvalidIds = [3,12,13,15];
		PvalidIds = [21,22,23,24,25,26];
	}
    if(encoderValue.innerHTML == "ProRes"){
		PFvalidIds = [12,13,27];
		PvalidIds = [27,28,29,30,31];
    }
    if(encoderValue.innerHTML == "HAP"){
		PFvalidIds = [12,13,27];
		PvalidIds = [32,33,34];
    }
	if(encoderValue.innerHTML == "VP9"){
		PFvalidIds = [0,2,3,5,10,11,12,13,14,16,17,19,20,21,22,23];
		AddHide = true;
    }
    if(encoderValue.innerHTML == "AOMedia Video 1(AV1)"){
		PFvalidIds = [0,3,5,11,12,13,16,17,18,20];
		AddHide = true;
    }
	if(encoderValue.innerHTML == "QuickTime Animation RLE"){
		PFvalidIds = [21,24,25,26];
		AddHide = true;
    }
	if(PFvalidIds[0] != undefined) {
		setVideoEncoder(PFvalidIds, PvalidIds, AddHide);
	}
}
//callback function for dropdown item event listener 
function setDropdownValue(args) {
    var ar = args[0];
    var atr = args[1];
    var manual = args[2]; //executed from composition list 
    var attribute ="";
    var dropdown = "";
    var topLabel ="";
    
    var extensionValue = document.getElementById("extensionValue");
        
    var encoderValue = document.getElementById("encoder-value");
    var encoderParent = encoderValue.parentElement.getElementsByClassName("item");

    var pixelFormatValue = document.getElementById("pixelFormat-value");
    var pixelFormatParent = pixelFormatValue.parentElement.getElementsByClassName("item");

    var profileValue = document.getElementById("profile-value");
    var profileParent = profileValue.parentElement.getElementsByClassName("item");
    
    var sampleRateValue = document.getElementById("sampleRate-value");
    var bitDepthValue = document.getElementById("bitDepth-value");
    var channelsValue = document.getElementById("channels-value");
    
	if(manual==false) {
		attribute = ar.getAttribute("data-el");
		if(attribute == "") {
			return;
		}
		dropdown = ar.parentElement.parentElement;
        if(ar.parentElement.parentElement.classList.contains("top-item")){
            dropdown = dropdown.parentElement.parentElement;
        }
		dropdown.classList.remove("dropdownClicked");
        dropdown.getElementsByClassName("dropdown-value")[0].innerHTML = attribute;
        topLabel = ar.parentElement.parentElement.getElementsByClassName("item")[0].innerHTML;
        
        // set object pproperties
        var dropName  = dropdown.parentElement.getElementsByClassName("label")[0].innerHTML;
        var currentObj = parsed[args[3]-1];
        if(dropName == "encoder"){
           currentObj.encoder = attribute;
        }
        if(dropName == "pixel format"){
           currentObj.pixel_format = attribute;
        }
        if(dropName == "profile"){
           currentObj.profile = attribute;
        }
        if(dropName == "extension"){                
            currentObj.encoder =  "";
            currentObj.pixel_format = "";
            currentObj.profile = "";
        }
        
        if(dropName == "sample rate"){
            currentObj.audio_sample_rate = sampleRateValue.innerHTML; 
			sampleRateValue.innerHTML = (currentObj.audio_sample_rate).toString() + " kHz";
        }
        if(dropName == "bit depth"){
            currentObj.audio_depth = bitDepthValue.innerHTML;
			bitDepthValue.innerHTML = (currentObj.audio_depth * 8).toString() + " bit";
        }
        if(dropName == "channels"){
            currentObj.audio_channels = channelsValue.innerHTML;
			if(currentObj.audio_channels == 1) {
				channelsValue.innerHTML = "mono";
			}
			else {
				channelsValue.innerHTML = "stereo";
			}
        }
        if(dropName == "extension"){
            currentObj.ext = extensionValue.innerHTML;  
        }
	}
	else {
		var videoExt = new Array("mp4","mkv","mov","webm","mxf");
		var imgSeqExt = new Array("DPX","IFF","EXR","PNG","PSD","SGI","TIFF");
		var audioExt = new Array("wav");
		
		attribute = atr;  
		if(attribute == "") {
			return;
		}	
		dropdown = document.getElementById("extensionValue");
		dropdown.classList.remove("dropdownClicked");	
		
		if(videoExt.includes(attribute)){			
			topLabel = "video";
			parsed[args[3]-1].is_sequence = 0;
		}
		else if(imgSeqExt.includes(attribute.split(" ")[0])){
			topLabel = "image sequence";
			parsed[args[3]-1].is_sequence = 1;
		}
		else if(audioExt.includes(attribute.split(" ")[0].toLowerCase())){
			topLabel = "audio";
			parsed[args[3]-1].is_sequence = 0;
		}
				
		if(parsed[args[3]-1].audio_sample_rate == ""){
			parsed[args[3]-1].audio_sample_rate = 48000;
		}            
		if(parsed[args[3]-1].audio_depth == ""){
			arsed[args[3]-1].audio_depth = 2;
		}
		sampleRateValue.innerHTML = (parsed[args[3]-1].audio_sample_rate).toString() + " kHz"; 
		bitDepthValue.innerHTML = (parsed[args[3]-1].audio_depth * 8).toString() + " bit";
		
		if(parsed[args[3]-1].audio_channels == ""){
			parsed[args[3]-1].audio_channels = 2;
		}
		if(parsed[args[3]-1].audio_channels == 1) {
			channelsValue.innerHTML = "mono";
		}
		else {
			channelsValue.innerHTML = "stereo";
		}
		extensionValue.innerHTML = parsed[args[3]-1].ext;
	}
	if(attribute == "not set") { /* display smth ? */ }
	if(topLabel=="video") {
		
            // show video settings container
		if(document.getElementById("videoLabel").classList.contains("hide")) {
			document.getElementById("videoLabel").classList.remove("hide");
			document.styleSheets[0].deleteRule(0);    
		}		
		var x=0;
		if(attribute=="mp4") {
			encoderValue.innerHTML = "H265/HEVC";
			for(x=0;x<encoderParent.length;x++) {
				if(encoderParent[x].classList.contains("hide")) {   
					encoderParent[x].classList.remove("hide");
				}     
			}			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.innerHTML = "main";
			profileValue.parentElement.parentElement.classList.remove("hide");
		}
		if(attribute=="mkv"){
			encoderValue.innerHTML = "H265/HEVC";
			for(x=0;x<encoderParent.length;x++){
				if(encoderParent[x].classList.contains("hide")){   
					encoderParent[x].classList.remove("hide");
				}
				else{
					encoderParent[x].classList.add("hide");
				}     
			}
			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.innerHTML = "main";
			profileValue.parentElement.parentElement.classList.remove("hide");
		}
		if(attribute=="mov"){
			encoderValue.innerHTML = "H265/HEVC";
			for(x=0;x<encoderParent.length;x++){
				encoderParent[x].classList.remove("hide");  
			}
			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.innerHTML = "main";
			profileValue.parentElement.parentElement.classList.remove("hide");
		}
		if(attribute=="webm"){
			encoderValue.innerHTML = "VP9";
			for(x=0;x<encoderParent.length;x++){
				if(x != 4 && x != 5){
					if(encoderParent[x].classList.contains("hide")==false){   
						encoderParent[x].classList.add("hide");
					}
				}
				else{
					if(encoderParent[x].classList.contains("hide")){   
						encoderParent[x].classList.remove("hide");
					}
				}
			}
			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.parentElement.parentElement.classList.add("hide");
			}
		if(attribute=="mxf"){
			encoderValue.innerHTML = "H264/AVC";
			for(x=0;x<encoderParent.length;x++){
				if(x != 1 ){
					if(encoderParent[x].classList.contains("hide")==false){   
						encoderParent[x].classList.add("hide");
					}
				}
				else{
					if(encoderParent[x].classList.contains("hide")){   
						encoderParent[x].classList.remove("hide");
					}
				}
			}                
			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.innerHTML = "baseline";
			profileValue.parentElement.parentElement.classList.remove("hide");
		}
		
		var framerate = document.getElementById("framerate"); 
		var bitrate = document.getElementById("bitrate"); 
		
		//video settings
		if(parsed[args[3]-1].encoder != "" ){
			parsed[args[3]-1].encoder = encoderValue.innerHTML;
		}
		if(parsed[args[3]-1].pixel_format == "" ){
			parsed[args[3]-1].pixel_format = pixelFormatValue.innerHTML;
		}
		if(parsed[args[3]-1].profile == ""){
			parsed[args[3]-1].profile = profileValue.innerHTML;
		}
		if(parsed[args[3]-1].fps == ""){ 
			parsed[args[3]-1].fps = framerate.value;
		}			
		if(parsed[args[3]-1].bitrate == "") {
			parsed[args[3]-1].bitrate = bitrate.value; 
		}
		encoderValue.innerHTML = parsed[args[3]-1].encoder;
		pixelFormatValue.innerHTML = parsed[args[3]-1].pixel_format;
		profileValue.innerHTML = parsed[args[3]-1].profile;
		bitrate.value = parsed[args[3]-1].bitrate;
		framerate.value = parsed[args[3]-1].fps;
	}
	else if(topLabel=="image sequence") {
		// hide video settings container
		if(document.getElementById("videoLabel").classList.contains("hide")==false) {
			document.getElementById("videoLabel").classList.add("hide");
			document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
		}
	}
    else if(topLabel=="audio") {
            // hide video settings container
		if(document.getElementById("videoLabel").classList.contains("hide")==false) {
			document.getElementById("videoLabel").classList.add("hide");
			document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
		}
		// set audio settings
		
		document.getElementById("extensionValue").innerHTML = "wav";
		
		var audioValues = attribute.split(" ");
		bitDepthValue.innerHTML = audioValues[2];
		if(audioValues[1] == "44kHz"){
			sampleRateValue.innerHTML = "44100";
		}
		else if(audioValues[1] == "48kHz"){
			sampleRateValue.innerHTML = "48000";
		}
		else if(audioValues[1] == "88kHz"){
			sampleRateValue.innerHTML = "88200";
		}
		else if(audioValues[1] == "96kHz"){
			sampleRateValue.innerHTML = "96000";
		}
	}
	if(manual){
		extensionValue.innerHTML = attribute;
	}
	else {
		curentRow.getElementsByTagName("td")[2].innerHTML = document.getElementById("extensionValue").innerHTML;
	}
	setVideoSettings(args);  
}

// callback function for composition list row event
function compoListClicked(ar){
    curentRow = ar; 
	
    var allRows = document.getElementById("compositionList").getElementsByTagName("TR");
		
    for(x=1; x<allRows.length; x++){
        allRows[x].style.background = "#2c2d30";
        if(allRows[x].getElementsByTagName("input")[0].getAttribute("data-el")==curentRow.getElementsByTagName("input")[0].getAttribute("data-el")){
           currentRowId = x;
		   allRows[x].style.background = "#3c87d1";
        }
    }
  
    var frameRange = document.getElementById("frameRange");
    var extensionValue = document.getElementById("extensionValue");
    frameRange.value = curentRow.getElementsByTagName("td")[1].innerHTML;
    extensionValue = curentRow.getElementsByTagName("td")[2].innerHTML;
    
    var ex = curentRow.getElementsByTagName("td")[2].innerHTML;
    var n = curentRow.getElementsByTagName("td")[0].innerHTML;
    var ext = "not set";
    if(ex!=""){
        ext = ex;
    }
    var args = new Array("", ext, true, currentRowId);
    
    
	if(parsed[args[3]-1].fps == ""){
		parsed[args[3]-1].fps = "25";
	}
	if(parsed[args[3]-1].bitrate == ""){
		parsed[args[3]-1].bitrate = "5000"
	}
	
    document.getElementById("framerate").value = parsed[args[3]-1].fps;
	document.getElementById("bitrate").value = parsed[args[3]-1].bitrate;

    setDropdownValue(args);  
}
// events functions for text fields and compo list checkbox

function dropdown(ar) { 
/*//console.log("dd clicked")*/ }

function frameRangeChanged(ar){
    curentRow.getElementsByTagName("td")[1].innerHTML = ar.value;
    parsed[currentRowId-1].frame_range = ar.value;
}
function framerateChanged(ar){
    parsed[currentRowId-1].fps = ar.value;
}
function bitrateChanged(ar){
    parsed[currentRowId-1].bitrate = ar.value;
}
function renderableChecked(ar){
	var the_row = ar.getAttribute("data-el");
	var rend_checked = 0;
	if(ar.checked == true)
		rend_checked = 1;
	parsed[the_row-1].renderable = rend_checked;
}
function ignoreMissingsChecked(ar){
    var miss_checked_val = 0;
    if(ar.checked == true) {
        miss_checked_val = 1;
    }
	data_struct.ignore_missings = miss_checked_val;
}
function smartCollectChecked(ar){
    var smart_checked_val = 0;
    if(ar.checked == true) {
        smart_checked_val = 1;
    }
    data_struct.smart_collect = smart_checked_val;
}

// filtering for queued compositions before submit
function setObjectSubmit(){
    var reparsed = new Array();
    
    for(var x=0; x< parsed.length; x++){
        if(parsed[x].renderable == 1){
            reparsed.push(parsed[x]);
        }
    }
    return reparsed
}

// submit function executed by SEND button
function submit(ar){
	data_struct.data = setObjectSubmit();
	var csInterface = new CSInterface();
	if(data_struct.data.length > 0) {
		window.close();
		csInterface.evalScript('submit(' + JSON.stringify( data_struct ) + ')', function(returned){});
	}
	else {
		alert("RenderBeamer: Nothing selected for render. Try again.");
	}
}

//add events 
function addEvents(){
// add event listener to dropdowns
    var dropdownItems = document.getElementsByClassName("item");
    var i = 0;
    for (i = 0; i < dropdownItems.length; i++) {
        dropdownItems[i].addEventListener('click', function(){	var ar = this; var args = new Array(ar,"",false, currentRowId); 
																setDropdownValue(args);}, false); }
   
// add event listener to frame range input
    var frameRange = document.getElementById("frameRange");
    frameRange.addEventListener('input', function() { var ar = this; frameRangeChanged(ar); }, false);

// add event listener to framerate input
    var framerate = document.getElementById("framerate");
    framerate.addEventListener('input', function(){ var ar = this; framerateChanged(ar); }, false);

// add event listener to bitrate input
    var bitrate = document.getElementById("bitrate");
    bitrate.addEventListener('input', function(){ var ar = this; bitrateChanged(ar); }, false);
    
//  add event listener to send button
    var sendButton = document.getElementById("sendButton");
    sendButton.addEventListener('click', function(){ var ar = this; submit(ar); }, false);
    
// add event listener to smart collect checkbox
    var smartCollect = document.getElementById("smartCollect");
    smartCollect.addEventListener('click', function(){ var ar = this; smartCollectChecked(ar); }, false);
    
// add event listener to ignore missing assets checkbox
    var ignoreMissings = document.getElementById("ignoreMissings");
    ignoreMissings.addEventListener('click', function(){ var ar = this; ignoreMissingsChecked(ar); }, false);
    
// add event listener for opening dropdowns
    var openDrop = document.getElementsByClassName("dropdown");
    for (i = 0; i < openDrop.length; i++) {
        openDrop[i].addEventListener('click', function() { var ar = this; dropdown(ar); } , false);
    }    
}

// execute function with the same name in hostscript.jsx
function initx() {    
    /*themeManager.init();*/
    var csInterface = new CSInterface();
    document.getElementById("settingsBtn").classList.toggle("button-selected");
    document.getElementById("videoLabel").classList.add("hide");
    document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);

    csInterface.evalScript('initX()', function(renderqueue_list)
	{
        var table = document.getElementById("compositionList");
        data_struct = JSON.parse(renderqueue_list);
        parsed = data_struct.data;
        console.log(renderqueue_list);
		
        for(var x=0; x < parsed.length; x++)
		{
            var row = table.insertRow(x+1);
            var cell1 = row.insertCell(0);
            var cell2 = row.insertCell(1);
            var cell3 = row.insertCell(2);
            var cell4 = row.insertCell(3);

            // Add some text to the new cells:
            cell1.innerHTML = parsed[x].name;
            cell2.innerHTML = parsed[x].frame_range;
            if(parsed[x].ext == ""){
               parsed[x].ext = "not set";
            }

            cell3.innerHTML = parsed[x].ext;
			var is_cell_checked = '" >'
			if(parsed[x].renderable == 1) {
                is_cell_checked = '" checked >';
            }
			cell4.innerHTML = '<input type="checkbox" class="checkbox" name="scales" data-el="' + (x+1).toString() + is_cell_checked;       
        }
        // set first row as selected 
        
		currentRowId = 1;
		var rows = table.getElementsByTagName("tr");
        console.log(rows[currentRowId]);
        compoListClicked(rows[currentRowId]);
        
        for (i = 1; i < rows.length; i++) 
		{
            rows[i].addEventListener('click', function(){ var ar = this; compoListClicked(ar);}, false);
            rows[i].getElementsByTagName("input")[0].addEventListener('click', function(){ var ar = this; renderableChecked(ar);}, false);
        }
    });    
    addEvents();
}