#include "AeFileNode.h"
#include "../GF_GlobalClasses.h"

namespace RenderBeamer {
AeFileNode::AeFileNode(bool is_sequence, std::string node_uid, fs::path source_path, std::string mask_base)
	: node_is_sequence(is_sequence)
	, node_files_size(0)
	, node_UID(std::move(node_uid))
	, mask_seq_base(std::move(mask_base))
	, source_root_path(std::move(source_path))
{

}
AeFileNode::~AeFileNode()
{
	while(!file_names.empty())
	{		
		delete file_names.back();
		file_names.pop_back();
	}
}
bool AeFileNode::operator ==(const AeFileNode &right) const
{
	FS_ERROR_CODE(fileError)
	if (!IsNodeSequence() && !right.IsNodeSequence())
	{
		if (fs::equivalent(GetFileFullSourceConstPath(), right.GetFileFullSourceConstPath(), fileError))
			return true;
	}
	else if (IsNodeSequence() && right.IsNodeSequence())
	{
	//	if (GetFilenameCouple()->sourceFileName.compare(right.GetFilenameCouple()->sourceFileName) == 0)
		//{
		if(GetSequenceMaskBase().empty() && right.GetSequenceMaskBase().empty())
		{			
			if(GetFilenamesNumber() == right.GetFilenamesNumber() && GetNodeFilesSize() == right.GetNodeFilesSize())
			{
				if (GetFileNodeUid() == right.GetFileNodeUid())
					return true;
			}
		}
		else if(!GetSequenceMaskBase().empty() && !right.GetSequenceMaskBase().empty() && GetSequenceMaskBase() == right.GetSequenceMaskBase())
		{
			if (fs::equivalent(GetFileSourcePath(), right.GetFileSourcePath(), fileError))
				return true;
		}
	}
	return false;
}

void AeFileNode::PushSourceFilename(FilenameCouple* filename_couple)
{
	std::lock_guard<std::mutex> lock(m);
	if (filename_couple != nullptr)
	{
		fs::path full_path(GetFileSourcePath() / filename_couple->sourceFileName);

		try {
			const fs::file_status full_path_status(status(full_path));

			if (fs::status_known(full_path_status) ? fs::exists(full_path_status) : fs::exists(full_path))
			{
				if (is_symlink(full_path)) {
					full_path = read_symlink(full_path);
					filename_couple->sourceFileName = full_path.filename().string();
					SetFileSourcePath(full_path.parent_path());
				}
				filename_couple->file_size = fs::file_size(full_path);
				filename_couple->file_is_missing = false;
			}
		}
		catch(...) {
			filename_couple->file_is_missing = true;
		}

		fs::path src_to_fix(filename_couple->sourceFileName);
		rbUtilities::pathStringFixIllegal(src_to_fix, false, false);		
		//filename_couple->relinkedFileName = GetFileRelinkPrefix() + filename_couple->sourceFileName;
		
		filename_couple->relinkedFileName = GetFileRelinkPrefix() + src_to_fix.string();
		node_files_size += filename_couple->file_size;
		file_names.push_back(filename_couple);
	}
}

void AeFileNode::PopLastFilenameCouple()
{
	if (!file_names.empty())
	{
		node_files_size -= file_names.back()->file_size;
		delete file_names.back();
		file_names.back() = nullptr;
		file_names.pop_back();
	}
}
AeFileNode::FilenameCouple* AeFileNode::GetFilenameCouple(unsigned long long n) const
{
	if(n < file_names.size())
		return file_names[n];
	return nullptr;
}
unsigned long long AeFileNode::GetFilenamesNumber() const
{
	return file_names.size();
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//		SETTERS 
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
void AeFileNode::SetFileSourcePath(const fs::path &source_path)
{
	source_root_path = source_path;
}
void AeFileNode::SetFileCopyPath(const fs::path &copy_path)
{
	copy_root_path = copy_path;
}
void AeFileNode::SetFileRelinkPath(const fs::path &relink_path)
{
	relink_root_path = relink_path;
}
void AeFileNode::SetFileNodeUid(const std::string &node_uid)
{
	node_UID = node_uid;
}
void AeFileNode::SetSequenceMaskBase(const std::string &mask_sequence)
{
	mask_seq_base = mask_sequence;
}
void AeFileNode::SetMaxFilesCount(unsigned long long n)
{
	if(file_names.capacity() < n)
		file_names.reserve(n);
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//		GETTERS 
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
std::string AeFileNode::GetFileRelinkPrefix() const
{
	return node_UID + "_";
}
const fs::path &AeFileNode::GetFileSourcePath() const  
{
	if (!source_root_path.has_filename())
		return source_root_path;
	source_root_path.filename().remove_filename();
	return source_root_path;
}
const fs::path &AeFileNode::GetFileFullSourcePath(unsigned long long n)
{
	reference_file_full_source_path = GetFileSourcePath().lexically_normal();
	auto *const pt = GetFilenameCouple(n);
	if(pt != nullptr)
		reference_file_full_source_path /= pt->sourceFileName;
	return reference_file_full_source_path;
}
fs::path AeFileNode::GetFileFullSourceConstPath(unsigned long long n) const
{
	auto *const pt = GetFilenameCouple(n);
	return GetFileSourcePath().lexically_normal() / (pt == nullptr ? "" : pt->sourceFileName);
}
const fs::path &AeFileNode::GetFileCopyPath() const
{
	if (copy_root_path.empty() || !copy_root_path.has_filename())
		return copy_root_path;
	copy_root_path.filename().remove_filename();
	return copy_root_path;
}
const fs::path &AeFileNode::GetFileFullCopyPath(unsigned long long n)
{
	reference_file_full_copy_path = GetFileCopyPath().lexically_normal();
	auto *const pt = GetFilenameCouple(n);
	if (pt != nullptr)
		reference_file_full_copy_path /= pt->relinkedFileName;
	return reference_file_full_copy_path;
}
const fs::path &AeFileNode::GetFileRelinkPath() const
{
	if (relink_root_path.empty() || !relink_root_path.has_filename())
		return relink_root_path;
	relink_root_path.filename().remove_filename();
	return relink_root_path;
}
const fs::path &AeFileNode::GetFileFullRelinkPath(unsigned long long n)
{
	reference_file_full_relink_path = GetFileRelinkPath().lexically_normal();
	auto *const pt = GetFilenameCouple(n);
	if (pt != nullptr)
		reference_file_full_relink_path /= pt->relinkedFileName;
	return reference_file_full_relink_path;
}
const std::string &AeFileNode::GetFileNodeUid() const
{
	return node_UID;
}
const std::string &AeFileNode::GetSequenceMaskBase() const
{
	return mask_seq_base;
}
std::string AeFileNode::GetSequenceMaskRelinkedBase() const
{
	if (!GetSequenceMaskBase().empty())
		return GetFileRelinkPrefix() + GetSequenceMaskBase();
	return "";
}
unsigned long long AeFileNode::GetMaxFilesCount() const
{
	return file_names.capacity();
}
uintmax_t AeFileNode::GetNodeFilesSize() const
{
	return node_files_size;
}
bool AeFileNode::IsNodeSequence() const
{
	return node_is_sequence;
}
void AeFileNode::ListNodeMainInfo()
{
	std::cout << "size  ::" << GetNodeFilesSize() << "::  uid::" << GetFileNodeUid() << "::  path::" << GetFileFullSourcePath().string() << std::endl;
	ListNodeFilenames();
}
void AeFileNode::ListNodeFilenames()
{
	for (auto pt : file_names)
	{
		std::cout << "size  ::" << pt->file_size << "::  src::" << pt->sourceFileName << "::  rel::" << pt->relinkedFileName << std::endl;
	}
}
void AeFileNode::ListNodeFullPathFiles() 
{
	long i = 0;
	for (auto pt : file_names)
	{
		std::cout << "size  ::" << pt->file_size << "::  src::" << GetFileFullSourcePath(i) << "::  rel::" << GetFileFullCopyPath(i) << std::endl;
		++i;
	}
}
} // namespace RenderBeamer
