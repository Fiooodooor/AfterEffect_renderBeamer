/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, Folder*/

function compositionX(name,frameRange,ext,renderable)
{
    this.name = name;
    this.frame_range = frameRange;
	this.fps = "29.97";
    this.ext  = ext;
    this.renderable = renderable;
    this.comp_has_audio = 0;
    this.out_audio_enabled = 0;
    this.audio_depth = 2;
    this.audio_channels = 2;
    this.audio_sample_rate = 48000;
    this.output_full_path = '';
    this.output_file_name = '';
    this.encoder = "";
    this.pixel_format = "";
    this.profile = "";
    this.framerate = "";
    this.bitrate = "";
    this.composition_id = ""; 
    this.rq_id = "";
    this.rq_out_id = "";
	this.width = "";
	this.height = "";
}


function submit(objS){
    var parsed = objS;
    var sectionName = 'renderBeamer';
	app.preferences.savePrefAsString(sectionName , "rq_items", parsed.length); 
    for(var x=0; x<parsed.length; x++){
        for(var key in parsed[x]){
            app.preferences.savePrefAsString(sectionName , key+"_"+x.toString(), parsed[x][key]); 
            //app.preferences.deletePref(sectionName, key+"_"+x.toString());
        }
    }
    app.preferences.saveToDisk();
    
    var rbid = app.findMenuCommandId("renderBeamer Batch Relinker");
   // app.executeCommand(rbid);
    return "success"
}

function initX(){
    var renderQueueItems = readRenderQueueItems();
    var compositions = new Array();
    
	var rqFps= "";
	var rqFrameRange = "";
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
			var c = new compositionX('','','',true);
			rqFps=rqMod[rqMod["Frame Rate"]];
			rqFrameRange = (Math.round((rq.timeSpanStart)*rqFps)).toString()+"to"+(Math.round((rq.timeSpanStart+rq.timeSpanDuration)*rqFps)-1).toString()+"s1";
			
			c.name = rq.comp.name;
			c.width = rq.comp.width;
			c.height = rq.comp.height;
			c.fps = rqFps;
			c.frame_range = rqFrameRange;
			c.composition_id = rq.comp.id;
			c.rq_id = (i+1).toString();
			c.rq_out_id = (rq_out_nr).toString();
			
			if(rq.comp.hasAudio == true) {
					c.comp_has_audio  = 1;
				} else {
					c.comp_has_audio  = 0;
			}
			if(rq_out_settings_num["Output Audio"] == 3)
			{
				c.out_audio_enabled = c.comp_has_audio;
			}
			else
			{
				c.out_audio_enabled = rq_out_settings["Output Audio"] - 1;
			}
			c.audio_channels = rq_out_settings["Audio Channels"];
			c.audio_sample_rate = rq_out_settings["Audio Sample Rate"];
			c.audio_depth = rq_out_settings["Audio Bit Depth"];
			c.output_full_path = rq_out_settings["Output File Info"]["Full Flat Path"];
			c.output_file_name = rq_out_settings["Output File Info"]["File Name"].split(".")[0];
			c.ext = rq_out_settings["Output File Info"]["File Name"].split(".")[1];			
			compositions.push(c);
		}
	}

    var na = createArrayFromObject(compositions);
    var jj = JSON.stringify(compositions);
    return jj;
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
        newArray[x].push(obj[x].output_file_name);
        newArray[x].push(obj[x].output_full_path);
        newArray[x].push(obj[x].fps);
        newArray[x].push(obj[x].width);
        newArray[x].push(obj[x].height);
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