data_struct = "";
parsed = "";
currentRowId = "";

function beamerExtensions() 
{	
	this.videoExt 		= ["mp4","mkv","mov","webm","mxf"];
	this.videoExtended 	= ["MP4 MPEG-4 Part 14 (.mp4)", "Matroska (.mkv)", "MOV QuickTime (.mov)", "WebM (.webm)", "Material Exchange Format (.mxf)"];
	this.imgSeqExt    	= ["DPX","IFF","EXR","PNG","PSD","SGI","TIFF"];
	this.audioExt 		= ["wav"];
}

		
function setPixelFormat(PFvalidIds) {
	var pixelFormatParent = document.getElementById("pixelFormat-value").parentElement.getElementsByClassName("item");
	
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
	var profileParent = document.getElementById("profile-value").parentElement.getElementsByClassName("item");
	
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
function setPixelAndProfileValues(PFvalidIds, PvalidIds, AddHide, dropdownName)
{
	var pixelFormatValue = document.getElementById("pixelFormat-value");
	var profileValue = document.getElementById("profile-value");
	
	if(profileValue.parentElement.parentElement.classList.contains("hide")){
		profileValue.parentElement.parentElement.classList.remove("hide");
	}
	if(AddHide == true) {
		profileValue.parentElement.parentElement.classList.add("hide");
	}
    if(dropdownName=="encoder"){
		pixelFormatValue.innerHTML = pixelFormatParent[PFvalidIds[0]].innerHTML;
		if(AddHide == false) {
			profileValue.innerHTML = profileParent[PvalidIds[0]].innerHTML;
		}
	}
}
function setVideoEncoder(PFvalidIds, PvalidIds, AddHide, dropdownName) {
	setPixelFormat(PFvalidIds);
	if(AddHide == false) {
		setProfileParent(PvalidIds);
	}	
	setPixelAndProfileValues(PFvalidIds, PvalidIds, AddHide, dropdownName);
}

// setting up video settings dropdowns elements based on selected encoder 
function setVideoSettings(args){
    var encoderValue = document.getElementById("encoder-value");
	var dropdownName = "";
    if(args[2] == true){
        if(parsed[args[3]-1].video_encoder != ""){
            encoderValue.innerHTML = parsed[args[3]-1].video_encoder;
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
		setVideoEncoder(PFvalidIds, PvalidIds, AddHide, dropdownName);
	}
}

function checkRenderExtension(extension_to_check) {
	return checkRenderExtensionLcase( extension_to_check.toLowerCase() ); 
}

function checkRenderExtensionLcase(lcase_extension_to_check)
{
	const renderExtensions = new beamerExtensions();
	if(renderExtensions.videoExt.includes( lcase_extension_to_check )) 
		return "video";
	else if(renderExtensions.audioExt.includes( lcase_extension_to_check ))
		return "audio";		
	
	return "image sequence";	
}

//callback function for dropdown item event listener 
function setDropdownValue(args) 
{
    var compoListCalling = args[2]; //executed from composition list  manual <=> compoListCalling
	var rowId = args[3]-1;
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
    
	if(compoListCalling==false) {
		var ar = args[0];	// eventListner=>clicked=> 'item' <==> this
		attribute = ar.getAttribute("data-el");
		if(attribute == "") {
			return;
		}
		dropdown = ar.parentElement.parentElement;
        if(ar.parentElement.parentElement.classList.contains("top-item")){	// Extension dropdown was clicked
            dropdown = dropdown.parentElement.parentElement;				// Then 2 more parents to go
        }
		dropdown.classList.remove("dropdownClicked");
        dropdown.getElementsByClassName("dropdown-value")[0].innerHTML = attribute;
		topLabel = checkRenderExtension(extensionValue.innerHTML.split(" ")[0]);
        //topLabel = ar.parentElement.parentElement.getElementsByClassName("item")[0].innerHTML;
        
        // set object pproperties
        var dropName  = dropdown.parentElement.getElementsByClassName("label")[0].innerHTML;        
        if(dropName == "encoder"){
           parsed[rowId].video_encoder = attribute;
        }
        if(dropName == "pixel format"){
           parsed[rowId].video_pixel_format = attribute;
        }
        if(dropName == "profile"){
           parsed[rowId].video_profile = attribute;
        }
        if(dropName == "sample rate"){			
            parsed[rowId].audio_sample_rate = attribute.slice(0, -3) + "000";
        }
        if(dropName == "bit depth"){  
			parsed[rowId].audio_depth = parseInt(attribute.slice(0, -3))/8;
        }
        if(dropName == "channels"){   
			parsed[rowId].audio_channels = attribute;		
			if(attribute == 1) {
				channelsValue.innerHTML = "mono";
			}
			else {
				channelsValue.innerHTML = "stereo";
			}
        }
        if(dropName == "extension"){
			parsed[rowId].file_ext = "";
			parsed[rowId].video_encoder =  "";
            parsed[rowId].video_pixel_format = "";
            parsed[rowId].video_profile = "";
			if(topLabel == "audio") {
				parsed[rowId].audio_sample_rate = "";
				parsed[rowId].audio_depth = "";
			}
            parsed[rowId].file_ext_format = attribute;
        }
	}
	else {	 // compoListCalling == true 
		attribute = args[1];  //	args[1]  ==>  parsed[rowId].file_ext_format
		if(attribute == "") {
			return;
		}	
		dropdown = document.getElementById("extensionValue");
		dropdown.classList.remove("dropdownClicked");	
				
		topLabel = checkRenderExtension(attribute.split(" ")[0]);

		sampleRateValue.innerHTML = parsed[rowId].audio_sample_rate.slice(0, -3) + "kHz";
		bitDepthValue.innerHTML = (parsed[rowId].audio_depth*8).toString() + "bit";
		
		if(parsed[rowId].audio_channels == ""){
			parsed[rowId].audio_channels = 2;
		}
		if(parsed[rowId].audio_channels == 1) {
			channelsValue.innerHTML = "mono";
		}
		else {
			channelsValue.innerHTML = "stereo";
		}
		extensionValue.innerHTML = parsed[rowId].file_ext_format;
	}
	if(topLabel=="video") {
            // show video settings container
		if(document.getElementById("videoLabel").classList.contains("hide")) {
		    document.getElementById("videoLine").classList.remove("hide");
		    document.getElementById("videoBreak").classList.remove("hide");
			document.getElementById("videoLabel").classList.remove("hide");
			document.styleSheets[0].deleteRule(0);    
		}
		var x=0;	
		if(attribute.toLowerCase()=="mp4") {
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
		if(attribute.toLowerCase()=="mkv"){
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
		if(attribute.toLowerCase()=="mov"){
			encoderValue.innerHTML = "H265/HEVC";
			for(x=0;x<encoderParent.length;x++){
				encoderParent[x].classList.remove("hide");  
			}
			
			pixelFormatValue.innerHTML = "yuv420p";
			profileValue.innerHTML = "main";
			profileValue.parentElement.parentElement.classList.remove("hide");
		}
		if(attribute.toLowerCase()=="webm"){
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
		if(attribute.toLowerCase()=="mxf"){
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
		if(parsed[rowId].video_encoder == "" ){
			parsed[rowId].video_encoder = encoderValue.innerHTML;
		}
		if(parsed[rowId].video_pixel_format == "" ){
			parsed[rowId].video_pixel_format = pixelFormatValue.innerHTML;
		}
		if(parsed[rowId].video_profile == ""){
			parsed[rowId].video_profile = profileValue.innerHTML;
		}
		if(parsed[rowId].fps == ""){ 
			parsed[rowId].fps = framerate.value;
		}			
		if(parsed[rowId].video_bitrate == "") {
			parsed[rowId].video_bitrate = bitrate.value; 
		}
		parsed[rowId].file_ext = attribute;
		parsed[rowId].out_is_sequence = 0;
		
		encoderValue.innerHTML = parsed[rowId].video_encoder;
		pixelFormatValue.innerHTML = parsed[rowId].video_pixel_format;
		profileValue.innerHTML = parsed[rowId].video_profile;
		bitrate.value = parsed[rowId].video_bitrate;
		framerate.value = parsed[rowId].fps;
	}
	else if(topLabel=="image sequence") {
		// hide video settings container
		if(document.getElementById("videoLabel").classList.contains("hide")==false) {
			document.getElementById("videoLabel").classList.add("hide");
			document.getElementById("videoBreak").classList.add("hide");
			document.getElementById("videoLine").classList.add("hide");
			document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
		}
		parsed[rowId].file_ext = attribute.split(" ")[0];
		parsed[rowId].out_is_sequence = 1;
	}
    else if(topLabel=="audio") {             
		if(document.getElementById("videoLabel").classList.contains("hide")==false) {
			document.getElementById("videoLabel").classList.add("hide");
			document.getElementById("videoBreak").classList.add("hide");
			document.getElementById("videoLine").classList.add("hide");// if not hidden then hide video settings 
			document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
		}
		
		if(parsed[rowId].audio_sample_rate == "" && parsed[rowId].audio_depth == "") {
			parsed[rowId].audio_sample_rate = parsed[rowId].file_ext_format.split(" ")[1].slice(0, -3) + "000";
			parsed[rowId].audio_depth = parseInt(parsed[rowId].file_ext_format.split(" ")[2].slice(0, -3))/8;
			sampleRateValue.innerHTML = parsed[rowId].file_ext_format.split(" ")[1];			
			bitDepthValue.innerHTML = parsed[rowId].file_ext_format.split(" ")[2];
			parsed[rowId].file_ext = "WAV";
			parsed[rowId].out_is_sequence = 0;
		}
		parsed[rowId].file_ext_format = "WAV " +  sampleRateValue.innerHTML + " " + bitDepthValue.innerHTML;
		extensionValue.innerHTML = parsed[rowId].file_ext_format;
	}
	
	curentRow.getElementsByTagName("td")[2].innerHTML = parsed[rowId].file_ext_format;
	setVideoSettings(args);  
}

// callback function for composition list row event
function compoListClicked(ar){
    curentRow = ar; 
    var allRows = document.getElementById("compositionList").getElementsByTagName("TR");
		
    for(x=1; x<allRows.length; x++){
        allRows[x].style.background = "#ffffff";
        if(allRows[x].getElementsByTagName("input")[0].getAttribute("data-el")==curentRow.getElementsByTagName("input")[0].getAttribute("data-el")){
            currentRowId = x;
		    allRows[x].style.background = "#E6E8F0";

        }
    }
  
    document.getElementById("frameRange").value = parsed[currentRowId-1].frame_range;
    document.getElementById("extensionValue").innerHTML = parsed[currentRowId-1].file_ext_format;

	if(parsed[currentRowId-1].fps == ""){
		parsed[currentRowId-1].fps = "25";
	}
	if(parsed[currentRowId-1].video_bitrate == ""){
		parsed[currentRowId-1].video_bitrate = "5000"
	}
	
    document.getElementById("framerate").value = parsed[currentRowId-1].fps;
	document.getElementById("bitrate").value = parsed[currentRowId-1].video_bitrate;

	var args = new Array("", parsed[currentRowId-1].file_ext_format, true, currentRowId);   
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
    parsed[currentRowId-1].video_bitrate = ar.value;
}
function renderableChecked(ar){
	var the_row = ar.getAttribute("data-el");
	var rend_checked = 0;
	if(ar.checked == true)
		rend_checked = 1;
	parsed[the_row-1].renderable = rend_checked;
}
function scroll(ar){
    var scrollElement =  document.getElementById("scroll");

    if(document.body.scrollTop <150){
        scrollElement.classList.remove("hide");
    }
    if(document.body.scrollTop >150){
        scrollElement.classList.add("hide");
    }
}
function windowResize(){
    if(window.innerHeight>703){
        document.getElementById("scroll").classList.add("hide");
    }
    else{
        document.getElementById("scroll").classList.remove("hide");
    }
//    document.getElementById("scroll").innerHTML = window.innerHeight;
}
function setObjectSubmit(){
	var renderExtensions = new beamerExtensions();
    var reparsed = new Array();
    
    for(var x=0; x< parsed.length; x++){
        if(parsed[x].renderable == 1)
		{
			if(renderExtensions.videoExt.includes(parsed[x].file_ext_format.toLowerCase())) {
				var index_number = renderExtensions.videoExt.indexOf(parsed[x].file_ext_format.toLowerCase());
				parsed[x].file_ext_format = renderExtensions.videoExtended[index_number];		
			}
            reparsed.push(parsed[x]);			
        }
    }
    return reparsed;
}

function submitToPlugin(){
	var ret_data_struct = data_struct;	
	ret_data_struct.data = setObjectSubmit();
	if(ret_data_struct.data == undefined || ret_data_struct.data.length < 1) {
		return null;
	}
	return ret_data_struct;
}

function addEvents(){
    var dropdownItems = document.getElementsByClassName("item");
    var i = 0;
    for (i = 0; i < dropdownItems.length; i++) {
        dropdownItems[i].addEventListener('click', function() {	setDropdownValue([this,"",false, currentRowId]); }, false); 
	}
   
// add event listener to frame range input
    var frameRange = document.getElementById("frameRange");
    frameRange.addEventListener('input', function() { var ar = this; frameRangeChanged(ar); }, false);

// add event listener to framerate input
    var framerate = document.getElementById("framerate");
    framerate.addEventListener('input', function(){ var ar = this; framerateChanged(ar); }, false);

// add event listener to bitrate input
    var bitrate = document.getElementById("bitrate");
    bitrate.addEventListener('input', function(){ var ar = this; bitrateChanged(ar); }, false);
       
// add event listener to smart collect and ignore missing checkbox
    document.getElementById("smartCollect").addEventListener('click', function(){ data_struct.smart_collect = this.checked ? 1 : 0; }, false);
    document.getElementById("ignoreMissings").addEventListener('click', function(){ data_struct.ignore_missings = this.checked ? 1 : 0; }, false);
    
// add event listener for opening dropdowns
    var openDrop = document.getElementsByClassName("dropdown");
    for (i = 0; i < openDrop.length; i++) {
        openDrop[i].addEventListener('click', function() { var ar = this; dropdown(ar); } , false);
    }

    window.addEventListener('scroll', () => {
        document.body.style.setProperty('--scroll',window.pageYOffset / (document.body.offsetHeight - window.innerHeight));
    }, false);

    window.addEventListener('resize', windowResize());
}
// execute function with the same name in hostscript.jsx
function initRenderbeamerPanel( renderQueue_list ) 
{
	var renderExtensions = new beamerExtensions();
    document.getElementById("settingsBtn").classList.toggle("button-selected");
    document.getElementById("videoLabel").classList.add("hide");
    document.getElementById("videoBreak").classList.add("hide");
	document.getElementById("videoLine").classList.add("hide");
//	document.getElementById("scroll").classList.add("hide");
	console.log(document.styleSheets[0])
	document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
	/* const styleSheet = document.createElement('style');
	document.head.appendChild(styleSheet);
	styleSheet.textContent = "div#videoContainer { display: none;}" */

	var jsonResult = renderQueue_list;		
	var table = document.getElementById("compositionList");		
	data_struct = JSON.parse(jsonResult);	
	console.log(data_struct.data);
	parsed = data_struct.data;
	for(var x=0; x < parsed.length; x++)
	{
		if(parsed[x].file_ext == "") {
			parsed[x].file_ext = "PNG";
		}
		if(renderExtensions.videoExt.includes(parsed[x].file_ext.toLowerCase())) {
			parsed[x].file_ext_format = parsed[x].file_ext.toLowerCase();
			parsed[x].out_is_sequence = 0;
		}
		else if(renderExtensions.audioExt.includes(parsed[x].file_ext.toLowerCase())) {
			parsed[x].file_ext_format = "wav " + parsed[x].audio_sample_rate.slice(0, -3) + "kHz " + parseInt(parsed[x].audio_depth)*8 + "bit";
			parsed[x].out_is_sequence = 0;
		}
		else if(renderExtensions.imgSeqExt.includes(parsed[x].file_ext.toUpperCase())) {
			parsed[x].file_ext = parsed[x].file_ext.toUpperCase();
			parsed[x].file_ext_format = parsed[x].file_ext + " 16bit";
			parsed[x].out_is_sequence = 1;
		}
		else {
			parsed[x].file_ext = "PNG";
			parsed[x].file_ext_format = "PNG 16bit"
			parsed[x].out_is_sequence = 1;
		}
		var row = table.insertRow(x+1);
		var cell1 = row.insertCell(0);
		var cell2 = row.insertCell(1);
		var cell3 = row.insertCell(2);
		var cell4 = row.insertCell(3);

		// Add some text to the new cells:
		cell1.innerHTML = parsed[x].name;
		cell2.innerHTML = parsed[x].frame_range;
		cell3.innerHTML = parsed[x].file_ext_format;
		var is_cell_checked = '" >'
		if(parsed[x].renderable == 1) {
			is_cell_checked = '" checked >';
		}
		cell4.innerHTML = '<input type="checkbox" class="myinput" name="scales" data-el="' + (x+1).toString() + is_cell_checked;
		compoListClicked(row);
	}
	currentRowId = 1;
	var rows = table.getElementsByTagName("tr");
	console.log(rows[currentRowId]);
	compoListClicked(rows[currentRowId]);
	
	for (i = 1; i < rows.length; i++) 
	{
		rows[i].addEventListener('click', function(){ var ar = this; compoListClicked(ar);}, false);
		rows[i].getElementsByTagName("input")[0].addEventListener('click', function(){ var ar = this; renderableChecked(ar);}, false);
	}
    addEvents();
    // When the user scrolls the page, execute myFunction

}

// Add the sticky class to the header when you reach its scroll position. Remove "sticky" when you leave the scroll position
