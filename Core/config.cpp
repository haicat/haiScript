#include "config.h"


namespace config {

	wchar_t* setEnvPath = nullptr;
	wchar_t* setConfigPath = nullptr;

	void setCharConfig(wchar_t** config, String^ value) {
		(*config) = new wchar_t[value->Length+1];
		//array<wchar_t>^ vAry = value->ToCharArray();
		for (int i = 0; i < value->Length; i++) {
			(*config)[i] = value[i];
		}
		(*config)[value->Length] = 0;
	}

	void freeConfig() {
		if (setEnvPath) {
			delete setEnvPath;
		}
		if (setConfigPath) {
			delete setConfigPath;
		}
	}

	String^ envPath() {
		if (setEnvPath == nullptr) {
			return (gcnew FileInfo(System::Windows::Forms::Application::StartupPath + "\\env"))->FullName;
		}
		return (gcnew FileInfo(gcnew String(setEnvPath)))->FullName;
	}

	String^ configPath() {
		if (setConfigPath == nullptr) {
			return (gcnew FileInfo(System::Windows::Forms::Application::StartupPath + "\\config.json"))->FullName;
		}
		return (gcnew FileInfo(gcnew String(setConfigPath)))->FullName;
	}

	namespace data {
		Generic::List<String^>^ config::mapScriptlets() {
			Generic::List<String^>^ list = gcnew Generic::List<String^>();
			for each (data::scriptlet^ scriptlet in this->scriptlets) {
				list->Add(scriptlet->name);
			}
			return list;
		}

		option::option(String^ id, String^ name, options::types type) {
			this->id = id;
			this->type = type;
			this->name = name;
		}

		option^ scriptlet::getOption(String^ id) {
			for each (option^ opt in this->options) {
				if (opt->id == id) {
					return opt;
				}
			}
			return nullptr;
		}

		String^ scriptlet::getString() {
			String^ text = "";
			for each (Generic::List<ShellToken^>^ line in this->shell) {
				String^ renderedLine = "";
				for each (ShellToken ^ token in line) {
					renderedLine += token->getString();
				}
				text += renderedLine + "\r\n";
			}
			return text;
		};

		namespace shell {
			builtIn::builtIn(String^ token) {
				this->token = token;
			}
			String^ builtIn::getString() {
				if (this->token == "!env") {
					return envPath();
				}
				throw gcnew System::ArgumentException("Built in variable \""+token+"\" does not exist.");
				return "";
			}

			optionRef::optionRef(option^ target) {
				this->target = target;
			}
			String^ optionRef::getString() {
				return this->target->getString();
			}


			text::text(String^ content) {
				this->content = content;
			}
			String^ text::getString() {
				return content;
			}
		}

		namespace options {
			file::file(String^ id, String^ name) : option(id, name, types::file) {
			}
			file::file(String^ id, String^ name, String^ dir, String^ bind, String^ ext) : option(id, name, types::file) {
				this->dir = dir;
				this->bind = bind;
				this->ext = ext;
			}
			String^ file::getString() {
				if (this->fInfo == nullptr) {
					return "\"\"";
				}
				return "\""+this->fInfo->FullName+"\"";
			}


			multi::choice::choice(String^ name, String^ value) {
				this->name = name;
				this->value = value;
			}
			String^ multi::choice::getString() {
				return this->value;
			}


			multi::multi(String^ id, String^ name) : option(id, name, types::multi) {};
			String^ multi::getString() {
				return this->choices[this->selection]->getString();
			}
			void multi::addChoice(String^ name, String^ value) {
				this->choices.Add(gcnew choice(name,value));
			}
			Generic::List<String^>^ multi::mapChoices() {
				Generic::List<String^>^ list = gcnew Generic::List<String^>();
				for each (data::options::multi::choice ^ choice in this->choices) {
					list->Add(choice->name);
				}
				return list;
			}
			
			error::error(String^ id, String^ name) : option(id, name, types::error) {};
			String^ error::getString() {
				return nullptr;
			}

		}

	}
}




