#pragma once
#using <mscorlib.dll>

using namespace System;
using namespace System::Collections;
using namespace System::IO;

namespace config {
	
	void setCharConfig(wchar_t**, String^);
	void freeConfig();

	extern wchar_t* setEnvPath;
	extern wchar_t* setConfigPath;
	extern String^ envPath();
	extern String^ configPath();

	namespace data {

		namespace options {
			enum class types {
				error, file, multi
			};
		}

		//typedef Generic::Dictionary<String^, String^> bindings;
		public ref class optionArgs : public Generic::Dictionary<String^, String^>{
		public:
			String^ operator^(String^ key) {
				if (this->ContainsKey(key)) {
					return this[key];
				}
				else {
					return "";
				}
			}
		};
		typedef optionArgs bindings;

		public interface class renderable {
			virtual String^ getString();
		};


		public interface class ShellToken : public renderable
		{

		};

		ref class option;

		public ref class optionRegistryEntry {
		public:
			optionRegistryEntry(String^, option ^ (*)(optionArgs^));
			String^ name;
			option^ (*createOption)(optionArgs^);
		};

		public ref class option : public renderable
		{
		public:
			static event System::EventHandler^ rerenderE;
			
			static void rerender(Object^, System::EventArgs^);
			static Generic::Dictionary<String^,optionRegistryEntry^> optionRegistry;
			static void registerOption(String^, option ^ (*)(optionArgs^));
			static option^ createOption(String^, optionArgs^);
			String^ id;
			String^ name;
			Windows::Forms::Control^ control;
			options::types type;
			virtual String^ getString() = 0;
			virtual Windows::Forms::Control^ getControl(bindings^) = 0;
			option(String^, String^, options::types);
		};


		namespace shell {

			public ref class builtIn : public ShellToken {
			private:
				String^ token;
			public:
				builtIn(String^);
				virtual String^ getString();
			};

			public ref class optionRef : public ShellToken {
			private:
				option^ target;
			public:
				optionRef(option^);
				virtual String^ getString();
			};

			public ref class text : public ShellToken {
			private:
				String^ content;
			public:
				text(String^);
				virtual String^ getString();
			};
		}

		public ref class scriptlet : public renderable {
		public:
			String^ name;
			bool outputFromInput;
			Generic::List<option^> options;
			Generic::List<Generic::List<ShellToken^>^> shell;
			option^ getOption(String^ id);
			virtual String^ getString();
		};

		public ref class config {
		public:
			Generic::List<scriptlet^> scriptlets;
			Generic::List<String^>^ mapScriptlets();
		};
	}

	data::config^ parseConfig(String^ fName);
}

#include "options.h"