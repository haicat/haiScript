#include <fstream>
#include "common.h"
#include "config.h"

using namespace System::Runtime::InteropServices;



namespace config {

	Generic::List<String^>^ __lineTokenize(String^ line, Generic::List<String^>^ tokens) {
		if (line == "") {
			return tokens;
		}
		if (!line->Contains("${")) {
			tokens->Add(line);
			return tokens;
		}
		int start = line->IndexOf("${");
		String^ text = line->Substring(0, start);
		
		String^ postStart = line->Substring(start);
		
		if (!postStart->Contains("}")) {
			tokens->Add(line);
			return tokens;
		}

		int end = postStart->IndexOf("}");

		String^ token = postStart->Substring(0, end+1);
		String^ remainder = postStart->Substring(end+1);

		tokens->Add(text);
		tokens->Add(token);

		return __lineTokenize(remainder, tokens);
	}

	Generic::List<String^>^ lineTokenize(String^ line) {
		Generic::List<String^>^ tokens = gcnew Generic::List<String^>();
		return __lineTokenize(line, tokens);
	}

	Generic::List<data::ShellToken^>^ parseShell(json j, data::scriptlet^ scriptlet) {
		if (!j.is_string()) {
			throw gcnew config::JSONParseException("Shell item not of type string.");
		}
		Generic::List<data::ShellToken^>^ shell = gcnew Generic::List<data::ShellToken^>();
		String^ line = jGetKeyS(j);
		Generic::List<String^>^ tokens = lineTokenize(line);
		for each(String^ token in tokens) {
			if (token->StartsWith("${") && token->Contains("}")) {
				String^ tokenRef = token->Substring(2,token->Length-3);
				if (tokenRef->StartsWith("!")) {
					data::shell::builtIn^ ref = gcnew data::shell::builtIn(tokenRef);
					shell->Add(ref);
				}
				else {
					data::shell::optionRef^ ref = gcnew data::shell::optionRef(scriptlet->getOption(tokenRef));
					shell->Add(ref);
				}
				continue;
			}
			shell->Add(gcnew data::shell::text(token));
		}
		return shell;
	}


	data::option^ parseOption(json jOption) {
		data::option^ Option;

		data::optionArgs^ args = gcnew data::optionArgs;

		for (json::iterator it = jOption.begin(); it != jOption.end(); ++it) {
			if (it.value().is_string()) {
				args->Add(gcnew String(it.key().c_str()), jGetKeyS(it.value()));
				continue;
			}
			args->Add(gcnew String(it.key().c_str()), gcnew String(it.value().dump(4).c_str()));
		}

		if (args->ContainsKey("type")) {
			Option = data::option::createOption(args["type"], args);
		}
		else {
			throw gcnew config::JSONParseException("Option does not contain a type.");
		}

		return Option;
	}

	data::config^ parseConfig(String^ fName) {
		IntPtr ip = Marshal::StringToHGlobalAnsi(fName);
		const char* sfName = static_cast<const char*>(ip.ToPointer());


		data::config^ cfg = gcnew data::config();

		FileInfo^ fInfo;
		FileStream^ fStream;

		try {
			fInfo = gcnew FileInfo(fName);
			fStream = fInfo->OpenRead();
		}
		catch (...) {
			throw gcnew config::JSONParseException("Could not open configuration file.");
		}
		

		std::ifstream f(sfName);
		json j;
		try {
			j = json::parse(f);
		}
		catch (json::parse_error e) {
			throw gcnew JSONParseException(gcnew String(e.what()));
		}

		fStream->Close();

		if (!j.contains("scriptlets")) {
			throw gcnew JSONParseException("Top level JSON does not contain scriptlets key.");
		}

		for (auto& iScriptlet : j["scriptlets"].items()) {
			json jScriptlet = iScriptlet.value();
			data::scriptlet^ scriptlet = gcnew data::scriptlet();
			scriptlet->name = jGetKeyS(jScriptlet["name"]);
			scriptlet->outputFromInput = jGetKeyB(jScriptlet["outputFromInput"],"", false);
			
			if (!jScriptlet.contains("options")) {
				throw gcnew JSONParseException("Scriptlet does not contain options key.");
			}

			if (!jScriptlet.contains("shell")) {
				throw gcnew JSONParseException("Scriptlet does not contain shell key.");
			}

			for (auto& iOption : jScriptlet["options"].items()) {
				json jOption = iOption.value();
				data::option^ option = parseOption(jOption);
				scriptlet->options.Add(option);
			}

			for (auto& iShell : jScriptlet["shell"].items()) {
				json jShell = iShell.value();
				scriptlet->shell.Add(parseShell(jShell, scriptlet));
			}

			cfg->scriptlets.Add(scriptlet);
		}

		Marshal::FreeHGlobal(ip);
		return cfg;
	}
}