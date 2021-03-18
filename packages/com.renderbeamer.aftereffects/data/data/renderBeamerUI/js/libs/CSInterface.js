/**************************************************************************************************
*
* ADOBE SYSTEMS INCORPORATED
* Copyright 2013 Adobe Systems Incorporated
* All Rights Reserved.
*
* NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
* terms of the Adobe license agreement accompanying it.  If you have received this file from a
* source other than Adobe, then your use, modification, or distribution of it requires the prior
* written permission of Adobe.
*
**************************************************************************************************/

/** CSInterface - v8.0.0 */

/**
 * Stores constants for the window types supported by the CSXS infrastructure.
 */
function CSXSWindowType()
{
}

/** Constant for the CSXS window type Panel. */
CSXSWindowType._PANEL = "Panel";

/** Constant for the CSXS window type Modeless. */
CSXSWindowType._MODELESS = "Modeless";

/** Constant for the CSXS window type ModalDialog. */
CSXSWindowType._MODAL_DIALOG = "ModalDialog";

/** EvalScript error message */
EvalScript_ErrMessage = "EvalScript error.";

/**
 * @class Version
 * Defines a version number with major, minor, micro, and special
 * components. The major, minor and micro values are numeric; the special
 * value can be any string.
 *
 * @param major   The major version component, a positive integer up to nine digits long.
 * @param minor   The minor version component, a positive integer up to nine digits long.
 * @param micro   The micro version component, a positive integer up to nine digits long.
 * @param special The special version component, an arbitrary string.
 *
 * @return A new \c Version object.
 */
function Version(major, minor, micro, special)
{
    this.major = major;
    this.minor = minor;
    this.micro = micro;
    this.special = special;
}

/**
 * The maximum value allowed for a numeric version component.
 * This reflects the maximum value allowed in PlugPlug and the manifest schema.
 */
Version.MAX_NUM = 999999999;

/**
 * @class VersionBound
 * Defines a boundary for a version range, which associates a \c Version object
 * with a flag for whether it is an inclusive or exclusive boundary.
 *
 * @param version   The \c #Version object.
 * @param inclusive True if this boundary is inclusive, false if it is exclusive.
 *
 * @return A new \c VersionBound object.
 */
function VersionBound(version, inclusive)
{
    this.version = version;
    this.inclusive = inclusive;
}

/**
 * @class VersionRange
 * Defines a range of versions using a lower boundary and optional upper boundary.
 *
 * @param lowerBound The \c #VersionBound object.
 * @param upperBound The \c #VersionBound object, or null for a range with no upper boundary.
 *
 * @return A new \c VersionRange object.
 */
function VersionRange(lowerBound, upperBound)
{
    this.lowerBound = lowerBound;
    this.upperBound = upperBound;
}

/**
 * @class Runtime
 * Represents a runtime related to the CEP infrastructure.
 * Extensions can declare dependencies on particular
 * CEP runtime versions in the extension manifest.
 *
 * @param name    The runtime name.
 * @param version A \c #VersionRange object that defines a range of valid versions.
 *
 * @return A new \c Runtime object.
 */
function Runtime(name, versionRange)
{
    this.name = name;
    this.versionRange = versionRange;
}

/**
* @class Extension
* Encapsulates a CEP-based extension to an Adobe application.
*
* @param id              The unique identifier of this extension.
* @param name            The localizable display name of this extension.
* @param mainPath        The path of the "index.html" file.
* @param basePath        The base path of this extension.
* @param windowType          The window type of the main window of this extension.
                 Valid values are defined by \c #CSXSWindowType.
* @param width           The default width in pixels of the main window of this extension.
* @param height          The d   The default, 255.0, means that the color is fully opaque.
 *
 * @return A new RGBColor object.
 */
function RGBColor(red, green, blue, alpha)
{
    this.red = red;
    this.green = green;
    this.blue = blue;
    this.alpha = alpha;
}

/**
 * @class Direction
 * A point value  in which the y component is 0 and the x component
 * is positive or negative for a right or left direction,
 * or the x component is 0 and the y component is positive or negative for
 * an up or down direction.
 *
 * @param x     The horizontal component of the point.
 * @param y     The vertical component of the point.
 *
 * @return A new \c Direction object.
 */
function Direction(x, y)
{
    this.x = x;
    this.y = y;
}

/**
 * @class GradientStop
 * Stores gradient stop information.
 *
 * @param offset   The offset of the gradient stop, in the range [0.0 to 1.0].
 * @param rgbColor The color of the gradient at this point, an \c #RGBColor object.
 *
 * @return GradientStop object.
 */
function GradientStop(offset, rgbColor)
{
    this.offset = offset;
    this.rgbColor = rgbColor;
}

/**
 * @class GradientColor
 * Stores gradient color information.
 *
 * @param type          The gradient type, must be "linear".
 * @param direction     A \c #Direction object for the direction of the gradient
                (up, down, right, or left).
 * @param numStops          The number of stops in the gradient.
 * @param gradientStopList  An array of \c #GradientStop objects.
 *
 * @return A new \c GradientColor object.
 */
function GradientColor(type, direction, numStops, arrGradientStop)
{
    this.type = type;
    this.direction = direction;
    this.numStops = numStops;
    this.arrGradientStop = arrGradientStop;
}

/**
 * @class UIColor
 * Stores color information, including the type, anti-alias level, and specific color
 * values in a color object of an appropriate type.
 *
 * @param type          The color type, 1 for "rgb" and 2 for "gradient".
                The supplied color object must correspond to this type.
 * @param antialiasLevel    The anti-alias level constant.
 * @param color         A \c #RGBColor or \c #GradientColor object containing specific color information.
 *
 * @return A new \c UIColor object.
 */
function UIColor(type, antialiasLevel, color)
{
    this.type = type;
    this.antialiasLevel = antialiasLevel;
    this.color = color;
}

/**
 * @class AppSkinInfo
 * Stores window-skin properties, such as color and font. All color parameter values are \c #UIColor objects except that systemHighlightColor is \c #RGBColor object.
 *
 * @param baseFontFamily        The base font family of the application.
 * @param baseFontSize          The base font size of the application.
 * @param appBarBackgroundColor     The application bar background color.
 * @param panelBackgroundColor      The background color of the extension panel.
 * @param appBarBackgroundColorSRGB     The application bar background color, as sRGB.
 * @param panelBackgroundColorSRGB      The background color of the extension panel, as sRGB.
 * @param systemHighlightColor          The highlight color of the extension panel, if provided by the host application. Otherwise, the operating-system highlight color. 
 *
 * @return AppSkinInfo object.
 */
function AppSkinInfo(baseFontFamily, baseFontSize, appBarBackgroundColor, panelBackgroundColor, appBarBackgroundColorSRGB, panelBackgroundColorSRGB, systemHighlightColor)
{
    this.baseFontFamily = baseFontFamily;
    this.baseFontSize = baseFontSize;
    this.appBarBackgroundColor = appBarBackgroundColor;
    this.panelBackgroundColor = panelBackgroundColor;
    this.appBarBackgroundColorSRGB = appBarBackgroundColorSRGB;
    this.panelBackgroundColorSRGB = panelBackgroundColorSRGB;
    this.systemHighlightColor = systemHighlightColor;
}

/**
 * @class HostEnvironment
 * Stores information about the environment in which the extension is loaded.
 *
 * @param appName   The application's name.
 * @param appVersion    Tar skinInfo = JSON.parse(window.__adobe_cep__.getHostEnvironment()).appSkinInfo;
 *    // Gets the style information such as color info from the skinInfo,
 *    // and redraw all UI controls of your extension according to the style info.
 * }
 */
CSInterface.THEME_COLOR_CHANGED_EVENT = "com.adobe.csxs.events.ThemeColorChanged";

/** The host environment data object. */
CSInterface.prototype.hostEnvironment = window.__adobe_cep__ ? JSON.parse(window.__adobe_cep__.getHostEnvironment()) : null;

/** Retrieves information about the host environment in which the
 *  extension is currently running.
 *
 *   @return A \c #HostEnvironment object.
 */
CSInterface.prototype.getHostEnvironment = function()
{
    this.hostEnvironment = JSON.parse(window.__adobe_cep__.getHostEnvironment());
    return this.hostEnvironment;
};

/** Closes this extension. */
CSInterface.prototype.closeExtension = function()
{
    window.__adobe_cep__.closeExtension();
};

/**
 * Retrieves a path for which a constant is defined in the system.
 *
 * @param pathType The path-type constant defined in \c #SystemPath ,
 *
 * @return The platform-specific system path string.
 */
CSInterface.prototype.getSystemPath = function(pathType)
{
    var path = decodeURI(window.__adobe_cep__.getSystemPath(pathType));
    var OSVersion = this.getOSInformation();
    if (OSVersion.indexOf("Windows") >= 0)
    {
      path = path.replace("file:///", "");
    }
    else if (OSVersion.indexOf("Mac") >= 0)
    {
      path = path.replace("file://", "");
    }
    return path;
};

/**
 * Evaluates a JavaScript script, which can use the JavaScript DOM
 * of the host application.
 *
 * @param script    The JavaScript script.
 * @param callback  Optional. A callback function that receives the result of execution.
 *          If execution fails, the callback function receives the error message \c EvalScript_ErrMessage.
 */
CSInterface.prototype.evalScript = function(script, callback)
{
    if(callback === null || callback === undefined)
    {
        callback = function(result){};
    }
    window.__adobe_cep__.evalScript(script, callback);
};

/**
 * Retrieves the unique identifier of the application.
 * in which the extension is currently running.
 *
 * @return The unique ID string.
 */
CSInterface.prototype.getApplicationID = function()
{
    var appId = this.hostEnvironment.appId;
    return appId;
};

/**
 * Retrieves host capability information for the application
 * in which the extension is currently running.
 *
 * @return A \c #HostCapabilities object.
 */
CSInterface.prototype.getHostCapabilities = function()
{
    var hostCapabilities = JSON.parse(window.__adobe_cep__.getHostCapabilities() );
    return hostCapabilities;
};

/**
 * Triggers a CEP event programmatically. Yoy can use it to dispatch
 * an event of a predefined type, or of a type you have defined.
 *
 * @param event A \c CSEvent object.
 */
CSInterface.prototype.dispatchEvent = function(event)
{
    if (typeof event.data == "object")
    {
        event.data = JSON.stringify(event.data);
    }

    window.__adobe_cep__.dispatchEvent(event);
};

/**
 * Registers an interest in a CEP event of a particular type, and
 * assigns an event handler.
 * The event infrastructure notifies your extension when events of this type occur,
 * passing the event object to the registered handler function.
 *
 * @param type     The name of the event type of interest.
 * @param listener The JavaScript handler function or method.
 * @param obj      Optional, the object containing the handler method, if any.
 *         Default is null.
 */
CSInterface.prototype.addEventListener = function(type, listener, obj)
{
    window.__adobe_cep__.addEventListener(type, listener, obj);
};

/**
 * Removes a registered event listener.
 *
 * @param type      The name of the event type of interest.
 * @param listener  The JavaScript handler function or method that was registered.
 * @param obj       Optional, the object 
 * If user customizes the User Agent by setting CEF command parameter "--user-agent", only
 * "Mac OS X" or "Windows" will be returned. 
 */
CSInterface.prototype.getOSInformation = function()
{
    var userAgent = navigator.userAgent;

    if ((navigator.platform == "Win32") || (navigator.platform == "Windows"))
    {
        var winVersion = "Windows";
        var winBit = "";
        if (userAgent.indexOf("Windows") > -1)
        {
            if (userAgent.indexOf("Windows NT 5.0") > -1)
            {
                winVersion = "Windows 2000";
            }
            else if (userAgent.indexOf("Windows NT 5.1") > -1)
            {
                winVersion = "Windows XP";
            }
            else if (userAgent.indexOf("Windows NT 5.2") > -1)
            {
                winVersion = "Windows Server 2003";
            }
            else if (userAgent.indexOf("Windows NT 6.0") > -1)
            {
                winVersion = "Windows Vista";
            }
            else if (userAgent.indexOf("Windows NT 6.1") > -1)
            {
                winVersion = "Windows 7";
            }
            else if (userAgent.indexOf("Windows NT 6.2") > -1)
            {
                winVersion = "Windows 8";
            }
            else if (userAgent.indexOf("Windows NT 6.3") > -1)
            {
                winVersion = "Windows 8.1";
            }
            else if (userAgent.indexOf("Windows NT 10") > -1)
            {
                winVersion = "Windows 10";
            }

            if (userAgent.indexOf("WOW64") > -1 || userAgent.indexOf("Win64") > -1)
            {
                winBit = " 64-bit";
            }
            else
            {
                winBit = " 32-bit";			
            }
        }

        return winVersion + winBit;
    }
    else if ((navigator.platform == "MacIntel") || (navigator.platform == "Macintosh"))
    {        
        var result = "Mac OS X";

        if (userAgent.indexOf("Mac OS X") > -1)
        {
            result = userAgent.substring(userAgent.indexOf("Mac OS X"), userAgent.indexOf(")"));
            result = result.replace(/_/g, ".");
        }

        return result;        
    }

    return "Unknown Operation System";
};

/**
 * Opens a page in the default system browser.
 *
 * Since 4.2.0
 *
 * @param url  The URL of the page/file to open, or the email address.
 * Must use HTTP/HTTPS/file/mailto protocol. For example:
 *   "http://www.adobe.com"
 *   "https://github.com"
 *   "file:///C:/log.txt"
 *   "mailto:test@adobe.com"
 *
 * @return One of these error codes:\n
 *      <ul>\n
 *          <li>NO_ERROR - 0</li>\n
 *          <li>ERR_UNKNOWN - 1</li>\n
 *          <li>ERR_INVALID_PARAMS - 2</li>\n
 *          <li>ERR_INVALID_URL - 201</li>\n
 *      </ul>\n
 */
CSInterface.prototype.openURLInDefaultBrowser = function(url)
{
    return cep.util.openURLInDefaultBrowser(url);
};

/**
 * Retrieves extension ID.
 *
 * Since 4.2.0
 *
 * @return extension ID.
 */
CSInterface.prototype.getExtensionID = function()
{
     return window.__adobe_cep__.getExtensionId();
};

/**
 * Retrieves the scale factor of screen. 
 * On Windows platform, the value of scale factor might be different from operating system's scale factor,
 * since host application may use its self-defined scale factor.
 *
 * Since 4.2.0
 *
 * @return One of the following float number.
 *      <ul>\n
 *          <li> -1.0 when error occurs </li>\n
 *          <li> 1.0 means normal screen </li>\n
 *          <li> >1.0 means HiDPI screen </li>\n
 *      </ul>\n
 */
CSInterface.prototype.getScaleFactor = function()
{
    return window.__adobe_cep__.getScaleFactor();
};

/**
 * Set a handler to detect any changes of scale factor. This only works on Mac.
 *
 * Since 4.2.0
 *
 * @param handler   The function to be called when scale factor is changed.
 *
 */
CSInterface.prototype.setScaleFactorChangedHandler = function(handler)
{
    window.__adobe_cep__.semple2-2" Enabled="true" Checkable="true" Checked="true"/>
 *   </MenuItem>
 *   <MenuItem Label="---" />
 *   <MenuItem Id="menuItemId3" Label="TestExample3" Enabled="false" Checkable="true" Checked="false"/>
 * </Menu>
 */
CSInterface.prototype.setContextMenu = function(menu, callback)
{
    if ("string" != typeof menu)
    {
        return;
    }
    
	window.__adobe_cep__.invokeAsync("setContextMenu", menu, callback);
};

/**
 * Set context menu by JSON string.
 *
 * Since 6.0.0
 *
 * There are a number of conventions used to communicate what type of menu item to create and how it should be handled.
 * - an item without menu ID or menu name is disabled and is not shown.
 * - if the item label is "---" (three hyphens) then it is treated as a separator. The menu ID in this case will always be NULL.
 * - Checkable attribute takes precedence over Checked attribute.
 * - a PNG icon. For optimal display results please supply a 16 x 16px icon as larger dimensions will increase the size of the menu item. 
     The Chrome extension contextMenus API was taken as a reference.
 * - the items with icons and checkable items cannot coexist on the same menu level. The former take precedences over the latter.
     https://developer.chrome.com/extensions/contextMenus
 *
 * @param menu      A JSON string which describes menu structure.
 * @param callback  The callback function which is called when a menu item is clicked. The only parameter is the returned ID of clicked menu item.
 *
 * @description An example menu JSON:
 *
 * { 
 *      "menu": [
 *          {
 *              "id": "menuItemId1",
 *              "label": "testExample1",
 *              "enabled": true,
 *              "checkable": true,
 *              "checked": false,
 *              "icon": "./image/small_16X16.png"
 *          },
 *          {
 *              "id": "menuItemId2",
 *              "label": "testExample2",
 *              "menu": [
 *                  {
 *                      "id": "menuItemId2-1",
 *                      "label": "testExample2-1",
 *                      "menu": [
 *                          {
 *                              "id": "menuItemId2-1-1",
 *                              "label": "testExample2-1-1",
 *                              "enabled": false,
 *                              "checkable": true,
 *                              "checked": true
 *                          }
 *                      ]
 *                  },
 *                  {
 *                      "id": "menuItemId2-2",
 *                      "label": "testExample2-2",
 *                      "enabled": true,
 *                      "checkable": true,
 *                      "checked": true
 *                  }
 *              ]
 *          },
 *          {
 *              "label": "---"
 *          },
 *          {
 *              "id": "menuItemId3",
 *              "label": "testExample3",
 *              "enabled": false,
 *              "checkable": true,
 *              "checked": false
 *          }
 *      ]
 *  }
 *
 */
CSInterface.prototype.setContextMenuByJSON = function(menu, callback)
{
    if ("string" != typeof menu)
    {
        return;	
    }
    
	window.__adobe_cep__.invokeAsync("setContextMenuByJSON", menu, callback);
};

/**
 * Updates a context menu item by setting the enabled and selection status.
 *  
 * Since 5.2.0
 *
 * @param menuItemID	The menu item ID. 
 * @param enabled		True to enable the item, false to disable it (gray it out).
 * @param checked		True to select the item, false to deselect it.
 */
CSInterface.prototype.updateContextMenuItem = function(menuItemID, enabled, checked)
{
	var itemStatus = new ContextMenuItemStatus(menuItemID, enabled, checked);
	ret = window.__adobe_cep__.invokeSync("updateContextMenuItem", JSON.stringify(itemStatus));
};

/**
 * Get the visibility status of an extension window. 
 *  
 * Since 6.0.0
 *
 * @return true if the extension window is visible; false if the extension wi * This function works with modal and modeless extensions in all Adobe products, and panel extensions in Photoshop, InDesign, InCopy, Illustrator, Flash Pro and Dreamweaver.
 *
 * Since 6.1.0
 *
 * @return The window title.
 */
CSInterface.prototype.getWindowTitle = function()
{
    return window.__adobe_cep__.invokeSync("getWindowTitle", "");
};
