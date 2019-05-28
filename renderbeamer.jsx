var PLUGIN_VERSION = "4";
doc = "";
logfile = "";
filesToReplace= {}
basepath2 = ""

// NewFile and New Folder- fix for OSX path
function NewFileCreator(in_path)

{


  var file;

  try

  {

    do

    {

     

      if (in_path instanceof File)

        var path = in_path.absoluteURI;

      else if (in_path instanceof Folder)

        var path = in_path.absoluteURI;

      else

        var path = in_path;

     

      file = new File(path)

     

      if (File.fs != "Macintosh")

        break;

     

      var filePath = file.fsName;

      if (filePath == path) // File path wasn't changed by Extendscript

        break;

     

      if (filePath.match(/^\/Volumes\//i) == null) // File path isn't a Volumes path

        break;

     

      var rootVolumeName = File("/").parent.displayName;

      filePath = filePath.replace(/^\/Volumes\//,"/" + rootVolumeName + "/")

     

      file = new File(filePath);

     

    }

    while (false);

  }

  catch (err)

  {

    file = null;

  }

 

  return file;

 

}

// ******************************

function NewFolderCreator(in_path)

{

  var folder;


  try

  {

    do

    {

      if (in_path instanceof File)

        var path = in_path.absoluteURI;

      else if (in_path instanceof Folder)

        var path = in_path.absoluteURI;

      else

        var path = in_path;

     

      folder = new Folder(path)

     

      if (File.fs != "Macintosh")

        break;

     

      var folderPath = folder.fsName;

      if (folderPath == path) // Folder path wasn't changed by Extendscript

        break;

     

      if (path.charAt(0) == "~")

        path = File("~").fsName + path.substr(1);

     

      if (path.charAt(0) != "/") // Not an absolute path

        break;

     

      var rootVolumeName = File("/").parent.displayName;

      folder = new Folder("/Volumes/" + rootVolumeName + path);

     

    }

    while (false);

  }

  catch (err)

  {

    folder = null;

  }

 

  return folder;

 

}

function getUserDir()
{
    if (system.osName == "MacOS")
    {
        home = '/Users/'+$.getenv("USER") ;
     }
    else
    {
        home = 'C:\\Users\\'+$.getenv("USERNAME");
    };
    return home
};

//---------------------------------terminal
function runScript (_PATH) {
    if (system.osName == "MacOS"){
        cmd = _PATH
    }else{
        
        cmd = "cmd.exe /c \"" + _PATH+"\"" ;
    }
    writeLn(cmd);
    return system.callSystem(cmd);
 }

function getTempdir()
{
    return runBeamer(["-tempdir"]);
}

function getUser()
{
        return runBeamer(["-getusr"]);
 }

function checkSceneName( scene, user)
{
        return runBeamer(['-checkName',scene, user, "-app", "AfterEffects"]);
 }
    
function addScene(localPath, farmPath)
{
    return runBeamer(["-app", "AfterEffects", "-a" , '"'+localPath+'"', "-sn" , '"'+farmPath+'"' ])
 }
function runBeamer( params)
{
            cmd = createCommand(params);
            cmd = cmd.join(" ")
            
            return runScript(cmd);

 }

function createCommand(params)
{
            beamerDir = getBeamerDir();
            jreexe = beamerDir + "/" + getJre();
            beamerjar = beamerDir + "/"+ "uploader.jar"
            jreexe= NewFileCreator(jreexe).fsName
            beamerjar= NewFileCreator(beamerjar).fsName
            cmda = ['"'+jreexe+'"',"-jar",'"'+beamerjar+'"'];
            cmda  = cmda.concat(params);
            return cmda;
};


function getJre()
{
    if (system.osName == "MacOS")
    {
        return "jre8/Contents/Home/bin/renderbeamer";
       
    }
    else
    {
        return "jre8\\bin\\renderbeamer.exe";
    };
    
};

function getBeamerDir()
{
    home = getUserDir();
    return home +"/"+ 'renderbeamer';
};


//--------------Timer 
function timer ()
{
    var date = new Date();
    var date = date.getTime()
    return date
 };

//-----------Adds zero to a number less than ten
function leadingZero (i)
{
    return (i < 10)? '0'+i : i;
 };

//---------------data logi
function datestring () 
{
    
    var date = new Date();
    
     return (leadingZero(date.getDate())+"/"+leadingZero(date.getMonth()+1)+"/"+date.getFullYear()+" "+ leadingZero(date.getHours())+"."+leadingZero(date.getMinutes())+"."+leadingZero(date.getSeconds())+" ");
    
};

//----------looking for not allowed characters in text and replace it with "_"
function replace (text) { 
    var allowed = "_-0123456789ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz";
    
    text.slice(0,-4)
    
    var nowy = "";
    var length = text.length;
    
    for (i = 0; i < text.length ; i++) {
     if (allowed.indexOf(text[i]) >= 0) {
         nowy = nowy + text[i];
         } else {
             nowy = nowy + "_";
             };
    };
    return (nowy);
    };
	
function replace2 (text) { 
    var allowed = "_-0123456789ABCDEFGHIJKLMNOPQRSTUWVXYZabcdefghijklmnopqrstuwvxyz";
    
    
    var nowy = "";
    var length = text.length;
    
    for (i = 0; i < text.length ; i++) {
     if (allowed.indexOf(text[i]) >= 0) {
         nowy = nowy + text[i];
         } else {
             nowy = nowy + "_";
             };
    };
    return (nowy);
    };

//------------printlog
function printlog (text){
     

        logfile.open("a");
        logfile.writeln(datestring () + text);
        logfile.close();
       
    
    };

function zeroPad(num, places) {
  var zero = places - num.toString().length + 1;
  return Array(+(zero > 0 && zero)).join("0") + num;
}




function get_project_dir(path){
    var dir = path.split("/") ;
    var dir = dir[(dir.length) -1];
    return replace(dir);
    
    };

function existsElement(arr, obj){
     for (k = 0; k <(arr.length) ; k++){
         if (obj == arr[k]){
             return true
             }
         
         }
     return false
    
    }


function getComp(){
    compList = [];
    lenRenderQueue =  app.project.renderQueue.numItems
    for (i = 1; i <=(app.project.renderQueue.numItems) ; i++) {

        comp = app.project.renderQueue.item(i).comp.name;
        
        if  (existsElement(compList, comp) == false){
            compList.push(comp);
            }
        
        
    }
    return compList
    }

function getRenderQueueItems(){
    renderQueueItems = [];
    existsComposite = [];
    
    
    for (var i = 1; i <=(app.project.renderQueue.numItems) ; i++) {
        var renderQueueItem = {};
        renderQueueItem['index'] = i.toString();
        oldName = app.project.renderQueue.item(i).comp.name;
        newName = replace2(app.project.renderQueue.item(i).comp.name);
        if (oldName != newName){
            newName = i.toString()+"_"+newName
            if (existsElement(existsComposite, newName) != true){
                existsComposite.push(newName);
                app.project.renderQueue.item(i).comp.name = newName

                
                }
            
            }
        renderQueueItem['nameComp'] = app.project.renderQueue.item(i).comp.name;
        var outputModule = app.project.renderQueue.item(i).outputModule(1).getSettings( GetSettingsFormat.STRING );
        var renderModule = app.project.renderQueue.item(i).getSettings( GetSettingsFormat.STRING );
        app.project.renderQueue.item(1).outputModule(1).getSettings( GetSettingsFormat.STRING );
        renderQueueItem['userOutput'] = outputModule["Output File Info"]["Base Path"];
        renderQueueItem['outFileExt'] = outputModule["Output File Info"]["File Name"].split('.').pop();
        renderQueueItem['height'] = app.project.renderQueue.item(1).comp.height;
        renderQueueItem['width'] = app.project.renderQueue.item(1).comp.width;
        
        if (renderModule["Frame Rate"] =="Use this frame rate"){
            renderQueueItem['frameRate'] = renderModule["Use this frame rate"]; 
        }else{
            renderQueueItem['frameRate'] = renderModule["Use comp's frame rate"];
        };
        renderQueueItem['startFrame'] = Math.round(renderModule["Time Span Start"] * renderQueueItem['frameRate'] )
        renderQueueItem['endFrame'] = Math.round(renderModule["Time Span End"] * renderQueueItem['frameRate'] )
        renderQueueItem['framestring'] = renderQueueItem['startFrame'].toString()+"to"+renderQueueItem['endFrame'].toString()+"s1"
        
        renderQueueItems.push(renderQueueItem);
    
    
    
    }

    return renderQueueItems;
    
    
}


function Asset( number) {
  this.number = number;
}

Asset.prototype.readPath =  function(){
    
    name = (app.project.item(this.number).file);
    name = name.toString(); 
    name = name.split("/"); 
    last = (name.length) -1; 
    this.fileName  = (name[last]); 
    this.seq = false
    
    this.originalPath = NewFileCreator(app.project.item(this.number).mainSource.file); 
    
    if (app.project.item(this.number).typeName == "Footage"){
        if (app.project.item(this.number).mainSource.isStill == false){
            
            this.seq = true

            }
        }
    
    }

Asset.prototype.exists= function(){
        if (this.originalPath.exists == true){

            return true;
            }
   
        return false;
        
 }

Asset.prototype.copyToSubmit= function(submitDir){
    
    if (this.seq == true){
        
        seqFileName = this.originalPath.fsName.split('\\').reverse()[0];
        orginarDirPath = this.originalPath.fsName.split('\\').slice(0,-1).join("\\");
        newDirPath = NewFolderCreator(submitDir).fsName
        
        filesToReplace[orginarDirPath ] = newDirPath
        
        seqName = seqFileName.split(".")[0]
        n = seqName.search(/\d+$/);
        firstString = seqName.substring(n, seqName.length);
        first = parseInt( firstString )
        pathToCopy = NewFileCreator( this.originalPath.fsName.replace(seqName, seqName.replace(/\d+$/,zeroPad(first, firstString.length ))));

        while(pathToCopy.exists==true){ 
            
            name = pathToCopy;
            name = name.toString(); 
            name = name.split("/"); 
            last = (name.length) -1; 
            fileNameSeq  = (name[last]); 
            
                newFileSeq = NewFileCreator(submitDir + "/" +fileNameSeq)
                printlog("copying " + (decodeURI( pathToCopy)) +" to " + (decodeURI( newFileSeq)));
                if (newFileSeq.exists == false){
                    pathToCopy.copy (newFileSeq);
                }
            
        first += 1;
        pathToCopy = NewFileCreator( this.originalPath.fsName.replace(seqName, seqName.replace(/\d+$/,zeroPad(first, firstString.length ))));

            }
        this.newFile = NewFileCreator(submitDir + "/" +this.fileName)
        
        return true;

        }


    this.newFile = NewFileCreator(submitDir + "/" +this.fileName)
    
    filesToReplace[this.originalPath.fsName] = this.newFile.fsName
    printlog("copying " + (decodeURI( this.originalPath)) +" to " + (decodeURI( this.newFile)));
    if (this.newFile.exists == false){
        this.originalPath.copy (this.newFile);
    }      
        return true;
        
 }

Asset.prototype.relink= function(){
    printlog("Relink " + (decodeURI( this.originalPath)) +" to " + (decodeURI( this.newFile)));
    
    if (this.seq == false){
    
        name = app.project.item(this.number).replace(this.newFile);
    }
    else
    {
        
        name = app.project.item(this.number).replaceWithSequence(this.newFile,false);
    };
        
 }


function AssetHandler()
{
        //this.sceneHandler = sceneHandler
        this.assetReadList = [];
        this.assetList = [];
        this.missingAssets =[];
        this.fonts = {};
        this.missingFonts = {};

    }

AssetHandler.prototype.readAssets = function() {  
    
    for (i = 1; i <=(app.project.numItems) ; i++) {
        if ((app.project.item(i).file) != undefined) {
            if  ((app.project.item(i).file) != null ) {
                
                assetObject = new Asset(i);
                assetObject.readPath();
                this.assetReadList.push(assetObject);
                

            }
        }
    
        // read fonts
    if ((app.project.item(i).constructor.name) == "CompItem") {
        

        layers = app.project.item(i).layers.length
        for (j = 1; j <=(layers); j++) {

            layer = app.project.item(i).layer(j);
            if (layer.constructor.name == "TextLayer"){
    
                fontFile = layer.property("Source Text").value.fontLocation 
                
                printlog("font name: "+layer.property("Source Text").value.font)
                printlog("font path: "+fontFile)

                fontFile = NewFileCreator(fontFile)
 
                if (fontFile.exists == true)
                {
                    this.fonts[layer.property("Source Text").value.font] = fontFile
                    printlog("found fonts " + (decodeURI(fontFile)));
                }
            else
            {
                     this.missingFonts[layer.property("Source Text").value.font] = layer.property("Source Text").value.font
                    printlog("Missing fonts " + layer.property("Source Text").value.font);
                
            }
              }
          
         }
     
     }
 }
          

    this.checkAssets();
    
    return  this.missingAssets.length == 0;
    
    

    
}




AssetHandler.prototype.checkAssets = function() {  
    
    for (i = 0; i <(this.assetReadList.length) ; i++) {
        if (this.assetReadList[i].exists()) {
            printlog("found asset "+ this.assetReadList[i].number+" path: " + decodeURI(this.assetReadList[i].originalPath))
            this.assetList.push(this.assetReadList[i]);
            }
        else
        {
            this.missingAssets.push(this.assetReadList[i]);
         }

      }
    
    
}

AssetHandler.prototype.copyAssets = function(submitDir) {  
    
    for (i = 0; i <(this.assetList.length) ; i++) {
        this.assetList[i].copyToSubmit(submitDir);
      }
      
}

AssetHandler.prototype.relinkAssets = function() {  
    
    tempScene = NewFileCreator(basepath2.concat("/", "temp.aepx"))
    printlog (tempScene.fsName)
    app.project.save(tempScene);
    f = File(tempScene);
    xmlString = f.read();  
    f.close()
    myXML = new XML(xmlString);  
    
    //app.project.save()
    
   // for (i = 0; i <(this.assetList.length) ; i++) {
        //this.assetList[i].relink();
      //}
      
}

AssetHandler.prototype.copyFonts = function(dirFonts) {  
    
    for(var key in this.fonts){
        var fontFile = this.fonts[key];
        printlog("copying fonts " + (decodeURI(fontFile)));
        var name = (fontFile );
        var name = name.toString(); 
        var name = name.split("/"); 
        var last = (name.length) -1; 
        var name = (name[last]);
        var newFile = NewFileCreator(dirFonts + "/" +name)
        if (newFile.exists == false)
        {
            fontFile.copy (newFile);
        }
    }
      
}

function main(){


    
    
    if (app.project.file == null){
        alert("Save project first.");
        return;
    };

    var customerName = getUser();
    writeLn(customerName);

    var scenePath = decodeURI(app.project.file.path);	
    var nameProject = decodeURI(app.project.file.name);


//var scene = get_scene(scenePath, customerName) ;

//writeLn(scene);
//------------------------------------

var myPath = app.project.file.path;
var orginalPath = app.project.file.path;

var orginal1 = orginalPath.concat("/",nameProject)

var nameProjectOryginal = decodeURI(app.project.file.name);

var nameProject = replace(nameProjectOryginal.slice(0,-4))+".aepx";

var name_folder = myPath.split("/") ;
var name_folder = name_folder[(name_folder.length) -1];
//--------------------------------------------------------------------------------------------------
var date = new Date();//pobieranie danych zwiazanych z czasem
var year = date.getFullYear();//wyodrebnianie z "data" roku
var month = leadingZero(date.getMonth()+1);//wyodrebnianie z "data" miesiaca
var day = leadingZero(date.getDate());//wyodrebnianie z "data" dnia
var hour = leadingZero(date.getHours());//wyodrebnianie z "data" godziny
var minute = leadingZero(date.getMinutes());//wyodrebnianie z "data" minuty
var second = leadingZero(date.getSeconds());//wyodrebnianie z "data" sekundy
//------------------------------------------------------------------------------------------------------
//------------------------cearte folder beamer_temp_data

var userProject = scenePath;
var response = getTempdir();

var response = NewFolderCreator (response);
if (response.exists && response != '[NULL]')
{
                userProject = response
 }
var basepath = "".concat(userProject , "/beamer_temp_", date.getFullYear(), leadingZero(date.getMonth()+1), leadingZero(date.getDate()), "-", leadingZero(date.getHours()), leadingZero(date.getMinutes()), leadingZero(date.getSeconds()), "/",get_project_dir(scenePath) );
var beamer = newFolder;
basepath2 = basepath.concat ("/data");
var newFolder = NewFolderCreator (basepath2);


//var scene = get_scene("".concat(get_project_dir(scenePath) ,"/data/", replace(nameProject)), customerName) ;
var scene = checkSceneName("".concat(get_project_dir(scenePath) ,"/data/", nameProject), customerName)
var sceneSubmit = "".concat(get_project_dir(scenePath) ,"/data/", scene);


var logFolderPath = getUserDir() +'/.renderbeamer/log/aftereffects'

var logFolder = NewFolderCreator (logFolderPath);
logFolder.create(); //tworzenie folderu

logfile = NewFileCreator(logFolderPath + '/pluginLog_afterEffects_'+nameProject +'_'+  date.getFullYear()+ leadingZero(date.getMonth()+1)+ leadingZero(date.getDate())+ "-"+ leadingZero(date.getHours())+ leadingZero(date.getMinutes())+ leadingZero(date.getSeconds())+'.txt');


printlog("PLUGIN_VERSION: " + PLUGIN_VERSION);
printlog("User:" + customerName);
printlog("After Effects version: "+app.version);
printlog("oryginal name: " + nameProjectOryginal);
printlog("replace name: " + nameProject);

if (app.project.renderQueue.numItems == 0)
{
     alert("No composition has not been added to Render Queue. To submit a scene, please add the composition and set the parameters for the render.");
    return true;
    
}

templength = basepath2+"/"+scene.slice(0,-5)+".gfs"
printlog("length path: " + templength.length)
if (templength.length >248)
{
    alert("Due to the Win OS file system limitations, the file path to your project is too long to operate on. Please move your project (including all the textures and assets) to a directory with a shorter path (less than 200 characters in total).\nAlso, we highly recommend not using long camera names in a scene as our plugin automatically adds it to the output path.");
    return true;
}

k = get_project_dir(scenePath).length
b = (nameProject.length )

lengthPath = 11+k +1 +5+ b*2+8+30
printlog ("Path length2 :"+ lengthPath )
if (lengthPath >248) 
{
    alert ("Due to the Win OS file system limitations, the file path to your output is too long for us to operate on. Please follow the guidelines below :\n- make your project and the scene name shorter." );
    return true;
 }

//---------create folder footage
var newFolder = "";

var newFolder = newFolder.concat(basepath2,"/relinkedAssets");
var footage = newFolder;
var newFolderC = NewFolderCreator (newFolder);


var fonts = {};
var dirFonts = ""
var dirFonts = dirFonts.concat(basepath2,"/fonts");
var dirFontsC = NewFolderCreator (dirFonts);

//------------------------copy object

var assetHandler = new AssetHandler();

if (assetHandler.readAssets() == false){
    missing = []
    for (i = 0; i <(assetHandler.missingAssets.length) ; i++) {
        missing.push(assetHandler.missingAssets[i].originalPath)
        
        
        }

    if (confirm("Missing assets:\n-"+missing.join([separator = '\n-'])+"\nContinue?") == false){
        return true;
        }
    
    }


    missing = []
    for(var key in assetHandler.missingFonts){
        missing.push(assetHandler.missingFonts[key]);
    }
    if (missing.length >0)
    {
    alert("Fonts:\n-"+missing.join([separator = '\n-'])+"\ncannot be copied with your project because of After Effects limitations. \nPlease contact with our support via. renderBeamer integrated chat to continue with the process.") ;
    }

    

//newFolder.create();
newFolderC.create(); //tworzenie folderu
dirFontsC.create(); //tworzenie folderu

var myPath2 =basepath2.concat("/", scene);//skladanie nazwy scierzki do nowego pliku
app.project.save(NewFileCreator(myPath2));



beamerDir = getBeamerDir();
copyAssetsExe = beamerDir + "/" + "AfterEffects/renderbeamer AfterEffects.exe";

if (system.osName == "MacOS")
{
    copyAssetsExe = beamerDir + "/" + "AfterEffects/renderbeamer AfterEffects.py";
 }

appExe= NewFileCreator(copyAssetsExe).fsName
beamerjar= NewFileCreator(beamerjar).fsName
cmda = ['"'+appExe+'"','"'+NewFileCreator(myPath2).fsName+'"'];


if (system.osName == "MacOS")
{
    cmda = ['python '+'"'+appExe+'"','"'+NewFileCreator(myPath2).fsName+'"'];
 }


cmda = cmda.join(" ")

printlog(cmda)


runScript(cmda)


//assetHandler.copyAssets(newFolder);
///assetHandler.relinkAssets();
assetHandler.copyFonts(dirFonts);


//---------create gfs
function printgfs (text){
     {
        encoding = "utf-8"
        doc.encoding = encoding; 
        doc.open("a");
        doc.writeln(text);
        doc.close();
       
    }
 };
renderQueueItems = getRenderQueueItems()
var AffterVersion = app.version;
var sceneName = scene.slice(0,-5)
doc= NewFileCreator(basepath2+"/"+scene.slice(0,-5)+".gfs");
//alert(beamer+"/log.gfs");
printgfs('<?xml version="1.0" encoding="utf-8"?>');
printgfs('<Scene App="AfterEffects" version="'+AffterVersion+'">');
printgfs('  <Settings outFileExt="' +renderQueueItems[0]['outFileExt'] + '" framestring="' +renderQueueItems[0]['framestring']+'" outFileName="'+sceneName+'_'+ '" outFilePath="U:\\'+customerName+'\\'+get_project_dir(scenePath)+"\\"+sceneName+'-Renders'+'" userOutput="'+renderQueueItems[0]['userOutput'] +'" >');
printgfs('    <AfterEffects height="'+renderQueueItems[0]['height']+'" width="'+renderQueueItems[0]['width']+'" fontDir="'+get_project_dir(scenePath)+ '/data/fonts">');


for (i = 0; i <(renderQueueItems.length) ; i++) {
    

    printgfs('      <RenderQueueItem index="'+renderQueueItems[i]["index"] +'" nameComp="'+ renderQueueItems[i]["nameComp"]  +'" outFileExt="' +renderQueueItems[i]['outFileExt'] + '" framestring="' +renderQueueItems[i]['framestring']+ '" height="'+renderQueueItems[i]['height']+'" width="'+renderQueueItems[i]['width']+'"/>');

    }

for(var key in assetHandler.fonts){
    var fontFile = assetHandler.fonts[key];
    var name = decodeURI(fontFile);

    var name = name.toString(); 
    var name = name.split("/"); 
    var last = (name.length) -1; 
    var name = (name[last]);
    printgfs('      <Font filename="'+ name +'"/>');


}

printgfs('    </AfterEffects>');
printgfs('  </Settings>');
printgfs('</Scene>');

//copy log
var logPath = "".concat(basepath2,"/log");
var newFolderC = NewFolderCreator (logPath);
newFolderC.create(); //tworzenie folderu
var nowyPlik = NewFileCreator(logfile); 
nowyPlik.copy (NewFileCreator(logPath+ '/pluginLog_afterEffects_'+nameProject +'_'+  date.getFullYear()+ leadingZero(date.getMonth()+1)+ leadingZero(date.getDate())+ "-"+ leadingZero(date.getHours())+ leadingZero(date.getMinutes())+ leadingZero(date.getSeconds())+'.txt'));


//--------------save scene
//var myPath2 =basepath2.concat("/", scene);//skladanie nazwy scierzki do nowego pliku
//app.project.save(NewFileCreator(myPath2));



//--------Send Scene  renderbeamer
basepathfile= NewFileCreator(basepath);

writeLn(basepathfile.fsName)
addScene(basepathfile.fsName, sceneSubmit);

//-------------open original scene
var project = NewFileCreator(orginal1);
printlog(orginal1);
try{
    app.project.close(CloseOptions.DO_NOT_SAVE_CHANGES);
    app.open(project);
}
catch(err) {
    printlog("error reload scene")
    writeLn("error reload scene")
}


 }

main()