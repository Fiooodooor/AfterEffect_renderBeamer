function renderBeamerQueueItemD()
{
    this.name = "Composition_Name";
    this.frame_range = "1to10s1";
	this.fps = "29.97";
	this.out_is_sequence = "1";
    this.file_ext_format = "png 16bit";
	this.file_ext = "png";
    this.renderable = "0";
    this.audio_available_in_comp = "0";
    this.audio_out_enabled = "0";
    this.audio_depth = "4";
    this.audio_channels = "2";
    this.audio_sample_rate = "48000";
    this.video_encoder = "";
    this.video_pixel_format = "";
    this.video_profile = "";
    this.video_bitrate = "5000";
    this.composition_id = "0"; 
    this.rq_id = "0";
    this.rq_out_id = "0";
	this.width = "0";
	this.height = "0";
}

function renderBeamerQueueSettingsD() 
{
	this.ignore_missings = "1";
	this.smart_collect = "0";
}

function getObjectLength(object_to_check) 
{
    var length = 0;
    for(var key in object_to_check) 
	{
        ++length;
    }
    return length;
}

function getStringifiedOutput(renderbeamerQueueArray, renderbeamerSettingsArray)
{
    var data = renderbeamerQueueArray;
	var settings = renderbeamerSettingsArray;

	var rbJSON = "{";
	for(var s_item in settings)
	{
		rbJSON = rbJSON.concat("\"", s_item.toString(), "\":\"", settings[s_item], "\",");
	}
	rbJSON = rbJSON.concat("\"data\":[");
    for(var x=0; x < data.length; x++)
	{
		rbJSON = rbJSON.concat("{");
		var data_len = getObjectLength(data[x]);
        for(var data_key in data[x])
		{
			rbJSON = rbJSON.concat("\"", data_key.toString(), "\":\"", data[x][data_key], "\"");
			if(--data_len > 0) {
				rbJSON = rbJSON.concat(",");
			} 				
        }
		rbJSON = rbJSON.concat("}");
		if(x < data.length-1) {
			rbJSON = rbJSON.concat(",");
		}
    }
	rbJSON = rbJSON.concat("]}");
    return rbJSON;
}

function checkIndexOf(stringsArrayToSearch, searchString)
{
    var arr = stringsArrayToSearch;
    var ss = searchString;
    for(var i=0; i < arr.length; i++)
    {
        if(arr[i] == ss)
            return i;        
    }
    return -1;
}

function getQueueItemsList()
{
    var rqi = new Array();
    for (var i = 1; i <= app.project.renderQueue.numItems; i++){ 
        rqi.push(app.project.renderQueue.items[i]);
    }
    return rqi;
}

function initRenderbeamerHostCollect()
{
    var renderQueueItems = getQueueItemsList();
    var renderBeamerQueueItemsList = new Array();
    var renderBeamerQueueSettings = new renderBeamerQueueSettingsD();
    
	var rq;
	var rq_settings;
	var rq_out;
	var rq_out_settings;
	var rq_out_nr = 1;
	
	for (var i = 0; i < renderQueueItems.length; i++)
	{
		var c = new renderBeamerQueueItemD();
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
		
		if(rq.comp.hasAudio == true)
			c.audio_available_in_comp  = 1;
		if(rq_out_settings["Output Audio"] == 3)
			c.audio_out_enabled = c.audio_available_in_comp;
		else
			c.audio_out_enabled = rq_out_settings["Output Audio"] - 1;
            
		c.audio_channels = 2;
        
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
		
		var temp_ext = rq_out_settings["Output File Info"]["File Name"].split(".");      
		c.file_ext = temp_ext[temp_ext.length-1];
		c.file_ext_format = c.file_ext;	
		
		renderBeamerQueueItemsList.push(c);	
	}
    return getStringifiedOutput(renderBeamerQueueItemsList, renderBeamerQueueSettings);
}

function initRenderbeamerHostRelinker(renderqueue_list)
{
	var videoExt = 	["mp4","mkv","mov","webm","mxf"];
	var videoExtF = ["MP4 MPEG-4 Part 14 (.mp4)", "Matroska (.mkv)", "MOV QuickTime (.mov)", "WebM (.webm)", "Material Exchange Format (.mxf)"];
    var parsed = renderqueue_list;
	var rbid = app.findMenuCommandId("renderBeamer_ui_function");
    var sectionName = 'renderBeamer';
	var indexOf = -1;
	app.preferences.savePrefAsString(sectionName , "rq_items", parsed.data.length);
	app.preferences.savePrefAsString(sectionName , "ignore_missings", parsed.ignore_missings);
	app.preferences.savePrefAsString(sectionName , "smart_collect", parsed.smart_collect);
	
    for(var x=0; x < parsed.data.length; x++)
	{
        for(var key in parsed.data[x])
		{
			if(key.toString() == "file_ext_format" && (indexOf=checkIndexOf(videoExt, parsed.data[x][key])) != -1) {
				app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), videoExtF[indexOf]);   				
			}
			else {
				app.preferences.savePrefAsString(sectionName , key + "_" + x.toString(), parsed.data[x][key]);
			}   				
        }
    }	
    app.preferences.saveToDisk();
    app.executeCommand(rbid);
    return "success";
}