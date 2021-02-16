
#include "AeGfsFileBuilder.h"

AeGfsFileBuilder* AeGfsFileBuilder::getInstance()
{	
	static AeGfsFileBuilder instance;
	return &instance;
}


ErrorCodesAE AeGfsFileBuilder::generateFullGfs()
{
	ErrorCodesAE _ErrorCode = NoError;
	ERROR_AE(generateGfsAndXmlHeader());
	ERROR_AE(generateSetting());
	ERROR_AE(generateRqItems());
	ERROR_AE(generateFonts());
	ERROR_AE(generateEffects());
	ERROR_AE(generateTail());
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::generateGfsAndXmlHeader() 
{
	ErrorCodesAE _ErrorCode = NoError;
	if (f_path.empty())
		return GfsFileOpenError;
	if (!fs::exists(f_path.parent_path()))
		return GfsFileOpenError;
	if (f_prv.is_open())
		f_prv.close();
	std::wofstream a;
    WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios::trunc);

	if(!f_prv.good())
		return GfsFileOpenError;

	f_prv << rbUtilities::toUtf8(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>") << std::endl;
	f_prv.close();
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::generateSetting() 
{ 
	ErrorCodesAE _ErrorCode = NoError;
	f_prv.imbue(toUtfLocale);
    WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios_base::app);

	if (f_prv.good()) 
	{
		gfsRqItem *pt = rqItems.front();
		fs::path correctUserOut = pt->outMods.back()->outputFile.parent_path();
		fs::path correctFileOut = pt->outMods.back()->outputFile.filename();
		rbUtilities::pathStringFixIllegal(correctUserOut, false, true);
		rbUtilities::pathStringFixIllegal(correctFileOut, false, false);
		correctUserOut += '\\';
		correctUserOut += correctFileOut.wstring();
		correctUserOut.replace_extension();

//		RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"<Scene App=\"AfterEffects\" version=\"%hs\" >\n\t<Settings outFileExt=\"%ls\" framestring=\"%hs\" outFileName=\"%ls\" outFilePath=\"%ls\" userOutput=\"%ls\" >"
 //                   , this->versionString , pt->outMods.back()->outputFile.extension().wstring().c_str()+1, pt->frameString, this->outFileName.wstring().c_str(), this->outFilePath.wstring().c_str() , correctUserOut.wstring().c_str() );
		RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"<Scene App=\"AfterEffects\" version=\"%hs\" >\n\t<Settings outFileExt=\"png\" framestring=\"%hs\" outFileName=\"%ls\" outFilePath=\"%ls\" userOutput=\"%ls.png\" >"
			, this->versionString, pt->frameString, this->outFileName.wstring().c_str(), this->outFilePath.wstring().c_str(), correctUserOut.wstring().c_str());
		f_prv << rbUtilities::toUtf8(pbuff) << std::endl;

        RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"\t\t<AfterEffects height=\"%d\" width=\"%d\" fontDir=\"%ls\" >", pt->height, pt->width, this->outFontsDir.wstring().c_str());
		f_prv << rbUtilities::toUtf8(pbuff) << std::endl;
		f_prv.close();
	}
	return _ErrorCode; 
}
ErrorCodesAE AeGfsFileBuilder::generateRqItems() 
{ 
	ErrorCodesAE _ErrorCode = NoError;	
	f_prv.imbue(toUtfLocale);
	WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios_base::app);
	if (f_prv.good())
	{
		for (auto pt : this->rqItems)
		{
			if (pt->outMods.empty()) {
				_ErrorCode = AE_ErrStruct;
				break;
			}			
			//RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"\t\t\t<RenderQueueItem index=\"%d\" nameComp=\"%hs\" outFileExt=\"%ls\" width=\"%d\" height=\"%d\" framestring=\"%hs\" fps=\"%hs\" isSeq=\"%d\" isMultiFr=\"%d\" outType=\"%hs\" outInfo=\"%hs\" ",
            //            pt->indexNr, pt->compositioName, pt->outMods.back()->outputFile.extension().wstring().c_str() + 1, pt->width, pt->height, pt->frameString, pt->fps, pt->outMods.back()->outFileIsSeq, pt->outMods.back()->outFileIsMultiframe, pt->outMods.back()->outputType, pt->outMods.back()->outputInfo);
			RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"\t\t\t<RenderQueueItem index=\"%d\" nameComp=\"%hs\" outFileExt=\"png\" width=\"%d\" height=\"%d\" framestring=\"%hs\" fps=\"%hs\" isSeq=\"%d\" isMultiFr=\"%d\" outType=\"png\" outInfo=\"-\" ",
				pt->indexNr, pt->compositioName, pt->width, pt->height, pt->frameString, pt->fps, pt->outMods.back()->outFileIsSeq, pt->outMods.back()->outFileIsMultiframe);
			f_prv << rbUtilities::toUtf8(pbuff);
			if (pt->outMods.back()->outputAudioEnabled) {
				RB_SWPRINTF(pbuff, PBUFFER_SIZE, L" audioEnabled=\"%d\" audioInUse=\"%d\" numChannels=\"%ld\" bytesPerSample=\"%ld\" encoding=\"%ld\" sampleRate=\"%.5f\" />",
						pt->outMods.back()->outputAudioEnabled, pt->outMods.back()->outputAudioSetToUse, pt->outMods.back()->soundFormat.num_channelsL, pt->outMods.back()->soundFormat.bytes_per_sampleL, pt->outMods.back()->soundFormat.encoding, pt->outMods.back()->soundFormat.sample_rateF);
			}
			else {
				RB_SWPRINTF(pbuff, PBUFFER_SIZE, L" />");
			}
			f_prv << rbUtilities::toUtf8(pbuff) << std::endl;
		}
		f_prv.close();
	}

	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::generateFonts() 
{ 
	ErrorCodesAE _ErrorCode = NoError;
	f_prv.imbue(toUtfLocale);
	WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios_base::app);
	if (f_prv.good())
	{
		for (auto pt : this->fontsList)
		{
            RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"\t\t\t<Font name=\"%d_%hs\" filename=\"%ls\" />", pt->subFontNr, pt->fontName.c_str(), pt->fontFile.wstring().c_str());
			f_prv << rbUtilities::toUtf8(pbuff) << std::endl;
		}
		f_prv.close();
	}
	return _ErrorCode; 
}
ErrorCodesAE AeGfsFileBuilder::generateEffects() 
{ 
	ErrorCodesAE _ErrorCode = NoError;
	f_prv.imbue(toUtfLocale);
	WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios_base::app);
	if (f_prv.good())
	{
		for (auto pt : this->effectsList)
		{
			RB_SWPRINTF(pbuff, PBUFFER_SIZE, L"\t\t\t<Effect name=\"%hs\" matchName=\"%hs\" effectCategory=\"%hs\" installedKey=\"%ld\" />", pt->safeName.c_str(), pt->matchName.c_str(), pt->effectCategory.c_str(), pt->installKey);
			f_prv << rbUtilities::toUtf8(pbuff) << std::endl;
		}
		f_prv.close();
	}
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::generateTail() 
{ 
	ErrorCodesAE _ErrorCode = NoError;
	f_prv.imbue(toUtfLocale);
	WOPEN(f_prv,f_path,std::ios::out | std::ios::binary | std::ios_base::app);
	if (f_prv.good())
	{
		f_prv << rbUtilities::toUtf8(L"\t\t</AfterEffects>\n\t</Settings>\n</Scene>\n") << std::endl;
		f_prv.close();
	}
	return _ErrorCode; 
}

ErrorCodesAE AeGfsFileBuilder::initGfsFileBuilder(beamerParamsStruct const &theBps)
{
	outFileName.clear();
	outFilePath.clear();
	outFontsDir.clear();	

	rqItems.clear();
	effectsList.clear();
	fontsList.clear();
	if (f_prv.is_open())
		f_prv.close();
	f_path.clear();

	return initGfsFileBuilder(theBps.bp.relGfsFile, theBps.bp.projectFilenameCorrect, theBps.bp.remotePath, theBps.bp.remoteFontsPath, theBps.versionStr);
}
ErrorCodesAE AeGfsFileBuilder::initGfsFileBuilder(fs::path const &gfsFilePath, fs::path const &fileName, fs::path const &outPath, fs::path const &fontsDir, A_char const *version)
{
	ErrorCodesAE _ErrorCode = NoError;
	fs::path tmpPath = outPath;
	ERROR_AE(setGfsFilePath(gfsFilePath));
	ERROR_AE(setOutFileName(fileName));
	ERROR_AE(setOutFilePath(tmpPath.replace_extension()));
	ERROR_AE(setFontsDirPath(fontsDir));
	ERROR_AE(setAeVersionString(version));
	return _ErrorCode;
}

ErrorCodesAE AeGfsFileBuilder::setGfsFilePath(fs::path const &gfsFilePath)
{
	ErrorCodesAE _ErrorCode = NoError;
	this->f_path = gfsFilePath;
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::setOutFileName(fs::path const &fileName)
{
	ErrorCodesAE _ErrorCode = NoError;
	this->outFileName = fileName;
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::setOutFilePath(fs::path const &outPath)
{
	ErrorCodesAE _ErrorCode = NoError;
	this->outFilePath = outPath;
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::setFontsDirPath(fs::path const &fontsDir)
{
	ErrorCodesAE _ErrorCode = NoError;
	this->outFontsDir = fontsDir;
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::setAeVersionString(A_char const *version)
{
	ErrorCodesAE _ErrorCode = NoError;
	size_t len = (sizeof(versionString) / sizeof(A_char));
	RB_STRNCPTY(this->versionString, version, len-2);
	this->versionString[len - 1] = '\0';
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::pushRqItem(gfsRqItem *node)
{
	ErrorCodesAE _ErrorCode = NoError;
	this->rqItems.push_back(node);
	return _ErrorCode;
}

ErrorCodesAE AeGfsFileBuilder::pushFontNode(gfsFontNode* node_pt)
{
	ErrorCodesAE _ErrorCode = NoError;
    for(auto it : fontsList) {
        if(it->fontFile.compare(node_pt->fontFile) == 0) {
            delete node_pt;
            return ErrorCodesAE::NoError;
        }
    }
	fontsList.push_back(node_pt);
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileBuilder::pushEffectsNode(gfsEffectNode* node_pt)
{
	ErrorCodesAE _ErrorCode = NoError;
    for(auto it : effectsList) {
        if(it->installKey == node_pt->installKey) {
            delete node_pt;
            return ErrorCodesAE::NoError;
        }
    }
	effectsList.push_back(node_pt);
	return _ErrorCode;
}

AeGfsFileBuilder::AeGfsFileBuilder() 
	: f_prv(), f_path()
{
	try {
		toUtfLocale = std::locale(RB_LOCALESTRING);	
	}
	catch (...) {
		toUtfLocale = std::locale("en-US");		
	}
	f_prv.imbue(toUtfLocale);
}
AeGfsFileBuilder::~AeGfsFileBuilder()
{
    gfsRqItem* tmp;
    while(!this->rqItems.empty()) {
        tmp = this->rqItems.back();
        if(tmp)
            delete tmp;
        this->rqItems.pop_back();
    }
    
}
