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

		public interface class renderable {
			virtual String^ getString();
		};


		public interface class ShellToken : public renderable
		{

		};

		public ref class option : public renderable
		{
		public:
			String^ id;
			String^ name;
			Object^ control;
			options::types type;
			virtual String^ getString() = 0;
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
		namespace options {
			public ref class file : public option {
			public:
				FileInfo^ fInfo;
				String^ dir;
				String^ bind;
				String^ ext;
				file(String^, String^);
				file(String^, String^, String^, String^, String^);
				virtual String^ getString() override;
			};

			public ref class multi : public option {
			public:
				ref class choice : public renderable {
				private:
					String^ value;
				public:
					String^ name;
					choice(String^, String^);
					virtual String^ getString();
				};
				multi(String^, String^);
				Generic::List<choice^> choices;
				unsigned int selection;
				void addChoice(String^, String^);
				virtual String^ getString() override;
				Generic::List<String^>^ mapChoices();
			};

			public ref class error : public option {
			public:
				error(String^, String^ id);
				String^ getString() override;
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