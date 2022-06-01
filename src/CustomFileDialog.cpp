#pragma once
#include "CustomFileDialog.h"
#include "UI.h"
#include "imstb_truetype.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imconfig.h"
#include "heif_converter.h"

namespace HEIFtoJPEG {
		bool CustomFileDialog::runAfterLoadingAFont() {
			static const ImWchar icons_ranges[] = { ICON_MIN_IMFDLG, ICON_MAX_IMFDLG, 0 };
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			auto& io = ImGui::GetIO();
			io.Fonts->AddFontFromMemoryCompressedBase85TTF(
				compressed_image_font_data_base85,
				15.0f,
				&icons_config, icons_ranges
			);
			return true;
		}
		void CustomFileDialog::goUpOneDirectoryFolder(){
			if (m_CurrentPath.has_parent_path()){
				// Handle case where this current folder is the root for a drive.
				if (m_CurrentPath.parent_path() == m_CurrentPath) {
					std::string dirs[26]{
						"A:\\", "B:\\", "C:\\", "D:\\", "E:\\", "F:\\", "G:\\", "H:\\",
						"I:\\", "J:\\", "K:\\", "L:\\", "M:\\", "N:\\", "O:\\", "P:\\",
						"Q:\\", "R:\\", "S:\\", "T:\\", "U:\\", "V:\\", "W:\\", "X:\\",
						"Y:\\", "Z:\\"
					};
					m_CurrentPath = "";
					clearCurrentFiles();
					m_FileList.push_back(FileInfoStruct("..", ".."));
					for (int i = 0; i < 26; ++i) {
						dirs[i][2] = (char)PATH_SEP;
						if (std::filesystem::is_directory(dirs[i])) {
							std::string disp_string = std::string(ICON_IMFDLG_FOLDER) + " " + dirs[i];
							m_FileList.push_back(FileInfoStruct(dirs[i], disp_string));
						}
					}
				}

				// Else, just go up to the next highest directory.
				else 
				{
					m_CurrentPath = m_CurrentPath.parent_path();
					resetCurrentFiles();
				}
			}
		}

		void CustomFileDialog::setFileSelection(bool& res)
		{
			res = true;
			m_ShowDialog = false;
			if (m_SelectedFileName.length() > 0 && m_SelectedFileNames.size() == 0)
				m_SelectedFileNames.push_back(m_SelectedFileName);
		}

		void CustomFileDialog::OpenDialog(const std::string& vKey, const char* vName, ImVec2 windowSize_,
			std::string startingDirectory, ImGuiWindowFlags vFlags) 
		{
			if (m_ShowDialog) // if already opened, quit
				return;

			// Clear all stored files in file list, so that they will reload every time the
			// dialog opens.
			clearCurrentFiles();
			resetFilepath();

			dlg_key = vKey;
			dlg_name = std::string(vName);
			if (startingDirectory.length()) {
				// Must remove '\\' or '/' from the end of a directory path, else this will cause an error.
				if (startingDirectory[startingDirectory.length() - 1] == PATH_SEP) {
					startingDirectory = startingDirectory.substr(0, startingDirectory.length() - 1);
				}
				dlg_path = startingDirectory;
				m_CurrentPath = startingDirectory;
			}
			else {
				dlg_path = std::filesystem::current_path().string();
				m_CurrentPath = dlg_path;
			}
			dlg_countSelectionMax = 1;
			dlg_defaultExt = "";
			windowSize = windowSize_;

			m_ShowDialog = true;
		}

		bool CustomFileDialog::FileDialog(const std::string& vKey, ImGuiWindowFlags vFlags) {
			bool res = false;

			static ImGuiIO& io = ImGui::GetIO();

			if (m_ShowDialog && dlg_key == vKey)
			{
				std::string name = dlg_name + "##" + dlg_key;
				if (m_Name != name)
				{
					clearCurrentFiles();
					m_CurrentPath_Decomposition.clear();
					m_SelectedExt.clear();
				}
				IsOk = false;
				ImGui::SetNextWindowSize(ui->fileDialogSize, ImGuiCond_Always);
				if(ImGui::Begin(std::string(ICON_IMFDLG_FOLDER_OPEN + m_CurrentPath.string()).c_str(),
					&m_ShowDialog, vFlags | ImGuiWindowFlags_NoScrollbar))
				{
					// Handle ctrl+A behavior.
					if (io.KeyCtrl && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					{
						m_SelectedFileNames.clear();
						for (auto fn : m_FileList)
							m_SelectedFileNames.push_back(fn.name);
					}

					m_Name = name;
					//if (ImGui::IsWindowHovered())
					//	isPopupHovered = true;
					m_AnyWindowsHovered |= ImGui::IsWindowHovered();
					if (dlg_path.empty()) dlg_path = ".";

					// Calc desired window/element sizes
					windowSize = ui->fileDialogSize;
					const ImVec2 fileDialogParentSz = ImVec2(windowSize[0] * 0.90f, windowSize[1] * 0.65f);
					const ImVec2 fileDialogDirectoryFieldSz = ImVec2(fileDialogParentSz.x * 0.90f, 15.0f);
					const ImVec2 fileDialogFileEntrySz = ImVec2(fileDialogDirectoryFieldSz.x * 0.95f, 8.0f);
					static const ImVec2 fileDialogButtonSz = ImVec2(15.f, 15.f);

					// 'Home' button.
					if (ImGui::ImageButton(ui->homeID, fileDialogButtonSz))
					{
						this->m_CurrentPath = std::filesystem::current_path();
						resetCurrentFiles();
						ImGui::End();
						return false;
					}

					// 'Up' button.
					ImGui::SameLine();
					if (ImGui::ImageButton(ui->upArrowID, fileDialogButtonSz))
					{
						goUpOneDirectoryFolder();
						ImGui::End();
						return false;
					}

					// Directory input field for displaying/typing addressses.
					ImGui::SameLine();
					DirectoryField textf(std::string("##directory path window"), m_CurrentPath.string(),
						fileDialogDirectoryFieldSz, &m_FileList, &m_CurrentPath);

					// Display all local files.
					ImGui::BeginChild("##file dialog child window", fileDialogParentSz,
						ImGuiWindowFlags_AlwaysVerticalScrollbar);
					{
						//if (ImGui::IsWindowHovered())
						//	eng->isPopupHovered = true;

						// Make sure file list is populated.
						refreshCurrentFiles();

						bool isSelected = false;
						for (int row = 0; row < m_FileList.size(); row++)
						{
							isSelected = std::find(m_SelectedFileNames.begin(), m_SelectedFileNames.end(), m_FileList[row].name) != m_SelectedFileNames.end();//m_FileList[row].name == m_SelectedFileName;

							if (ImGui::Selectable(m_FileList[row].displayName.c_str(), &isSelected, ImGuiSelectableFlags_AllowDoubleClick, fileDialogFileEntrySz))
							{
								// Show currently selected filename.
								m_SelectedFileName = m_FileList[row].name;
								
								if (io.KeyCtrl)
								{
									auto it = std::find(m_SelectedFileNames.begin(), m_SelectedFileNames.end(), m_FileList[row].name);
									if (it != m_SelectedFileNames.end())
									{
										m_SelectedFileNames.erase(it);
									}
									else
										m_SelectedFileNames.push_back(m_FileList[row].name);
								}
								else
								{
									m_SelectedFileNames.clear();
									m_SelectedFileNames.push_back(m_FileList[row].name);
								}

								strcpy(filenameBuffer, (char*)m_FileList[row].name.c_str());

								if (ImGui::IsMouseDoubleClicked(0))
								{
									m_SelectedFileNames.clear();
									m_SelectedFileNames.push_back(m_SelectedFileName);

									// If selecting the '..' string, go up a folder in the directory tree.
									if (m_SelectedFileName == "..") {
										if (m_CurrentPath.has_parent_path()) {
											goUpOneDirectoryFolder();
											ImGui::EndChild();
											ImGui::End();

											return false;//change to true to allow folder selction
										}
									}

									// If selecting a directory, make it the current directory.
									else if (std::filesystem::is_directory(m_CurrentPath.string() + PATH_SEP + m_SelectedFileName)) {
										m_CurrentPath = std::filesystem::path(m_CurrentPath.string() + PATH_SEP + m_SelectedFileName);
										resetCurrentFiles();
										ImGui::EndChild();
										ImGui::End();
										return false;//change to true to allow folder selction
									}
									// Handle case where selection is a root drive directory.
									else if (m_CurrentPath == "" && std::filesystem::is_directory(m_SelectedFileName)) {
										m_CurrentPath = m_SelectedFileName;
										ImGui::EndChild();
										ImGui::End();
										return false;//change to true to allow folder selction
									}
									// If selecting a file, select this file and close the dialog.
									else {
										setFileSelection(res);
										IsOk = true;
										clearCurrentFiles();
										ImGui::EndChild();
										ImGui::End();

										return true;
									}
								}
							}
						}
					}
					ImGui::EndChild();					


					// Show input text line.
					ImGui::Text("Filename:");
					ImGui::SetNextItemWidth(fileDialogParentSz.x * 0.8f);
					ImGui::SameLine(); 
					ImGui::InputText("##file dialog filename selector", filenameBuffer, FILEDIALOG_DEFAULT_BUFFER_SIZE);

					// Show response buttons.
					ImGui::SameLine();
					if (ImGui::Button("OK##ok custom file dialog"))
					{
						if (std::filesystem::is_regular_file(m_CurrentPath.string() + PATH_SEP + filenameBuffer))
						{
							setFileSelection(res);
							IsOk = true;
						}
						else if (std::filesystem::is_directory(m_CurrentPath.string() + PATH_SEP + m_SelectedFileName))
						{							
							m_CurrentPath = std::filesystem::path(m_CurrentPath.string() + PATH_SEP + m_SelectedFileName);
							resetCurrentFiles();
							res = false;
						}
						else
						{
							//ErrorMessageBox("ERROR! Please select a valid file.");
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel##cancel custom file dialog"))
					{
						setFileSelection(res);
						IsOk = false;
					}
				}

				ImGui::End();
			}

			return res;
		}

		std::string trim(std::string str) {//removes spaces in front/back of string
			while (str[str.length() - 1] == ' ') { str.pop_back(); }
			while (str[0] == ' ') { str = str.substr(1); }
			return str;
		}

		void CustomFileDialog::FileInfoStruct::genDisplayNameFromName() {
			std::string file_name_str = "";
			switch (this->type) {
			case FileInfoStruct::TYPE::FILE:
				file_name_str = ICON_IMFDLG_FILE;
				file_name_str.append(" ");
				break;
			case FileInfoStruct::TYPE::FOLDER:
				file_name_str = ICON_IMFDLG_FOLDER;
				file_name_str.append(" ");
				break;
			}

			file_name_str.append(this->name);

			if (this->type == FileInfoStruct::TYPE::FILE) {
				file_name_str.append(" (" + CustomFileDialog::Instance()->ComputeFileSize(
					CustomFileDialog::Instance()->m_CurrentPath.string() +
					PATH_SEP + this->name) + ")");
			}

			this->displayName = file_name_str;
		}

		void CustomFileDialog::getFileDisplayNames() {
			
			// Format directory path as necessary.
			std::string sep = " ";
			sep[0] = PATH_SEP;
			m_CurrentPath = replaceString(m_CurrentPath.string(), "\\\\", sep);
			m_CurrentPath = replaceString(m_CurrentPath.string(), "//", sep);

			// Get current directory's file sizes if necessary.
			for (int i = 0; i < m_FileList.size(); ++i) 			
				m_FileList[i].genDisplayNameFromName();			
		}

		bool CustomFileDialog::fileComparator(FileInfoStruct a, FileInfoStruct b) 
		{
			if (a.type == FileInfoStruct::TYPE::FOLDER && b.type != FileInfoStruct::TYPE::FOLDER)
				return true;
			else if (a.type != FileInfoStruct::TYPE::FOLDER && b.type == FileInfoStruct::TYPE::FOLDER)
				return false;
			else
				return a.name < b.name;
		}

		void CustomFileDialog::refreshCurrentFiles() {
			if (!m_FileList.size()) 
			{
				getCurrentFiles();
				getFileDisplayNames();

				// Sort alphabetically, and by folder first.
				std::sort(m_FileList.begin(), m_FileList.end(), fileComparator);

				// Insert the 'parent directory' folder at index = 0 for all file string arrays.
				m_FileList.insert(m_FileList.begin(), FileInfoStruct("..", std::string(ICON_IMFDLG_FOLDER) + " .."));
			}
		}

		void CustomFileDialog::getCurrentFiles() {

			// Get all other files in current path.
			for (auto& p : std::filesystem::directory_iterator(m_CurrentPath,
				std::filesystem::directory_options::skip_permission_denied
			)) {
				try 
				{
					if (std::filesystem::is_directory(p.path())) 
					{	// File is actually a directory.
						std::string folderNm = p.path().string();
						m_FileList.push_back(FileInfoStruct(getLastFolderOfDirectory(folderNm)));
					}
					else 
					{	
						std::string fileExt = getExtension(p.path().string());
						std::string filename = getFilename(p.path().string());

						m_FileList.push_back(FileInfoStruct(filename, fileExt, FileInfoStruct::TYPE::FILE));
					}
				}
				catch (std::exception e1) {
					// NOTE: Due to some issues with folder pathfinding, it's necessary to catch exceptions here.
					// Mostly this seems to be due to invalid file naming or restriction of access, so exception detailss
					// here aren't necessarily useful.
					
					//ErrorMessageBox(e1.what());
				}
			}

			// Remove invalid entries.
			for (int i = 0; i < m_FileList.size(); ++i) {
				if (m_FileList[i].name == "" && m_FileList[i].type == FileInfoStruct::TYPE::FOLDER) {
					m_FileList.erase(m_FileList.begin() + i);
					i--;
				}
			}
		}

		std::string CustomFileDialog::ComputeFileSize(std::string pathToCheck)
		{
			uintmax_t filesize = 0;
			if (std::filesystem::exists(pathToCheck) &&
				std::filesystem::is_regular_file(pathToCheck))
			{
				auto err = std::error_code{};
				filesize = std::filesystem::file_size(std::filesystem::path(pathToCheck), err);
				if (filesize != static_cast<uintmax_t>(-1));
			}
			if (!filesize)
				return "";

			uintmax_t bytes = filesize;

			// Add string for byte value.
			if (bytes > 1000) {
				bytes /= 1000;
				if (bytes > 1000) {
					bytes /= 1000;
					if (bytes > 1000) {
						bytes /= 1000;
						if (bytes >= 1000) {
							return std::to_string(bytes) + " TB";
						}
						else {
							return std::to_string(bytes) + " GB";
						}
					}
					else {
						return std::to_string(bytes) + " MB";
					}
				}
				else {
					return std::to_string(bytes) + " KB";
				}
			}
			else {
				return std::to_string(bytes) + " B";
			}

		}
}