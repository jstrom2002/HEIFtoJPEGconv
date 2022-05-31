#pragma once
#include "imgui.h"
#include "UI.h"
#include <filesystem>

namespace HEIFtoJPEG
{

	/**
	*  \brief Based upon aiekick's file dialog widget: https://aiekick.github.io/ and the table demo
	*		  from 'imgui_demo.cpp.' It is intended to mirror the Windows file dialog functionality.
	*/
	class CustomFileDialog {
	public:

#define FILEDIALOG_DEFAULT_BUFFER_SIZE 1024 


// Platform-specific directory path marker.
#ifdef _WIN32
#define PATH_SEP '\\'
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#elif defined(LINUX) or defined(APPLE)
#include <sys/types.h>
#include <dirent.h>
#define PATH_SEP '/'
#endif

		// Public member variables.
		bool IsOk = false;						//boolean for whether a file has been selected or not.
		bool m_AnyWindowsHovered = false;		//boolean for whether any sub windows are hovered.
		bool m_CreateDirectoryMode = false;		//boolean for allowing the creation of directories.
		std::filesystem::path m_CurrentPath;		

		// Generic struct to pair file name, type, and other info.	
		struct FileInfoStruct {
			enum class TYPE 
			{
				FILE,
				FOLDER,
				LINK,
				DRIVE
			};

			TYPE type = TYPE::FOLDER;
			std::string name = "";
			std::string displayName = "";
			std::string extension= "";

			void genDisplayNameFromName();

			FileInfoStruct(std::string name_) : name(name_), extension(""), type(TYPE::FOLDER) 
			{
				genDisplayNameFromName();
			}

			FileInfoStruct(std::string name_, std::string display_name) : name(name_), displayName(display_name), extension(""),type(TYPE::FOLDER) {}//Constructor for folders
			FileInfoStruct(std::string name_, std::string extension_, TYPE type_) : name(name_), extension(extension_),type(type_) {}
		};


		/* List of custom font icons
		ICON_IMFDLG_ADD
		ICON_IMFDLG_OK " OK"
		ICON_IMFDLG_CANCEL " Cancel"
		ICON_IMFDLG_RESET
		ICON_IMFDLG_DRIVES
		ICON_IMFDLG_SEARCH
		ICON_IMFDLG_FOLDER
		ICON_IMFDLG_LINK
		ICON_IMFDLG_FILE
		*/

		//From a header Generated with https://github.com/aiekick/ImGuiFontStudio
		//Based on https://github.com/juliettef/IconFontCppHeaders
		const char compressed_image_font_data_base85[4105 + 1] =
			"7])#######s]'=Q'/###O@?>#+lQS%Ql#v#X^@iF$VvM2J[+##I^&##o?T=B8[2JL:^Ee-]br-$IQshF=I&##:_(##G-13C;h^`IImeP&NZwV%M-ppAf-d<B)Z,mS@Ir%cB4pV.<[b2C"
			"%jkc1^N(&Pmh3G4,5LsC7@:1pP`%/Lra^TnqV,-GVFk<8,5<kF(b<ZM6#e>#sLFjI'he<6+RE9%VQv&#Nlptee+^,MPFQ:v-f:?#27Dt-].=GMM%g+MPXZ##0Urt--Aa+MNq;?#Tj<X("
			"9p(d*J`Md*f$IP/111^#Qfq@-w@g;-lVjfL%10/LpuM/LTTs-$1&[w'KSPsIm3.AF7=m<-3[`=-T/]jL(DD5M%IG,M)5T;-g*tSS?A7M.'Auu#$i''#3l:;$'%Wt1pW,/(O`:;&jIdK'"
			"a>-t5&KMEDOE40K)fiQ@Z]1_3N.`&M?RGgLr03MMt(8;q_x.>-bGDiLru,s-lW6iLk]Y&#CQsJMmo('#F/B;-$?Uw-Sr[fLrrH##.GY##2Sl##6`($#u6po3>xL$#B.`$#F:r$#JF.%#"
			"NR@%#r%LhMI%1kL[#h5#;]O.#tVJ=#X4ofL%G;pL7Qr:#:K(7#D.u.#D,r7#OHSnLX;Z6#ftbgL)``pLL<t4#b%R(#db=gL-(m<->X`=-+5T;-<@;=-6%Vp.p-%##0g4eQa&a#$gxIfL"
			"SSe##7*vM($i?X-,BG`a*HWf#Ybf;-?G^##D<7I-Q4An0)]$s$)8[0#K?O&#f>a'#S.`$#_*'u$;)MB#@e%?5*3?/(Zl(?-e:Xe)UZ-)*KpR@tTGS@tIEJYGX._h3&1$YYBm@iug@>iF"
			"=Ol>#ZGhc`h_pu>B[)I-'Y0dMKrsILvLEB#KU3f-#75nu?YWh]tFKjLD01pM5SW$#r>$(#d,>>#c5Gw0mNv)4')'J3Pw?(+&KSx'eEI$$M<T_uE:BO;8t]e$1(L#$vX8Y-OmqNXrKpC#"
			"lSsJ;39Y%'-&e%'?7P&#:^Lh.Zr@/L=]&C1'b.-#hxQ0#_->>#O/Vl-W?$LcG7tx'[@o)(O4:_#-c;^u'@l(5fIV2M3.dW-J$m4+083A'g>4A'po]4=?<NJMBH#W-:?gC&*aqC&EhLs)"
			"fvvU;KYqr$E7rq7:J)E>&lQ##l%>o793dr/h23U-?I3$')t:`/]7Nu7#oa)#HRq/#n2^I$AT@%#>0B:%S:Ls-%/65/#&AA4M7GgLwuYA#vTa@uD;Rc<^^xOS<,Qb%ps^^#i>Mt-C6bOM"
			"ZYBcNwVhw'*qx[ui.suLR(;230F+q$wJT2#kiKS.M7%s$MhGgLN7LkL4%du%LE,-*Hx:?#nEKe?u@BquCEB_-+Md'&4:(B#KjJgM18YCjp9sQNjw/SS>lg<MfVaj$2v^##x@PS.kN%##"
			"AFDZ#5D-W.Ihl.9x_[?TEQCD3EU@lL/RXD#n?an8QI3-v^GUv-_Q3L#;1sB#TekD#3YJ,3VG+G4XX48nw8<cDMa]iuium9)igAE+%]v7/gu%t]:U.g9wCo)*sK#(+M]Oo7hgYDOrbdU)"
			"7xO-)V&dN#3BQG)uP/O45FMo$n2/tu-i(UL+0IQ1p`b'OMk8hGn2R.)PNd*7cTPDOjx3R/DUsl&F;*VCGkG&#u_^>$ZQ=X-mb1-4WqAw'Tw;<%^T3(tf)p^f(*^e$[%ZcaV*:I$J#:qU"
			"*@jM-`)1E9nEa9`KsBp.u3YD#E2O-ZjP^E#+F]$QYFvO/Da14'GtMnS%@,gL7ifbMWFuS7tw^[On98Y-G)G(/fFl%l*anA#UGZW-]L=r)F6jI-9%.Q44?&##$&>uuZgUV$f._'#>0B:%"
			"sYc8/Y^Tv-`fWI)$M'f)-<Tv-Lq[P/qC,c4ETnO(,p^I*>9(,)joVb3(=wsT$P:'7b?uJ(U0O9%l4jiKr3CVdr(X1)nPGMT^,RW%0<,A#/@PD4I2?[CsQj8.acRL(p(n;%Zm21d?R#N#"
			";DOjL'K5%0Sr@/Lw758.c7b$%sZ'u$a8tKPYVd8/vj)gLnCYD#<)]L(<muX$Xd;^uO2Oda8.c1B93dr/U:w$'hUj'0hu;;RofOkLUL$I1C.0u$CGF&#]rgo.(n%Z&QkeNk)k'u$Od]b3"
			"vQ)Wuin1T.G8][#lq8gLg0N;XJ&5R*rNZ'X+H^[#-n`?ucXD.N',[=lj#?Po$8Gs%>^Uu8T[Zv$QV?iLe;^;-9m%B.Mj:,)'j3^#E9w2MF]-lL-k5pLG(^,Mr+p+M%8K/L*VMk=3qY-2"
			"?]J,3D9dTMn2:Z@4=3H#(+_^#Rs).$Uqvw'6T9@MSGOJMbp2^#r+TV-KhQw9@.EM083LG):.i?#ZVd8/pk4D#nGpkLTaic)tBo8%N.7r8=aC_A$_1)-Dc*nTD*H1.tZX`<6$3@$eo[_u"
			">MSIA3`sR[(7SbITYrPMZIM;.g:.o9HvK;.:p0F%),5MV>j-VHBe*=$bn(?$i@5M9`>a%$vAYc2iH%##*c?X-GOFb3J?gf1w[aBQ+I1N(e]YD4$,;g)D8)21x)Qv$t@0+*s<RF45nha'"
			"gvf@#o]r.L$px=#;`*U)tDtM&nbbr/BV4'5/dfr/r%0'5+Y=TT&(^fL)V/+Mdo<c3i33-,+)do/$)wIL;a1e2Qml7&$VT@%Q#G>#]?l(5N2>>#Aj*.)sA'%MZ9E$u:D;=-%W@Y/OR+on"
			"JE&##rgSs-VxIfLl>T%#KSj**i%^F*0]WF3M3(D0K&iO'.m$O'K*b0>q?cr/(F:@-VvwFiQL:cD:;2p/gXfCM+A,gLViVV$Pi[+MW1[##PG<A+q^EM0Ue$##XAic)jm=.3i,2K(=5MB#"
			"'c?X-]p*P(5ZGb%+56J*qSc8/+gB.*kHx6,broF-C3:h0+VxA&W@1,'_8V+M&n$>#OAt=#;4w.L.(j3GS%<G$L&$w/YGNRN9W^o8)2wo&eB%Btr)mx4=mgQt9QLnuZD#73v`q92X;-4#"
			"a('.ZSl68%=wfi'^UIa4k=2A=ab.*4@aH&4?4PcMV^UeVBvHg)q1A-d0J'f)>f0wgPJ,G4_`0i)KQ#]#;X^:/bke[#%X0##$fK#$i4jU5#O0>#&F%`%-x,d%lsoQ<-cn<Ep,97;2<f1#"
			"+kiZq]+HNtKf<puNQ/`=kDn.+wJ^c;=>j[ui'^:/UXKfL5ICp7:,%B5#[#5Ac:OQ0t(4I)v;Tv-faFp8/A^%6=n+D#(Ko4(+IA8%`xJ+*V#[)*O>XjLbU$lLLX>hL(`Uq7W%i)39pVI)"
			"F0gQ&nR;h(nArNX^'+3-IDr<(2bkX$dF.U(c'4e`V*&)?>i`^#]PX1)8wmq7/l&?5sOo)*vBxU%=&Sx(h_=h(uvY.:uiYlSDx(K(OELRJvjpp)b>hTRDwT4A<7c)4>PxM5:uAL(9&?pg"
			"3iI(#Hu*9/$?F78L8$Z$rEo5/,pvC#U7*u7uuU^#q;BO;mv8c$*npR)Q'pM'$TfX#Gbe<$ZG?2L]KdA#e#*^#1]ccaxeBB#=>?5&*8w+)ups+;Tvtti;ktD#c9OA#axJ+*?7%s$DXI5/"
			"CKU:%eQ+,2=C587Rg;E4Z3f.*?ulW-tYqw0`EmS/8f,C#[<;S&mInS%FAov#@P%E+If&hLN`vN'+)U40`a5k'sA;)*RIRF%rs<jLEWOjL$:HErwNw5/*vwA#;nn0#Q*66Q`_r7%xcE#$"
			"+PY8&w@o%u@%jE-JAx/&el68%EQ(,)b[18.vsai0R[$##'G3]-7(.m/J.rv-0S<+3/(H<-DL;=-h::p$.JSfL(dtH$Nin+Mu6m;-rbm(5g3]COCI,hL.>Y5t?A3.$K_%L#fRO1T)fM&5"
			"-Y;(&XI9L5G:6V-99fY-_55f$H5YY#tEWS%(m###*J(v#.c_V$2%@8%6=wo%:UWP&>n82'B0pi'FHPJ(Ja1,)N#ic)R;ID*aJ>vGk#fuBb8ftB)-W]Ab2Q[&OM0$HvP#lEwHr*H6F6=("
			"2:vOE]%KKF`vgC%)p:eGimFF$CHsS&SKu2Cs,omD,WSq)1,C,$W^p?&v@I'I(h)]AG/CEN,B*hFv/5gCtWrTC,I,+%<+V.GoC/vGed+F$p)fUC6:R206R]/GmGKoD.^e@'(9;gDB2?'O"
			";2^iFKWaKOT^*pDm&M]0hjS/C0gkVCFQUV$As+JAA$###";

#define FONT_ICON_BUFFER_NAME_IMFDLG IMFDLG_compressed_data_base85
#define FONT_ICON_BUFFER_SIZE_IMFDLG 0x1009

#define ICON_MIN_IMFDLG 0xf002
#define ICON_MAX_IMFDLG 0xf233

#define ICON_IMFDLG_ADD u8"\uf067"
#define ICON_IMFDLG_CANCEL u8"\uf00d"
#define ICON_IMFDLG_CHEVRON_DOWN u8"\uf078"
#define ICON_IMFDLG_CHEVRON_UP u8"\uf077"
#define ICON_IMFDLG_DRIVES u8"\uf233"
#define ICON_IMFDLG_FILE u8"\uf15b"
#define ICON_IMFDLG_FILE_TYPE_ARCHIVE u8"\uf1c6"
#define ICON_IMFDLG_FILE_TYPE_PIC u8"\uf1c5"
#define ICON_IMFDLG_FILE_TYPE_SOUND u8"\uf1c7"
#define ICON_IMFDLG_FILE_TYPE_VID u8"\uf1c8"
#define ICON_IMFDLG_FOLDER u8"\uf07b"
#define ICON_IMFDLG_FOLDER_OPEN u8"\uf07c"
#define ICON_IMFDLG_ICONS_BIG u8"\uf009"
#define ICON_IMFDLG_ICONS_SMALL u8"\uf00a"
#define ICON_IMFDLG_LINK u8"\uf1c9"
#define ICON_IMFDLG_LIST u8"\uf039"
#define ICON_IMFDLG_LIST_DETAIL u8"\uf03a"
#define ICON_IMFDLG_LIST_ICONS u8"\uf00b"
#define ICON_IMFDLG_OK u8"\uf00c"
#define ICON_IMFDLG_REFRESH u8"\uf021"
#define ICON_IMFDLG_RESET u8"\uf064"
#define ICON_IMFDLG_SAVE u8"\uf0c7"
#define ICON_IMFDLG_SEARCH u8"\uf002"

		/**
		 *  \verbose Widget to display a single line text field that has unlimited bounds.
		 */
		class DirectoryField {
		public:

			DirectoryField(
				std::string label, 
				std::string contents, 
				const ImVec2 size,
				std::vector<FileInfoStruct>* m_FileList,
				std::filesystem::path* m_CurrentPath,
				int flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_EnterReturnsTrue
					| ImGuiInputTextFlags_CallbackResize
			) 
			{
				// For now the size param only controls directory field size, not height.
				ImGui::SetNextItemWidth(size.x);

				// Get path from input box and update current path.
				if (ImGui::InputText(label.c_str(),
					(char*)contents.c_str(), contents.size()+1, flags,
					(ImGuiInputTextCallback)InputTextCallback,
					&contents)) 
				{
					// Remove filepaths that have files at the end (ie 'C:\samples\test.png')
					size_t idx = std::string::npos;
					while ((idx = contents.rfind(".")) != std::string::npos)
					{
						contents = contents.substr(0, idx);

						// Truncate string to the nearest directory slash
						if((idx=contents.rfind("\\")) != std::string::npos)
							contents = contents.substr(0, idx);
						else if ((idx = contents.rfind("/")) != std::string::npos)
							contents = contents.substr(0, idx);
					}

					if (contents.find(":\\") != std::string::npos
						&& std::filesystem::exists(contents)) 
					{
						*m_CurrentPath = contents;
						ui->fileDialogPath = contents;
						m_FileList->clear();
					}
				}
			}

			static int InputTextCallback(ImGuiInputTextCallbackData* data)
			{
				if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
				{
					// Resize string callback
					std::string* str = (std::string*)data->UserData;
					IM_ASSERT(data->Buf == str->c_str());
					str->resize(data->BufTextLen);
					data->Buf = (char*)str->c_str();
					
				}
				return 1;
			}
		};

		/* Load icon font for current font or else icons will instead be invalid chars ('?' characters typically).
		*	This must be run after loading EVERY font that you would like to use to display the dialog icons.
		*	Example:
		*		'ImGuiIO& io = ImGui::GetIO(); (void)io;
		*		ImFont* font_roboto14 = io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 14.0f);
		*		CustomFileDialog::Instance()->runAfterLoadingAFont();' 	*/
		bool runAfterLoadingAFont();

		static CustomFileDialog* Instance()
		{
			static auto* _instance = new CustomFileDialog();
			return _instance;
		}

		void OpenDialog(const std::string& vKey, const char* vName, ImVec2 windowSize_,
			std::string startingDirectory = "",
			ImGuiWindowFlags vFlags = ImGuiWindowFlags_None);

		bool FileDialog(const std::string& vKey, ImGuiWindowFlags vFlags = ImGuiWindowFlags_NoCollapse);

		void CloseDialog(const std::string& vKey)
		{
			if (dlg_key == vKey)
			{
				dlg_key.clear();
				m_ShowDialog = false;
			}
			ui->showFileDialog = false;
		}

		std::string GetFilepathName() 
		{
			std::string  result = m_CurrentPath.string();
			if (result[result.length()-1] != PATH_SEP)
			{
				result += PATH_SEP;
			}			
			result += filenameBuffer;
			return result;
		}

		inline void resetFilepath()
		{
			strcpy(filenameBuffer, "");
		}

		static bool fileComparator(FileInfoStruct a, FileInfoStruct b);

	protected:

		CustomFileDialog()
		{
			IsOk = false;
			m_ShowDialog = false;
			m_CreateDirectoryMode = false;
			dlg_filters = "";
		}

		~CustomFileDialog() = default;


	private:

		void clearCurrentFiles() {
			m_FileList.clear();
		}

		void resetCurrentFiles() {
			clearCurrentFiles();
			refreshCurrentFiles();
		}

		void getFileDisplayNames();
		void refreshCurrentFiles();
		void setFileSelection(bool& res);

		std::string getDirectory(std::string str) {
			if (str.find("/") != std::string::npos)
				str = str.substr(0, str.rfind("/") + 1);
			if (str.find("\\") != std::string::npos)
				str = str.substr(0, str.rfind("\\") + 1);
			return str;
		}

		std::string getLastFolderOfDirectory(std::string str) {
			if (str.find("/") != std::string::npos)
				str = str.substr(str.rfind("/") + 1);
			if (str.find("\\") != std::string::npos)
				str = str.substr(str.rfind("\\") + 1);
			return str;
		}

		std::string getExtension(std::string str) {
			if (str.find(".") == std::string::npos) { return ""; }
			str = str.substr(str.rfind("."));
			return str;
		}

		std::string getFilename(std::string str) {
			while (str.rfind("/") != std::string::npos && str.length() > 0) {
				str = str.substr(str.rfind("/") + 1);
			}
			while (str.rfind("\\") != std::string::npos && str.length() > 0) {
				str = str.substr(str.rfind("\\") + 1);
			}
			return str;
		}

		void goUpOneDirectoryFolder();
		void getCurrentFiles();
		std::string ComputeFileSize(std::string pathToCheck);

		std::string dlg_key="";
		std::string dlg_name="";
		const char* dlg_filters{};
		std::string dlg_path="";
		std::string dlg_defaultFileName="";
		std::string dlg_defaultExt="";
		std::string searchTag="";
		size_t dlg_countSelectionMax = 1; // 0 for infinite

		std::vector<FileInfoStruct> m_FileList;
		std::string m_SelectedFileName="";
		std::string m_SelectedExt="";
		std::vector<std::string> m_CurrentPath_Decomposition;
		std::string m_Name="";
		bool m_ShowDialog = false;
		bool m_ShowDrives = false;
		std::string m_LastSelectedFileName=""; // for shift multi selection
		ImVec2 windowSize = ImVec2(0, 0);

		char filenameBuffer[FILEDIALOG_DEFAULT_BUFFER_SIZE];// input text buffer.

		static inline std::string s_fs_root = std::string(1u, PATH_SEP);
	};
}