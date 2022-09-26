#include "config.h"


namespace config {

	JSONParseException::JSONParseException(String^ message) : Exception(message) {
	}

	wchar_t* setEnvPath = nullptr;
	wchar_t* setConfigPath = nullptr;

	void setCharConfig(wchar_t** config, String^ value) {
		(*config) = new wchar_t[value->Length+1];
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

		option::option(String^ id, String^ name) {
			this->id = id;
			this->name = name;
		}
		void option::registerOption(String^ name, option ^ (*createOption)(optionArgs^)) {
			optionRegistryEntry^ entry = gcnew optionRegistryEntry(name, createOption);
			optionRegistry.Add(entry->name, entry);
		}
		option^ option::createOption(String^ name, optionArgs^ args) {
			return (*(optionRegistry[name]->createOption))(args);
		}

		void option::rerender(Object^ caller, System::EventArgs^ args) {
			option::rerenderE(caller, args);
		}

		optionRegistryEntry::optionRegistryEntry(String^ name, option ^ (*createOption)(optionArgs^)) {
			this->name = name;
			this->createOption = createOption;
		}

		option^ scriptlet::getOption(String^ id) {
			for each (option^ opt in this->options) {
				if (opt->id == id) {
					return opt;
				}
			}
			throw gcnew JSONParseException("No option exists with specified ID \""+ id +"\"");
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
				throw gcnew JSONParseException("Built in variable \""+token+"\" does not exist.");
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

	}
}




