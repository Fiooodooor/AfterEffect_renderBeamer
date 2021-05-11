#ifndef AEFILENODE_HPP
#define AEFILENODE_HPP

#include <iostream>
#include <vector>
#include "../GF_GlobalTypes.h"
#include "tinyXml2.h"

namespace RenderBeamer {

class AeFileNode
{
public:
	struct FilenameCouple
	{
		FilenameCouple()
				: file_is_missing(true), file_size(0), xmlFileReference(nullptr), sourceFileName(""), relinkedFileName("") {}
		FilenameCouple(bool is_missing, uintmax_t size, tinyxml2::XMLElement *xmlReference, std::string source_name)
				: file_is_missing(is_missing), file_size(size), xmlFileReference(xmlReference), sourceFileName(std::move(source_name)), relinkedFileName("") {}

		bool file_is_missing;
		uintmax_t file_size;
		tinyxml2::XMLElement *xmlFileReference;
		std::string sourceFileName;	
		std::string relinkedFileName;
	};

	AeFileNode() = delete;
	AeFileNode(const AeFileNode &) = delete;
	AeFileNode(AeFileNode &&) = delete;
	
	AeFileNode(bool is_sequence, std::string node_uid, fs::path source_path, std::string mask_base="");
	~AeFileNode();

	bool operator ==(const AeFileNode &right) const;
	void PushSourceFilename(FilenameCouple* filename_couple);
	void PopLastFilenameCouple();
	FilenameCouple* GetFilenameCouple(unsigned long long n = 0) const;
	unsigned long long GetFilenamesNumber() const;
	
	void SetFileSourcePath(const fs::path &source_path);
	void SetFileCopyPath(const fs::path &copy_path);
	void SetFileRelinkPath(const fs::path &relink_path);
	void SetFileNodeUid(const std::string &node_uid);
	void SetSequenceMaskBase(const std::string &mask_sequence);
	void SetMaxFilesCount(unsigned long long n);

	const fs::path &GetFileSourcePath() const;
	const fs::path &GetFileFullSourcePath(unsigned long long n=0);
	fs::path GetFileFullSourceConstPath(unsigned long long n=0) const;
	const fs::path &GetFileCopyPath() const;
	const fs::path &GetFileFullCopyPath(unsigned long long n=0);
	const fs::path &GetFileRelinkPath() const;
	const fs::path &GetFileFullRelinkPath(unsigned long long n=0);
	const std::string &GetFileNodeUid() const;
	const std::string &GetSequenceMaskBase() const;
	std::string GetSequenceMaskRelinkedBase() const;
	unsigned long long GetMaxFilesCount() const;	
	uintmax_t GetNodeFilesSize() const;

	bool IsNodeSequence() const;
	void ListNodeMainInfo();
	void ListNodeFilenames();
	void ListNodeFullPathFiles();
	static bool FileExtensionCheck(const fs::path& path_to_check, const fs::path& extension);
	std::mutex m;
	
protected:
	bool node_is_sequence;
	uintmax_t node_files_size;
	std::string node_UID;
	std::string mask_seq_base;
	std::vector<FilenameCouple*> file_names;
	fs::path source_root_path;
	fs::path copy_root_path;
	fs::path relink_root_path;
	
	fs::path reference_file_full_source_path, reference_file_full_copy_path, reference_file_full_relink_path;

	std::string GetFileRelinkPrefix() const;

};
} // namespace RenderBeamer
#endif
