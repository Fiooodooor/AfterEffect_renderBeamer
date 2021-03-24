/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, Folder*/

function compositionX()
{
    this.name = "";
    this.frame_range = "";
	this.fps = 25;
    this.ext  = "";
    this.renderable = 0;
    this.comp_has_audio = 0;
    this.out_audio_enabled = 0;
    this.audio_depth = 2;
    this.audio_channels = 2;
    this.audio_sample_rate = 48000;
    this.encoder = "";
    this.pixel_format = "";
    this.profile = "";
    this.framerate = "";
    this.bitrate = "";
    this.composition_id = ""; 
    this.rq_id = "";
    this.rq_out_id = "";
	this.width = 0;
	this.height = 0;
	this.ignore_missings = 1;
	this.smart_collect = 0;
}

function readRenderQueueItems(){
    var rqi = new Array();
    for (var i = 1; i <= app.project.renderQueue.numItems; i++){ 
        rqi.push(app.project.renderQueue.item(i));
    }
    return rqi;
}

function submit(objS){
    var parsed = objS;
    var sectionName = 'renderBeamer';
	app.preferences.savePrefAsString(sectionName , "rq_items", parsed.length); 
    for(var x=0; x < parsed.length; x++){
        for(var key in parsed[x]){
            app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), parsed[x][key]);             
        }
    }
    app.preferences.saveToDisk();
    
    var rbid = app.findMenuCommandId("renderBeamer_ui_function");
    app.executeCommand(rbid);
    return "success"
}

function initX(){
    var renderQueueItems = readRenderQueueItems();
    var compositions = new Array();
    
	var rq;
	var rq_settings;
	var rq_out;
	var rq_out_settings;
	var rq_out_nr = 1;
	
	for (var i = 0; i < renderQueueItems.length; i++)
	{
		var c = new compositionX();
		if(renderQueueItems[i].status == RQItemStatus.QUEUED ) {
			c.renderable = 1;
		}
		rq =  renderQueueItems[i];
		rq_settings = rq.getSettings(GetSettingsFormat.STRING);
		rq_out = rq.outputModule(rq_out_nr)        
		rq_out_settings = rq_out.getSettings(GetSettingsFormat.NUMBER );			
		
		c.name = rq.comp.name;
		c.fps = rq_settings[rq_settings["Frame Rate"]];
		c.frame_range = (Math.round((rq.timeSpanStart)*c.fps)).toString()+"to"+(Math.round((rq.timeSpanStart+rq.timeSpanDuration)*c.fps)-1).toString()+"s1";
		
		c.width = rq.comp.width;
		c.height = rq.comp.height;				
		c.composition_id = rq.comp.id;
		c.rq_id = (i+1).toString();
		c.rq_out_id = (rq_out_nr).toString();
		
		if(rq.comp.hasAudio == true) {
			c.comp_has_audio  = 1;
		} else {
			c.comp_has_audio  = 0;
		}
		if(rq_out_settings["Output Audio"] == 3) {
			c.out_audio_enabled = c.comp_has_audio;
		}
		else {
			c.out_audio_enabled = rq_out_settings["Output Audio"] - 1;
		}
		c.audio_channels = rq_out_settings["Audio Channels"];
		if(rq_out_settings["Audio Sample Rate"] > 0)
			c.audio_sample_rate = rq_out_settings["Audio Sample Rate"];
		c.audio_depth = rq_out_settings["Audio Bit Depth"];
		c.ext = rq_out_settings["Output File Info"]["File Name"].split(".")[1];			
		compositions.push(c);	
	}
    return JSON.stringify(compositions);
}

function createArrayFromObject(obj){
    var newArray = new Array();
    for(var x=0; x < obj.length; x++){
        newArray.push(new Array(obj[x].name));
        newArray[x].push(obj[x].rq_id);
        newArray[x].push(obj[x].rq_out_id);
        newArray[x].push(obj[x].frame_range);
        newArray[x].push(obj[x].ext);
        newArray[x].push(obj[x].width);
        newArray[x].push(obj[x].height);
        newArray[x].push(obj[x].renderable);
        newArray[x].push(obj[x].audio);
        newArray[x].push(obj[x].audio_sample_rate);
        newArray[x].push(obj[x].audio_channels);
        newArray[x].push(obj[x].audio_depth);
        newArray[x].push(obj[x].fps);
        newArray[x].push(obj[x].ignore_missings);
        newArray[x].push(obj[x].smart_collect);
    }
    return newArray;
}
   