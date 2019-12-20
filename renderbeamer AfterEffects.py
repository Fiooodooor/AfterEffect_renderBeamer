import os
from shutil import copy2, copytree, copy, rmtree
from time import gmtime, strftime
import tempfile
import subprocess
import hashlib
import re
import sys
import traceback
from os.path import expanduser
import threading
import platform
import shutil

try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

VERSION_PLUGIN = '5'

'''
import Tkinter
import tkMessageBox

top = Tkinter.Tk()
def hello():
   tkMessageBox.showinfo("Say Hello", "Hello World")

B1 = Tkinter.Button(top, text = "Say Hello", command = hello)
B1.pack()

top.mainloop()


'''

os_symlink = getattr(os, "symlink", None)
if callable(os_symlink) is False and platform.system() == "Windows":
    
    def symlink_win(source, linkname):
        """ Symlink implementation on Windows platform, working from Vista
        """
        import ctypes
        flags = 1 if source is not None and os.path.isdir(source) else 0
        try:
            linkname = linkname.replace('/', '\\')
            csl = ctypes.windll.kernel32.CreateSymbolicLinkW
            csl.argtypes = (ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32)
            csl.restype = ctypes.c_ubyte
            ret = csl(linkname, source, flags)
        except Exception as e:
            ret = 0
        if ret == 0:
            raise OSError
    os.symlink = symlink_win

os_hardlink = getattr(os, "link", None)
if callable(os_hardlink) is False and platform.system() == "Windows":
    
    def link_win(source, linkname):
        """ Hardlink implementation on Windows platform, working from Vista
        """
        import ctypes
        flags = 1 if source is not None and os.path.isdir(source) else 0
        try:
            linkname = linkname.replace('/', '\\')
            csl = ctypes.windll.kernel32.CreateHardLinkW
            csl.argtypes = (ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32)
            csl.restype = ctypes.c_ubyte
            ret = csl(linkname, source, flags)
        except Exception as e:
            print e
            ret = 0
        if ret == 0:
            raise OSErr
        
class Memoize:

    def __init__(self, fn):
        self.fn = fn
        self.memo = {}

    def __call__(self, *args):
        if args not in self.memo:
            self.memo[args] = self.fn(*args)
        return self.memo[args]
    
@Memoize 
def copyWithSymlink(src, dst):
    # copy single file with symlink
    try:
        
        if platform.system() == "Windows":
            src = fixWinLongPath(src)
            dst = fixWinLongPath(dst)
        if not os.path.exists(dst):   
            try:
                os.symlink(src, dst)
                flag = True
                try:
                    myfile = open(dst, "r") 
                    myfile.close()
                except IOError:
                    flag = False
                    os.remove(dst)
                if flag:
                    printlog("Used symlink to copy file")
                    return True
            except:
                pass
            
            try:
                os.link(src, dst)
                printlog("Used hardlink to copy file")
                return True
            except:
                pass
                
            shutil.copy(src, dst)
            return True
    except Exception as e:
        printlogEx("Error copy file with Symlink", e)
        return False

@Memoize 
def copyTreeWithSymlink(src, dst):
    # copy whole directory with symlink
    try:
        if platform.system() == "Windows":
            src = fixWinLongPath(src)
            dst = fixWinLongPath(dst)
            
        if not os.path.exists(dst): 
            
            
            try:
                os.symlink(src, dst)
                flag = True
                try:
                    os.listdir(dst)
                except Exception as e:
                    flag = False
                    os.rmdir(dst)
                if flag:
                    printlog("Used symlink to copy directory")
                    return True
            except Exception as e:
                pass
            
            try:
                copyTreeHard(src, dst)
                printlog("Used hardlink to copy directory")
                return True
            except Exception as e:
                pass
            
            if os.path.exists(dst):
                shutil.rmtree(dst)
            
            shutil.copytree(src, dst)
            return True
    except Exception as e:
        printlogEx("Error copy directory with Symlink", e)
        return False
    
def getUserDir():
    # returns user home directory
    if platform.system() == "Windows":
        home = os.environ['HOMEDRIVE'] + os.environ['HOMEPATH']
        if not os.path.exists(os.path.join(home,'renderbeamer')):
            home = expanduser("~")
    else:
        home = expanduser("~")
    return home

class BeamerHandler:

    def __init__(self):
        self.proc = None
        self.response = 'ERROR:'
        self.timeout = 120
 
    # returns the user logged into beamer    
    def getUser(self):
        return self.__run(["-getusr"])
    
    # returns Tempdir
    def getTempdir(self):
        return self.__run(["-tempdir"])
        
    # check scene name on REST server
    def checkSceneName(self, scene, user):
        return self.__run(['-checkName',str(scene), str(user), "-app", "Aftereffects"])
    
    # submit project to beamer
    def addScene(self, localPath, farmPath):
        os = platform.system()
        if os == "Windows":
            return self.__run(["-app", "Aftereffects", "-a" , localPath, "-sn" , str(farmPath) ])
        else:
            return self.__run(["-app", "Aftereffects", "-a" , localPath , "-sn" , str(farmPath) ])

    # submit log to beamer
    def sendLog(self, logfile, user):
        return self.__run(["-log", logfile, "-logSubject", " " + str(user) + " AfterEffects "])
        
     
    # runs beamer with parameters and returns the results
    # result has 2 elements , the first one is boolean if the command was successful, the second is the data or error string    
    def __runBeamer(self, params):
        try:
            cmd = self.__createCommand(params)
            printlog("running cmd: " + " ".join(cmd))
            self.proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, shell = True)
            self.response = str(self.proc.communicate()[0].decode("utf-8").strip())
        except Exception as e:
            printlog("running command: "+str(e))
            return False, str(e)

    def __run(self, params):
        try:
            thread = threading.Thread(target=self.__runBeamer, args=(params,))
            thread.start()
            thread.join(self.timeout)

            if thread.is_alive():
                self.proc.terminate()
                thread.join()
                self.response = 'ERROR: renderbeamer did not reply in time'

            if self.response.startswith("ERROR:"):
                return False, self.response
            else:
                return True, self.response
        except Exception as e:
            printlog("running command error: "+str(e))
            return False, str(e)
    
    # creates the full command to run beamer on system    
    def __createCommand(self, params):
        printlog("creating cmd")
        try:
            beamerDir = self.__getBeamerDir()
            beamerScript = os.path.join(beamerDir , self.__getBeamerScript())
            cmda = [beamerScript]
            cmda.extend(params)
            return cmda
        except Exception as e:
            printlog("creating command error: "+str(e))
            
    
    # returns the path to beamer shell script        
    def __getBeamerScript(self):
        o = platform.system()
        if o == "Windows":
            return "renderbeamer.bat"
        else:
            return "renderbeamer.sh"
        
    
    # returns beamer home, its (user directory)/renderbeamer but can be overwritten in mb.cfg config   
    def __getBeamerDir(self):
        home = getUserDir()
        return os.path.join(home, "renderbeamer")
    

def fixWinLongPath(src):
    if platform.system() == "Windows":
        src=unfixWinLongPath(src)
        out = os.path.normpath(src)
        if len(out) < 250:
            return out
        if out.startswith("\\"):
            return "\\\\?\\UNC\\"+out[2:]
        return "\\\\?\\" + out
    return src

def unfixWinLongPath(src):
    if src.startswith("\\\\?\\UNC\\"):
        return "\\\\"+src[7:]
    if src.startswith("\\\\?\\"):
        return src[4:]
    return src
    
def createlog():
    global logfile
    logfile = os.path.join(getUserDir(),'.renderbeamer','log','aftereffects','pluginLog_AftereffectsCopyScript_'+strftime('%Y%m%d-%H%M%S', gmtime())+'.txt')
    if not os.path.exists(os.path.split(logfile)[0]):
        os.makedirs(os.path.split(logfile)[0])
    try:
        file_ = open(logfile, 'a')
        file_.write(strftime('%Y%m%d-%H%M%S ', gmtime())+'Create log\n')
        file_.close()
    except Exception, e:
        traceback.print_exc()
     
def printlog(log):
    global logfile    
    file_ = open(logfile, 'a')
    file_.write(strftime('%Y%m%d-%H%M%S', gmtime())+' Info: '+log.encode('utf-8') +'\n')
    file_.close()
    
def printlogEx(log, ex):    
    text = "exception " + log + "\n" + str(ex) + "\n" + ''.join(traceback.format_stack()) + ''.join(traceback.format_exc())
    printlog(text)
    
def md5_for_file(f, block_size=2**20*100):
    f = open(f,'rb')
    md5 = hashlib.md5()
    while True:
        data = f.read(block_size)
        if not data:
            break
        md5.update(data)
    return md5.hexdigest()

def check_latin(path):
    chars = "-0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz."
    text = os.path.basename(path)
    text = os.path.splitext(text)[0]+os.path.splitext(text)[1][1:]
    for i in text:
        if not i in chars:
            return False
    return True

def check_file(oldpath, newpath):
    if os.path.isfile(newpath) or not check_latin(oldpath):
        name  = md5_for_file(oldpath)
        if os.path.isfile(newpath):
            name_new  = md5_for_file(newpath)
            if name == name_new:
                return newpath
        
        filename = os.path.basename(oldpath)
        newpath = os.path.join(os.path.dirname(newpath),name+os.path.splitext(filename)[1])
        
        return newpath
    else:
        return newpath


class Asset(object):
    def __init__(self, node, rootNode):
        self._asset = node # asset node name in scene returned by modo
        self.rootNode = rootNode
        self.readPath() # reads the asset path set in the scene
        self.assetDir = 'relinkedAssets'


    def readPath(self):
        # read the path to the asset currently set in scene

        path = self._asset.attrib['fullpath']
        printlog(path)
        self.seq =  True if self._asset.attrib["target_is_folder"] == "1" else False
        if self.seq:
            self.seqName = self.rootNode.getchildren()[3].text
            self.seqExt =  self.rootNode.getchildren()[4].text
            
        self.originalPath = path
        
        if not self.seq:
            self.fileName = os.path.basename(self.originalPath)
        
        
    def relink(self, user):
        # relinks the asset to farm path
        
        try:
            self.submitPath = os.path.join('U:\\', user, os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(self.submitPath)))), 'data', self.assetDir, os.path.basename(self.submitPath))
            self.submitPath = self.submitPath.replace('/','\\').replace('\\\\','\\')
            self._asset.attrib['fullpath'] = self.submitPath
            printlog("relinked: " + self.originalPath + " to: " + self.submitPath)
            return True
        except Exception as e:
            printlogEx("relinking: " + self.originalPath + " to: " + self.submitPath, e)
            return False
    
        
    def copyToSubmit(self, submitDir):
        # copy asset file to submit directory
        try:  
            assetDir = os.path.join(submitDir, self.assetDir)
            if not os.path.exists(fixWinLongPath(assetDir)):
                os.makedirs(fixWinLongPath(assetDir))
                
            if self.seq:
                md5 = hashlib.md5()
                md5.update(self.originalPath)
                newName = md5.hexdigest()
                assetDir = os.path.join(assetDir, newName)
                self.submitPath = assetDir
                if not os.path.exists(fixWinLongPath(assetDir)):
                    os.makedirs(fixWinLongPath(assetDir))

                for f in self.seqFiles:
                    seqSubmitPath = os.path.join(assetDir, f)
                    src = os.path.join(self.seqDir, f)
                    printlog("copying: " + src + " to: " + seqSubmitPath)
                    copyWithSymlink(src, seqSubmitPath)

            else:
                # for single files just copy them
                self.submitPath = os.path.join(assetDir, self.fileName)
                self.submitPath = check_file(self.filePath, self.submitPath)
                if not os.path.isfile(self.submitPath):
                    printlog("copying: " + self.filePath + " to: " + self.submitPath)
                    copyWithSymlink(self.filePath, self.submitPath)
                else:
                    printlog("File is already copied: " + self.filePath)
                    return True
            return True
        except Exception as ex:
            printlogEx("copy asset to submit", ex)
            return False
            
        
    def exists(self):
        # checks if the assets original path is correct
        if self.seq:
            self.findSeqFiles()
            if self.seqFiles:
                return True
        else:
            if os.path.exists(self.originalPath):
                self.filePath = self.originalPath
                return True
        
                
        return False
    
    def findSeqFiles(self):
        # finds all paths to files of a stacked asset using regex
        # this is performed when the assets are copied to submit  directory
        # not recursive, just searches the specified directory
        try:
            self.seqFiles = set()
            regex = self.seqName + '\d+' +self.seqExt
            printlog("Sequence regex: "+regex)
            #for f in os.listdir(os.path.dirname(self.originalPath)):
            self.seqDir = self.originalPath
            for f in os.listdir(self.seqDir):
                if re.match(regex, f):
                    printlog('seq files:'+f )
                    self.seqFiles.add(f)
                
        except Exception as e:
            printlogEx("finding seqfiles",e)
            
            
class AssetHandler(object):
    
    def __init__(self, scene):
        self.scene = scene
        self.assetReadList = list()
        self.assetList = list()
        self.missingAssets = list()
        self.assetFileMap = dict()

    def readAssets(self):
        ET.register_namespace('', "http://www.adobe.com/products/aftereffects")
        
        self.sceneXml = ET.parse(fixWinLongPath(self.scene))
        
        items =  self.sceneXml.findall('.//{http://www.adobe.com/products/aftereffects}Pin')
   
        for it in items:
                i = it.find('.//{http://www.adobe.com/products/aftereffects}Als2/{http://www.adobe.com/products/aftereffects}fileReference')
                if i != None:
                    assetObject = Asset(i, it)
                    self.assetReadList.append(assetObject)
            
        self.checkAssets()

    def checkAssets(self):
        for assetObject in self.assetReadList:
            if not assetObject.exists():
                self.missingAssets.append(assetObject)
            else:
                self.assetList.append(assetObject)
                
    def relinkAssets(self, user):
        printlog("relinking...")
        for f in self.assetList:
            f.relink(user)

    def copyAssets(self, submitDir):
        printlog("copying assets...")
        try:
            for asset in self.assetList:
                asset.copyToSubmit(submitDir)

        except Exception as e:
            printlogEx("copying assets" ,e)
            
    def saveScene(self):
        try:
            os.remove(fixWinLongPath(self.scene))
            self.sceneXml.write(fixWinLongPath(self.scene), encoding='utf-8', xml_declaration=True)
            
        except Exception as e:
            printlogEx("save scene" ,e)


def main():
    
    global user
    user = ''
    global logfile
    createlog()   
    __beamerHandler = BeamerHandler()
    response = __beamerHandler.getUser()
    if response[0]:
        user = response[1]
    scene = sys.argv[1]   
    #printlog(scene)
    #scene = "U:\\2018\\data\\HUL_040316_GF_RENDERQUEUE copy.aepx"
    #scene = "U:\\5513_SEQUENCE_AND_PSD_ISSUE\\NBC_render_Farm_low_version folder\\plik.aepx"
    assetHandler = AssetHandler(scene)
    
    assetHandler.readAssets()

    submitDir = os.path.dirname(scene)
    
    sceneName = os.path.splitext(os.path.basename(scene))[0]

    # copying assets
    assetHandler.copyAssets(submitDir)
    # relink
    assetHandler.relinkAssets(user )
    
    assetHandler.saveScene()
    
    # copylog
    if not os.path.exists(os.path.join(submitDir, 'log')):
        os.makedirs(os.path.join(submitDir, 'log'))
    copy(logfile, fixWinLongPath(os.path.join(submitDir, 'log', os.path.splitext(os.path.basename(logfile))[0]+'_'+sceneName+'.txt') ))
    
         
if __name__ == "__main__":
    try:
        main()
    except:
        traceback.print_exc()
        file_ = open(logfile, 'a')
        traceback.print_exc(file = file_)
        file_.close()
        __beamerHandler = BeamerHandler()              
        __beamerHandler.sendLog(logfile, user)
