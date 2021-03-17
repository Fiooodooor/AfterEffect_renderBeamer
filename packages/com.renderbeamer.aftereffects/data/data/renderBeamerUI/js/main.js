/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50 */
/*global $, window, location, CSInterface, SystemPath, themeManager*/

(function () {
    'use strict';
    var csInterface = new CSInterface();

    function init() 
	{    
        /*themeManager.init();*/

        document.getElementById("settingsBtn").classList.toggle("button-selected");
        
        document.getElementById("videoLabel").classList.add("hide");
        document.styleSheets[0].insertRule("div#videoContainer { display: none;}", 0);
        
    
        csInterface.evalScript('initX()', function(jj)
		{
            var cc = jj;
            var table = document.getElementById("compositionList");
            var parsed = JSON.parse(cc);
            //alert(parsed[0].name);
            for(var x=0; x < parsed.length; x++)
			{                
                var row = table.insertRow(x+1);

                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);

                // Add some text to the new cells:
                cell1.innerHTML = parsed[x].name;
                cell2.innerHTML = parsed[x].frame_range;
                cell3.innerHTML = parsed[x].ext;
                if(parsed[x].renderable == true){
                    cell4.innerHTML = '<input type="checkbox" class="checkbox" name="scales" checked>';
                   }
                else{
                    cell4.innerHTML = '<input type="checkbox" class="checkbox" name="scales">';
                }                
			}
        });       
    }    
    init();    
	
})();
    
