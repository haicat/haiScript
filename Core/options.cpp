#include "common.h"
#include "options.h"

using namespace System::Runtime::InteropServices;

using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace config {
	namespace data {
		namespace options {
#pragma region file
			void saveFileOk(Object^, System::ComponentModel::CancelEventArgs^);
			void openFileOk(Object^, System::ComponentModel::CancelEventArgs^);

			ref class glob {
			public:
				static TextBox^ fileReturn = nullptr;
				static SaveFileDialog^ saveFileDialog = gcnew System::Windows::Forms::SaveFileDialog();
				static OpenFileDialog^ openFileDialog = gcnew System::Windows::Forms::OpenFileDialog();
				static glob() {
					openFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(&openFileOk);
					openFileDialog->CheckFileExists = true;
					openFileDialog->CheckPathExists = true;

					saveFileDialog->FileOk += gcnew System::ComponentModel::CancelEventHandler(&saveFileOk);
					saveFileDialog->AddExtension = false;
					saveFileDialog->CheckPathExists = true;
				}
			};

			file::file(String^ id, String^ name) : option(id, name) {
			}
			file::file(String^ id, String^ name, String^ dir, String^ bind, String^ ext) : option(id, name) {
				this->dir = dir;
				this->bind = bind;
				this->ext = ext;

			}
			String^ file::getString() {
				if (this->fInfo == nullptr) {
					return "\"\"";
				}
				return "\"" + this->fInfo->FullName + "\"";
			}

			

			option^ file::create(optionArgs^ args) {
				return gcnew file(args^"id", args^"name", args^"dir", args^"bind", args^"ext");
			};

			void fileChanged(Object^ sender, EventArgs^ e) {
				auto cFile = (TextBox^)sender;
				auto oFile = (file^)(cFile->Tag);
				try {
					oFile->fInfo = gcnew FileInfo(cFile->Text);
				}
				catch (...) {
					oFile->fInfo = nullptr;
				}
				option::rerender(sender, e);
			}

			void openFileOk(Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
				if (glob::fileReturn == nullptr) {
					return;
				}
				glob::fileReturn->Text = glob::openFileDialog->FileName;
			}

			void saveFileOk(Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
				if (glob::fileReturn == nullptr) {
					return;
				}
				glob::fileReturn->Text = glob::saveFileDialog->FileName;
			}

			void fileClick(Object^ sender, EventArgs^ e) {
				auto cFile = (Button^)sender;
				auto oFile = (file^)(cFile->Tag);
				auto textCtl = (TextBox^)oFile->control;

				FileDialog^ diag = glob::openFileDialog;
				if (oFile->dir == "output") {
					diag = glob::saveFileDialog;
				}
				
				diag->AddExtension = oFile->ext != "";
				diag->DefaultExt = oFile->ext;
				if (oFile->fInfo) {
					diag->FileName = oFile->fInfo->Name;
					diag->InitialDirectory = oFile->fInfo->DirectoryName;
				}
				

				glob::fileReturn = textCtl;

				diag->ShowDialog();
			}

			Windows::Forms::Control^ file::getControl(bindings^ binds){

				Panel^ filePanel = gcnew Panel();
				filePanel->Width = 100;

				TextBox^ fileOpt = gcnew TextBox();
				Button^ fileButton = gcnew Button();

				fileOpt->Name = this->id;
				fileOpt->Width = 75;
				fileOpt->Tag = this;

				fileOpt->TextChanged += gcnew System::EventHandler(&fileChanged);
				this->control = fileOpt;

				fileButton->Name = this->id;
				fileButton->Width = 25;
				fileButton->Location = Point(75, -1);
				fileButton->Text = "...";
				fileButton->Tag = this;
				fileButton->Click += gcnew System::EventHandler(&fileClick);
				

				if (this->bind != nullptr && this->bind != "" && binds->ContainsKey(this->bind)) {

					String^ ext = ((this->ext != nullptr) ? this->ext : "");
					ext = ext->Trim();
					if ((!ext->StartsWith(".")) && ext != "") {
						ext = "." + ext;
					}

					String^ fname = binds[this->bind];

					if (this->bind == "output" ) {
						fname += ext;
					}
					
					fileOpt->Text = fname;
				}

				filePanel->Controls->Add(fileOpt);
				filePanel->Controls->Add(fileButton);

				return filePanel;
			}
#pragma endregion

#pragma region multi
			multi::choice::choice(String^ name, String^ value) {
				this->name = name;
				this->value = value;
			}
			String^ multi::choice::getString() {
				return this->value;
			}
			option^ multi::create(optionArgs^ args) {
				multi^ opt = gcnew multi(args["id"], args["name"]);

				String^ mChoices = args["choices"];
				IntPtr ip = Marshal::StringToHGlobalAnsi(mChoices);

				const char* sChoices = static_cast<const char*>(ip.ToPointer());
				
				json choices = json::parse(sChoices);
				for (auto& iChoice : choices.items()) {
					json jChoice = iChoice.value();
					String^ name = jGetKeyS(jChoice["name"]);
					String^ value = jGetKeyS(jChoice["value"]);
					opt->addChoice(name, value);
				}
				Marshal::FreeHGlobal(ip);
				return opt;
			};

			multi::multi(String^ id, String^ name) : option(id, name) {};
			String^ multi::getString() {
				return this->choices[this->selection]->getString();
			}
			void multi::addChoice(String^ name, String^ value) {
				this->choices.Add(gcnew choice(name, value));
			}
			Generic::List<String^>^ multi::mapChoices() {
				Generic::List<String^>^ list = gcnew Generic::List<String^>();
				for each (data::options::multi::choice ^ choice in this->choices) {
					list->Add(choice->name);
				}
				return list;
			}

			void multiChanged(Object^ sender, EventArgs^ e) {
				auto cMulti = (ComboBox^)sender;
				auto oMulti = (multi^)(cMulti->Tag);
				if (oMulti == nullptr) { return; }
				oMulti->selection = cMulti->SelectedIndex;
				option::rerender(sender, e);
				return;
			}
			Control^ multi::getControl(bindings^ binds) {
				ComboBox^ multiOpt = gcnew ComboBox();
				multiOpt->Tag = this;
				multiOpt->Name = this->id;
				multiOpt->Width = 100;
				//multiOpt->Location = Point(70, 0);
				multiOpt->DropDownStyle = ComboBoxStyle::DropDownList;
				multiOpt->Items->AddRange(this->mapChoices()->ToArray());
				multiOpt->SelectedIndexChanged += gcnew System::EventHandler(&multiChanged);
				multiOpt->SelectedIndex = 0;

				return multiOpt;
			}
#pragma endregion

#pragma region text
			text::text(String^ id, String^ name) : option(id, name) {};

			option^ text::create(optionArgs^ args) {
				text^ opt = gcnew text(args["id"], args["name"]);
				return opt;
			};

			String^ text::getString() {
				return this->value;
			}

			void textChanged(Object^ sender, EventArgs^ e) {
				auto cText = (TextBox^)sender;
				auto oText = (text^)(cText->Tag);
				if (oText == nullptr) { return; }
				oText->value = cText->Text;
				option::rerender(sender, e);
				return;
			}

			Control^ text::getControl(bindings^ binds) {
				TextBox^ textOpt = gcnew TextBox();
				textOpt->Tag = this;
				textOpt->Name = this->id;
				textOpt->Width = 100;
				textOpt->TextChanged += gcnew System::EventHandler(&textChanged);
				return textOpt;
			}
#pragma endregion

#pragma region error
			error::error() : option("", "") {};
			String^ error::getString() {
				return nullptr;
			}

			Control^ error::getControl(bindings^ binds) {
				auto c = gcnew Windows::Forms::Label();
				c->Text = "<ERROR!!>";
				return c;
			}

			option^ error::create(optionArgs^) {
				return gcnew error();
			};

#pragma endregion

		}
	}
}