function getUserDir()
{
    if (system.osName == "MacOS")
    {
        home = '/Users/'+system.userName;
     }
    else
    {
        home = 'C:\\Users\\'+system.userName;
    };
    return home
};

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

function trim (str) {  
  return str.replace(/(^[\s\n\r\t\x0B]+)|([\s\n\r\t\x0B]+$)/g, '');  
}  

function getCPU(){
    var cpu = "unknown";
    if(system.osName == "MacOS")
    {
        cpu = system.callSystem("/usr/sbin/sysctl -n machdep.cpu.brand_string");
        //cpu = cpu.split("\n")[1];
        cpu = trim(cpu);
        
    }
    else
    {
        cpu = system.callSystem("wmic cpu get name");
        
        cpu = cpu.split("\n")[1];
        cpu = trim(cpu);
    }
    return cpu;
    
   }

function main()
{
    var cpu = getCPU();
    
    var ccFile = NewFileCreator(getUserDir()+ "/" + 'renderbeamer/cc');
    if (ccFile.exists == true)
    {
        url = "https://copernicuscomputing.com/pl/estimate/";
    }
    else
    {
        url = "https://garagefarm.net/cost-calculator/"
    }

    url =  url + "?cpu=" + cpu + "&frames=1";
    url = url.replace(/ /g, "%20");
    writeLn(url)
    

    if(system.osName == "MacOS")
    {
        var POST = "open \""+ url +"\""
        system.callSystem(POST);
    }
    else
    {

        var POST = "cmd.exe /c start  "+url
        writeLn(POST);
        system.callSystem(POST);
    }
}

main()



