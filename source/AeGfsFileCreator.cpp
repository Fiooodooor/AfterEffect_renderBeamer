
#include "AeGfsFileCreator.h"

AeGfsFileCreator* AeGfsFileCreator::getInstance()
{	
	static AeGfsFileCreator instance;
	return &instance;
}

AeGfsFileCreator::AeGfsFileCreator()
	: document_declaration(nullptr), doc_root(nullptr), doc_settings(nullptr), doc_aftereffects(nullptr), rq_item(nullptr), font_item(nullptr), effect_item(nullptr)
{
}
AeGfsFileCreator::~AeGfsFileCreator()
{
	ClearGfsFileCreator();
}
ErrorCodesAE AeGfsFileCreator::GenerateAndSaveDocument()
{
	if (rqItems.empty())
		return NoValidRqItems;
	gfsRqItem *pt = rqItems.front();

	gfs_document.Clear();
	document_declaration = gfs_document.NewDeclaration();
	gfs_document.InsertFirstChild(document_declaration);

	doc_root = gfs_document.NewElement("Scene");
	doc_root->SetAttribute("App", "AfterEffects");
	doc_root->SetAttribute("version", versionString.c_str());
	gfs_document.InsertAfterChild(document_declaration, doc_root);

	// <Settings outFileExt="png" framestring="0to505s1" outFileName="AE_Mac_test_1_.aepx" outFilePath="AE_Mac_test_1K/AE_Mac_test_1K-renders" userOutput="/Users/soth/Desktop/AE_v17_mac_tests/AE_Mac_test_1_/Final_Comp_.png" >
	doc_settings = gfs_document.NewElement("Settings");
	doc_settings->SetAttribute("outFileExt", pt->output_mods.back()->file_ext);
	doc_settings->SetAttribute("framestring", pt->frame_string);
	doc_settings->SetAttribute("outFileName", outFileName.string().c_str());
	doc_settings->SetAttribute("outFilePath", ("\\" + outFilePath.string()).c_str());
	doc_settings->SetAttribute("userOutput", fs::path(pt->output_mods.back()->outputFile).replace_extension(".png").string().c_str());
	doc_root->InsertEndChild(doc_settings);

	// <AfterEffects height="1080" width="1920" fontDir="AE_Mac_test_1K/data/fonts" >
	doc_aftereffects = gfs_document.NewElement("AfterEffects");
	if(pt->continue_on_missing == 1)
		doc_aftereffects->SetAttribute("continueOnMissingAssets", "true");
	else
		doc_aftereffects->SetAttribute("continueOnMissingAssets", "false");
	doc_aftereffects->SetAttribute("height", pt->height);
	doc_aftereffects->SetAttribute("width", pt->width);
	doc_aftereffects->SetAttribute("fontDir", ("\\" + outFontsDir.string()).c_str());
	doc_settings->InsertEndChild(doc_aftereffects);

	if (GenerateRenderQueueItems() == NoError)
	{
		GenerateFontItems();
		GenerateEffectItems();
		return SaveDocument();
	}
	return GfsFileOpenError;
}
ErrorCodesAE AeGfsFileCreator::SaveDocument()
{
	ERROR_CATCH_START
		std::fstream gfs_file_out_ptr;
		std::locale locale_utf8;
		tinyxml2::XMLPrinter gfs_out_printer;

		if (gfs_file_path.empty() || !fs::exists(gfs_file_path.parent_path())) {
			return GfsFileOpenError;
		}
		try {
			locale_utf8 = std::locale("en_US.UTF8");
		}
		catch (std::runtime_error&) {
			locale_utf8 = std::locale(locale_utf8, "", std::locale::ctype);
		}
		gfs_file_out_ptr.imbue(locale_utf8);
		gfs_file_out_ptr.open(gfs_file_path.c_str(), std::fstream::out);

		if (!gfs_file_out_ptr.is_open())
			return GfsFileOpenError;
		gfs_document.Print(&gfs_out_printer);
		gfs_file_out_ptr.write(gfs_out_printer.CStr(), gfs_out_printer.CStrSize()-1);
		gfs_file_out_ptr.flush();
		gfs_file_out_ptr.close();
	ERROR_CATCH_END_NO_INFO_RETURN
}
ErrorCodesAE AeGfsFileCreator::GenerateRenderQueueItems()
{
	if (rqItems.empty())
		return NoValidRqItems;

	ErrorCodesAE _ErrorCode = NoError;
	for (auto *pt : this->rqItems)
	{
		if (pt->output_mods.empty()) {
			_ErrorCode = AE_ErrStruct;
			continue;
		}
		// <RenderQueueItem index="1" nameComp="Final_Comp" outFileExt="png" width="1920" height="1080" framestring="0to505s1" fps="30.0" isSeq="0" isMultiFr="1" outType="png" outInfo="-">
		rq_item = gfs_document.NewElement("RenderQueueItem");
		rq_item->SetAttribute("index", pt->indexNr);
		rq_item->SetAttribute("nameComp", pt->composition_name.c_str());
		rq_item->SetAttribute("outFileExt", pt->output_mods.back()->file_ext_format);
		rq_item->SetAttribute("width", pt->width);
		rq_item->SetAttribute("height", pt->height);
		rq_item->SetAttribute("framestring", pt->frame_string);
		rq_item->SetAttribute("fps", pt->fps);
		rq_item->SetAttribute("isSeq", pt->output_mods.back()->outFileIsSeq);
		rq_item->SetAttribute("isMultiFr", 0);
		rq_item->SetAttribute("outType", pt->output_mods.back()->file_ext);
		rq_item->SetAttribute("audioEnabled", pt->output_mods.back()->outputAudioEnabled);

		if(pt->output_mods.back()->video_encoder[0] != '\0' ) {
			rq_item->SetAttribute("encoder", pt->output_mods.back()->video_encoder);
			rq_item->SetAttribute("pixelFormat", pt->output_mods.back()->video_pixel_format);
			rq_item->SetAttribute("profile", pt->output_mods.back()->video_profile);
			rq_item->SetAttribute("bitRate", pt->output_mods.back()->video_bitrate);
		}
		
		if (pt->output_mods.back()->outputAudioEnabled) {
			// audioEnabled="1" audioInUse="1" numChannels="2" bytesPerSample="4294967298" encoding="105553116266498" sampleRate="48000.00000" />			
			rq_item->SetAttribute("audioInUse", pt->output_mods.back()->outputAudioSetToUse);
			rq_item->SetAttribute("numChannels", pt->output_mods.back()->soundFormat.num_channelsL);
			rq_item->SetAttribute("bytesPerSample", pt->output_mods.back()->soundFormat.bytes_per_sampleL);
			rq_item->SetAttribute("encoding", pt->output_mods.back()->soundFormat.encoding);
			rq_item->SetAttribute("sampleRate", pt->output_mods.back()->soundFormat.sample_rateF);
		}
		doc_aftereffects->InsertEndChild(rq_item);
	}
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileCreator::GenerateFontItems()
{
	for (auto *pt : this->fontsList)  // <Font name="0_NexaBold" filename="0_NexaBold.otf" />
	{
		font_item = gfs_document.NewElement("Font");
		font_item->SetAttribute("name", (std::to_string(pt->subFontNr) + "_" + pt->fontName).c_str());
		font_item->SetAttribute("filename", pt->fontFile.string().c_str());
		doc_aftereffects->InsertEndChild(font_item);
	}
	return NoError;
}

ErrorCodesAE AeGfsFileCreator::GenerateEffectItems()
{
	for (auto *pt : this->effectsList)   // <Effect name="Slider Control" matchName="ADBE_Slider_Control" effectCategory="Expression_Controls" installedKey="105553116267494" />
	{
		effect_item = gfs_document.NewElement("Effect");
		effect_item->SetAttribute("name", pt->effectName.c_str());
		effect_item->SetAttribute("matchName", pt->matchName.c_str());
		effect_item->SetAttribute("effectCategory", pt->effectCategory.c_str());
		effect_item->SetAttribute("installedKey", std::to_string(pt->installKey).c_str());
		doc_aftereffects->InsertEndChild(effect_item);
	}
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::PushRenderQueueItem(gfsRqItem *node)
{
	this->rqItems.push_back(node);
	return NoError;
}

ErrorCodesAE AeGfsFileCreator::PushFontNode(gfsFontNode* node_pt)
{
	for (auto *it : fontsList) {
		if (it->fontFile.compare(node_pt->fontFile) == 0) {
			delete node_pt;
			node_pt = nullptr;
			return NullResult;
		}
	}
	fontsList.push_back(node_pt);
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::PushEffectNode(gfsEffectNode* node_pt)
{
	for (auto *it : effectsList) {
		if (it->installKey == node_pt->installKey) {
			delete node_pt;
			node_pt = nullptr;
			return NullResult;
		}
	}
	effectsList.push_back(node_pt);
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::InitGfsFileBuilder(beamerParamsStruct const &theBps)
{
	ClearGfsFileCreator();
	return InitGfsFileCreator(theBps.bp.relGfsFile, theBps.bp.projectFilenameCorrect, theBps.bp.remote_renders_path, theBps.bp.remoteFontsPath, theBps.versionStr);
}

ErrorCodesAE AeGfsFileCreator::InitGfsFileCreator(fs::path const &gfsFilePath, fs::path const &fileName, fs::path const &outPath, fs::path const &fontsDir, A_char const *version)
{
	ErrorCodesAE _ErrorCode = NoError;
	ERROR_AE(SetGfsFilePath(gfsFilePath))
	ERROR_AE(SetOutFileName(fs::path(fileName).replace_extension()))
	ERROR_AE(SetOutFilePath(fs::path(outPath).replace_extension()))
	ERROR_AE(SetFontsDirPath(fontsDir))
	ERROR_AE(SetAeVersionString(version))
	return _ErrorCode;
}
ErrorCodesAE AeGfsFileCreator::ClearGfsFileCreator()
{
	while (!rqItems.empty())
	{
		delete rqItems.back();
		rqItems.pop_back();
	}
	while (!fontsList.empty())
	{
		delete fontsList.back();
		fontsList.pop_back();
	}
	while (!effectsList.empty())
	{
		delete effectsList.back();
		effectsList.pop_back();
	}
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::SetGfsFilePath(fs::path const &gfsFilePath)
{
	gfs_file_path = gfsFilePath;
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::SetOutFileName(fs::path const &fileName)
{
	outFileName = fileName;
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::SetOutFilePath(fs::path const &outPath)
{
	outFilePath = outPath;
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::SetFontsDirPath(fs::path const &fontsDir)
{
	outFontsDir = fontsDir;
	return NoError;
}
ErrorCodesAE AeGfsFileCreator::SetAeVersionString(A_char const *version)
{
	if (version == nullptr)
		return NullPointerResult;
	versionString = version;
	return NoError;
}
