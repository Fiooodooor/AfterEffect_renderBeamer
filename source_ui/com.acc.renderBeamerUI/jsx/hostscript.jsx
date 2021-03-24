/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, Folder*/

function compositionX()
{
    this.name = "No name";
    this.frame_range = "1to10s1";
	this.fps = 29.97;
	this.is_sequence = 1;
    this.ext = "png";
    this.renderable = 0;
    this.comp_has_audio = 0;
    this.out_audio_enabled = 0;
    this.audio_depth = 2;
    this.audio_channels = 2;
    this.audio_sample_rate = 48000;
    this.encoder = "";
    this.pixel_format = "";
    this.profile = "";
    this.bitrate = "5000";
    this.composition_id = "0"; 
    this.rq_id = "0";
    this.rq_out_id = "0";
	this.width = 0;
	this.height = 0;
}

function readRenderQueueItems(){
    var rqi = new Array();
    for (var i = 1; i <= app.project.renderQueue.numItems; i++){ 
        rqi.push(app.project.renderQueue.item(i));
    }
    return rqi;
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
	
    return JSON.stringify({ ignore_missings: 1, smart_collect: 0, data: compositions });
}

function submit(renderqueue_list){
    var parsed = renderqueue_list;
	var rbid = app.findMenuCommandId("renderBeamer_ui_function");
    var sectionName = 'renderBeamer';
	app.preferences.savePrefAsString(sectionName , "rq_items", parsed.data.length);
	app.preferences.savePrefAsString(sectionName , "ignore_missings", parsed.ignore_missings);
	app.preferences.savePrefAsString(sectionName , "smart_collect", parsed.smart_collect);
	
    for(var x=0; x < parsed.data.length; x++){
        for(var key in parsed.data[x]){
            app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), parsed.data[x][key]);             
        }
    }
	
    app.preferences.saveToDisk();
    app.executeCommand(rbid);
    return "success"
}
   