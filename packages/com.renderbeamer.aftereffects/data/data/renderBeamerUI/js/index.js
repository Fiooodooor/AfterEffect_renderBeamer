/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, window, location, CSInterface, SystemPath, themeManager*/



parsed = "";
parsed2 = new Array(); 
currentRowId = "";



function readParsedComps(parsed,currentName){
    var c = new Array();
    for(var x=0; x < parsed.length; x++){
        if(parsed[x].name == currentName){
            //alert(x);
            for(key in parsed[x]){
                c.push(parsed[x][key]);
            }
           }
    }
    //alert(c);
}
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

// setting up video settings dropdowns elements based on selected encoder 
function setVideoSettings(args){
    var encoderValue = "";
    if(args[2] == true){
        if(parsed[args[3]-1].encoder != ""){
            encoderValue = parsed[args[3]-1].encoder;
        }
        else{
            encoderValue = document.getElementById("encoder-value");
        }
        var dropdownName = "";
    }
    else{
        encoderValue = document.getElementById("encoder-value");
        console.log(args[0].parentElement.parentElement.parentElement);
        var dropdownName = "";
        var dropParent = args[0].parentElement.parentElement.parentElement.getElementsByClassName("label");
        if(dropParent.length == 0){
           dropdownName = args[0].parentElement.parentElement.parentElement.parentElement.parentElement.getElementsByClassName("label")[0].innerHTML;
        }
        else{
            dropdownName = dropParent[0].innerHTML;
        }
    }
    
    var encoderParent = document.getElementById("encoder-value").parentElement.getElementsByClassName("item");

    var pixelFormatValue = document.getElementById("pixelFormat-value");
    var pixelFormatParent = pixelFormatValue.parentElement.getElementsByClassName("item");

    var profileValue = document.getElementById("profile-value");
    var profileParent = profileValue.parentElement.getElementsByClassName("item");
    
    var PFvalidIds = [];
    var PvalidIds =  [];
    var x = 0;
    
    
    if(encoderValue.innerHTML == "H265/HEVC"){
        for(x=0;x<pixelFormatParent.length;x++){
            // dropdown-content item id 
            //
            PFvalidIds = [0,3,5,10,11,12,13,14,15,16,17,18,20,21,22,23];
            if(PFvalidIds.includes(x)){
                if(pixelFormatParent[x].classList.contains("hide")){   
                    pixelFormatParent[x].classList.remove("hide");
                } 
            }
            else{
                if(pixelFormatParent[x].classList.contains("hide")==false){   
                        pixelFormatParent[x].classList.add("hide");
                }                     
            }

        }
        for(x=0;x<profileParent.length;x++){
            PvalidIds = [1,2,3,4,5,6,7,8,9,10,11,14,15,16];
            if(PvalidIds.includes(x)){
                if(profileParent[x].classList.contains("hide")){   
                    profileParent[x].classList.remove("hide");
                }
            }
            else{
                if(profileParent[x].classList.contains("hide")==false){   
                    profileParent[x].classList.add("hide");
                }  
            }
        }
        if(profileValue.parentElement.parentElement.classList.contains("hide")){
           profileValue.parentElement.parehide");
        }
        if(dropdownName=="encoder"){
            pixelFormatValue.innerHTML = pixelFormatParent[0].innerHTML;
            //profileValue.innerHTML = profileParent[0].innerHTML;
        }
    }
    if(encoderValue.innerHTML == "AOMedia Video 1(AV1)"){
        for(x=0;x<pixelFormatParent.length;x++){
            // dropdown-content item id 
            PFvalidIds = [0,3,5,11,12,13,16,17,18,20];
            if(PFvalidIds.includes(x)){
                if(pixelFormatParent[x].classList.contains("hide")){   
                    pixelFormatParent[x].classList.remove("hide");
                } 
            }
            else{
            if(pixelFormatParent[x].classList.contains("hide")==false){   
                    pixelFormatParent[x].classList.add("hide");
                }                     
            }

        }
        if(profileValue.parentElement.parentElement.classList.contains("hide")==false){
           profileValue.parentElement.parentElement.classList.add("hide");
        }
        if(dropdownName=="encoder"){
            pixelFormatValue.innerHTML = pixelFormatParent[0].innerHTML;
            //profileValue.innerHTML = profileParent[0].innerHTML;
        }
    }
    if(encoderValue.innerHTML == "ProRes"){
        for(x=0;x<pixelFormatParent.length;x++){
            // dropdown-content item id 
            PFvalidIds = [12,13,27];
            if(PFvalidIds.includes(x)){
                if(pixelFormatParent[x].classList.contains("hide")){   
                    pixelFormatParent[x].classList.remove("hide");
                } 
            }
            else{
            if(pixelFormatParent[x].classList.contains("hide")==false){   
                    pixelFormatParent[x].classList.add("hide");
                }                     
            }

        }
        for(x=0;x<profileParent.length;x++){
            PvalidIds = [27,28,29,30,31];
            if(PvalidIds.includes(x)){
                if(profileParent[x].classList.contains("hide")){   
                        profileParent[x].classList.remove("hide");
                }
            }
            else{
                if(profileParent[x].classList.contains("hide")==false){   
                    profileParent[x].classList.add("hide");
                }  
            }

        }
        if(profileValue.parentElement.parentElement.classList.contains("hide")){
           profileValue.parentElement.parentElement.classList.remove("hide");
        }
        if(dropdownName=="encoder"){
            pixelFormatValue.innerHTML = pixelFormatParent[12].innerHTML;
            profileValue.innerHTML = profileParent[27].innerHTML;
        }
    }
    if(encoderValue.innerHTML == "QuickTime Animation RLE"){
        for(x=0;x<pixelFormatParent.length;x++){
            // dropdown-content item id 
            PFvalidIds = [21,24,25,26];
            if(PFvalidIds.includes(x)){
                if(pixelFormatParent[x].classList.contains("hide")){   
                    pixelFormatParent[x].classList.remove("hide");
                } 
            }
            else{
                if(pixelFormatParent[x].classList.contains("hide")==false){   
                        pixelFormatParent[x].classList.add("hide");
                    }                     
            }

        }
        if(profileValue.parentElement.parentElement.classList.contains("hide")==false){
           profileValue.parentElement.parentElement.classList.add("hide");
        }
        if(dropdownName=="encoder"){
            pixelFormatValue.innerHTML = pixelFormatParent[24].innerHTML;
            //profileValue.innerHTML = profileParent[0].innerHTML;
        }
    }
    if(encoderValue.innerHTML == "HAP"){
        for(x=0;x<pixelFormatParent.length;x++){
            // dropdown-content item id 
            PFvalidIds = [12,13,27];
            if(PFvalidIds.includes(x)){
                if(pixelFormatParent[x].classList.contains("hide")){   
                    pixelFormatParent[x].claepth * 8).toString() + " bit";
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
        else{
            var videoExt = new Array("mp4","mkv","mov","webm","mxf");
            var imgSeqExt = new Array("DPX","IFF","EXR","PNG","PSD","SGI","TIFF");
            var audioExt = new Array("wav");
            
            if(videoExt.includes(attribute)){
                topLabel = "video";
            }
            else if(imgSeqExt.includes(attribute.split(" ")[0])){
                topLabel = "image sequence";
            }
            else if(audioExt.includes(attribute.split(" ")[0].toLowerCase())){
                topLabel = "audio";
            }
            
            if(parsed[args[3]-1].audio_sample_rate == ""){
				parsed[args[3]-1].audio_sample_rate = 48000;
			}
            sampleRateValue.innerHTML = (parsed[args[3]-1].audio_sample_rate).toString() + " kHz"; 
            if(parsed[args[3]-1].audio_depth == ""){
				parsed[args[3]-1].audio_depth = 2;
			}
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
        if(attribute == "not set"){
            
           // display smth ?
            
        }
        //alert(topLabel);
        
        if(topLabel=="video"){
            // show video settings container
            if(document.getElementById("videoLabel").classList.contains("hide")){
                document.getElementById("videoLabel").classList.remove("hide");
                document.styleSheets[0].deleteRule(0);    
            }
            
            if(manual==true){
               extensionValue.innerHTML = attribute;
            }
            var x=0;
            if(attribute=="mp4"){
                encoderValue.innerHTML = "H265/HEVC";
                for(x=0;x<encoderParent.length;x++){
                    if(encoderParent[x].classList.contains("hide")){   
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
                profileValue.parentElement.parentElement.classList.remove("       // set audio settings
            
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
        if(manual==false){
            curentRow.getElementsByTagName("td")[2].innerHTML = document.getElementById("extensionValue").innerHTML;
        }
        setVideoSettings(args);
    }
    
}

// callback function for composition list row event
function compoListClicked(ar){
    var row = ar;
    curentRow = row;
    var tab = document.getElementById("compositionList");
    var allRows = tab.getElementsByTagName("TR");
    var allRowsL = tab.getElementsByTagName("TR").length;
    for(var x =1;x<allRowsL;x++){
        //if(allRows[x].getElementsByTagName("td")[0].innerHTML==row.getElementsByTagName("td")[0].innerHTML){
        if(allRows[x].getElementsByTagName("input")[0].getAttribute("data-el")==row.getElementsByTagName("input")[0].getAttribute("data-el")){
           currentRowId = x;
           }
        allRows[x].style.background = "#2c2d30";
    }
    console.log(row);
    row.style.background = "#3c87d1";
    var frameRange = document.getElementById("frameRange");
    var extensionValue = document.getElementById("extensionValue");
    frameRange.value = row.getElementsByTagName("td")[1].innerHTML;
    extensionValue = row.getElementsByTagName("td")[2].innerHTML;
    
    
    var ex = row.getElementsByTagName("td")[2].innerHTML;
    var n = row.getElementsByTagName("td")[0].innerHTML;
    var ext = "";
    if(ex!=""){
        ext = ex;
    }
    else{
        ext = "not set";
    }
    var args = new Array("", ext, true, currentRowId);
    
    if(parsed[args[3]-1].framerate != ""){
        document.getElementById("framerate").value = parsed[args[3]-1].framerate;
    }
    else{
        document.getElementById("framerate").value = "25"
    }
    if(parsed[args[3]-1].bitrate != ""){
        document.getElementById("bitrate").value = parsed[args[3]-1].bitrate;
    }
    else{
        document.getElementById("bitrate").value = "5000"
    }
        
    setDropdownValue(args);  
}


// events functions for text fields and compo list checkbox

function dropdown(ar){ 
    //console.log("dd clicked")
    //ar.classList.add("dropdownClicked");
}

function frameRangeChanged(ar){
    curentRow.getElementsByTagName("td")[1].innerHTML = ar.value;
    parsed[currentRowId-1].frame_range = ar.value;
}
function framerateChanged(ar){
    parsed[currentRowId-1].framerate = ar.value;
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
	for(var x=0; x < parsed.length; x++){
		parsed[x].ignore_missings = miss_checked_val;
	}
}
function smartCollectChecked(ar){
	var smart_checked_val = 0;
	if(ar.checked == true) {
		smart_checked_val = 1;
	}
	for(var x=0; x < parsed.length; x++){
		parsed[x].smart_collect = smart_checked_val;
	}
}


// filtering for queued compositions before submit
function setObjectSubmit(){
    var reparsed = new Array();
    var ar = "";
    var lastSelected = currentRowId;
    
    for(var x=0;x< parsed.length;x++){
        if(parsed[x].renderable == 1){
            ar = nd return them back as JSON 
// + default setup for composition list table
function initx() {    
    /*themeManager.init();*/
    var csInterface = new CSInterface();
    document.getElementById("settingsBtn").classList.toggle("button-selected");

    document.getElementById("videoLabel").classList.add("hide");
    document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);


    csInterface.evalScript('initX()', function(jj){
        var cc = jj;
        var table = document.getElementById("compositionList");
        parsed = JSON.parse(cc);
        console.log(cc);
        var c =1;
        for(var x=0; x < parsed.length; x++){
            if(parsed[x].renderable == 1){
                var row = table.insertRow(c);
                c++;
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
                if(parsed[x].renderable == 1){
                    cell4.innerHTML = '<input type="checkbox" class="checkbox" name="scales" data-el="' + (x+1).toString() + '" checked>';
                   }
                else{
                    cell4.innerHTML = '<input type="checkbox" class="checkbox" name="scales data-el="' + (x+1).toString() + '">';
                }
                parsed2.push(parsed[x]);
            }

        }
        // set first row as selected 
        currentRowId = 1;
        curentRow = document.getElementById("compositionList").getElementsByTagName("tr")[currentRowId];
        var ar =  document.getElementById("compositionList").getElementsByTagName("tr")[1];
        console.log(ar);
        compoListClicked(ar);
        parsed = parsed2;
        
        // add event listener to composition list rows in <body> onload event due to dynamically added content 
        var table = document.getElementById("compositionList");
        var rows = table.getElementsByTagName("tr");
        var check_box = "";
        for (i = 1; i < rows.length; i++) {
            rows[i].addEventListener('click', function(){
                                                    var ar = this;
                                                    compoListClicked(ar);}
                                        , false);
            check_box = rows[i].getElementsByTagName("input")[0];
            check_box.addEventListener('click', function(){
                                                    var ar = this;
                                                    renderableChecked(ar);}
                                        , false);
        }
    });
    
    addEvents();
}