/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, Folder*/
var 	  videoExt = new Array("mp4","mkv","mov","webm","mxf");
var videoExtFormat = new Array("MP4 MPEG-4 Part 14 (.mp4)", "Matroska (.mkv)", "MOV QuickTime (.mov)", "WebM (.webm)", "Material Exchange Format (.mxf)");
var imgSeqExt = new Array("DPX","IFF","EXR","PNG","PSD","SGI","TIFF");
var audioExt = new Array("wav");

function compositionX()
{
    this.name = "No name";
    this.frame_range = "1to10s1";
	this.fps = 29.97;
	this.out_is_sequence = 1;
    this.file_ext_format = "png 16bit";
	this.file_ext = "";
    this.renderable = 0;
    this.audio_available_in_comp = 0;
    this.audio_out_enabled = 0;
    this.audio_depth = 2;
    this.audio_channels = 2;
    this.audio_sample_rate = "48000";
    this.video_encoder = "";
    this.video_pixel_format = "";
    this.video_profile = "";
    this.video_bitrate = "5000";
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
	
	//var 	  videoExt = new Array("mp4","mkv","mov","webm","mxf");
	//var videoExtFormat = new Array("MP4 MPEG-4 Part 14 (.mp4)", "Matroska (.mkv)", "MOV QuickTime (.mov)", "WebM (.webm)", "Material Exchange Format (.mxf)");
	//var imgSeqExt = new Array("DPX","IFF","EXR","PNG","PSD","SGI","TIFF");
	//var audioExt = new Array("wav");
	
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
			c.audio_available_in_comp  = 1;
		}		
		if(rq_out_settings["Output Audio"] == 3) {
			c.audio_out_enabled = c.audio_available_in_comp;
		}
		else {
			c.audio_out_enabled = rq_out_settings["Output Audio"] - 1;
		}
		
		c.audio_channels = rq_out_settings["Audio Channels"];
		if(rq_out_settings["Audio Sample Rate"] <= 44100)
			c.audio_sample_rate = "44100";
		else if(rq_out_settings["Audio Sample Rate"] <= 48000)
			c.audio_sample_rate = "48000";
		else if(rq_out_settings["Audio Sample Rate"] <= 88200)
			c.audio_sample_rate = "88200";
		else
			c.audio_sample_rate = "96000";
		
		if(rq_out_settings["Audio Bit Depth"] <= 2)
			c.audio_depth = 2;
		else
			c.audio_depth = 4;
		
		var temp_ext = rq_out_settings["Output File Info"]["File Name"].split(".");      
		c.file_ext = temp_ext[temp_ext.length-1];
		
		if(videoExt.indexOf(c.file_ext.toLowerCase()) != -1)
		{
			c.file_ext_format = c.file_ext.toLowerCase();
			c.out_is_sequence = 0;
		}
		else if(imgSeqExt.indexOf(c.file_ext.toUpperCase()) != -1)
		{
			if("DPX" == c.file_ext.toUpperCase()) {
				c.file_ext_format = "DPX 10bit";
			}
			else {
				c.file_ext_format = c.file_ext.toUpperCase() + " 16bit";
			}
			c.out_is_sequence = 1;
		}
		else if(audioExt.indexOf(c.file_ext.toLowerCase()) != -1)
		{
			c.file_ext_format = audioExt[0] + " " + c.audio_sample_rate + " " + c.audio_depth;
			c.out_is_sequence = 0;
		}
		else
		{
			c.file_ext_format = videoExt[0];
			c.file_ext = videoExt[0];
			c.out_is_sequence = 0;
		}
		
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
	
    for(var x=0; x < parsed.data.length; x++)
	{
        for(var key in parsed.data[x])
		{
			if(key.toString() == "file_ext_format" && videoExt.indexOf(parsed.data[x][key]) != -1) {
				app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), videoExtFormat[videoExt.indexOf(parsed.data[x][key])]);   				
			}
			else {
				app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), parsed.data[x][key]);
			}   				
        }
    }
	
    app.preferences.saveToDisk();
    app.executeCommand(rbid);
    return "success"
}
   