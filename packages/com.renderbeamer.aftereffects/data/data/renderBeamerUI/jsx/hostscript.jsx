/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, Folder*/

function compositionX(name,frameRange,theFps,renderable)
{
    this.name = name;
    this.frame_range = frameRange;
	this.fps = theFps;
    this.ext  = "";
    this.renderable = renderable;
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


function submit(objS){
    var parsed = objS;
    var sectionName = 'renderBeamer';
	app.preferences.savePrefAsString(sectionName , "rq_items", parsed.length); 
    for(var x=0; x < parsed.length; x++){
        for(var key in parsed[x]){
            app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), parsed[x][key]); 
            //app.preferences.deletePref(sectionName, key+"_"+x.toString());
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
    
	var rq_fps= "";
	var rq_frame_range = "";
	var rq;
	var rq_settings;
	var rq_out;
	var rq_out_settings;
	var rq_out_nr = 1;
	
	for (var i = 0; i < renderQueueItems.length; i++)
	{        
		if(renderQueueItems[i].status == RQItemStatus.QUEUED )
		{ 
			rq =  renderQueueItems[i];
			rq_settings = rq.getSettings(GetSettingsFormat.STRING);
			rq_out = rq.outputModule(rq_out_nr)        
			rq_out_settings = rq_out.getSettings(GetSettingsFormat.NUMBER );
			rq_fps=rq_settings[rq_settings["Frame Rate"]];
			rq_frame_range = (Math.round((rq.timeSpanStart)*rq_fps)).toString()+"to"+(Math.round((rq.timeSpanStart+rq.timeSpanDuration)*rq_fps)-1).toString()+"s1";
			
			var c = new compositionX(rq.comp.name,rq_frame_range,rq_fps,1);			
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
			if(rq_out_settings["Output Audio"] == 3)
			{
				c.out_audio_enabled = c.comp_has_audio;
			}
			else
			{
				c.out_audio_enabled = rq_out_settings["Output Audio"] - 1;
			}
			c.audio_channels = rq_out_settings["Audio Channels"];
			if(rq_out_settings["Audio Sample Rate"] > 0)
				c.audio_sample_rate = rq_out_settings["Audio Sample Rate"];
			c.audio_depth = rq_out_settings["Audio Bit Depth"];
			c.ext = rq_out_settings["Output File Info"]["File Name"].split(".")[1];			
			compositions.push(c);
		}
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
    

function readCompositions(){
    var comps = new Array();
    for (var i = 1; i <= app.project.numItems; i++){ 
        if (app.project.item(i) instanceof CompItem) {
            comps.push(app.project.item(i));
        }
    }
    return comps;
}

function readRenderQueueItems(){
    var rqi = new Array();
    for (var i = 1; i <= app.project.renderQueue.numItems; i++){ 
        rqi.push(app.project.renderQueue.item(i));
    }
    return rqi;
}
    
function mainX(){
    var cc = initX();
    var x  = new Array();
    var s = new Array;
    for (var i = 1; i <= cc.length; i++){ 

        s.push(cc[i].name);
        s.push(cc[i].frame_range);
        s.push(cc[i].ext);
        s.push(cc[i].renderable);
        
        x.push(s.join(' '));
    }
    alert(x.join('\n'));
}