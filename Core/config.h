#pragma once
#using <mscorlib.dll>

using namespace System;
using namespace System::Collections;
using namespace System::IO;

namespace config {
	
	/// <summary>
	/// set a native global config variable
	/// </summary>
	void setCharConfig(wchar_t**, String^);
	
	/// <summary>
	/// frees up native global config variables
	/// </summary>
	void freeConfig();

	//environment path where the ${!env} token points to for intermediate file usage
	extern wchar_t* setEnvPath;
	//file path for config.json
	extern wchar_t* setConfigPath;

	/// <summary>
	/// gets the environment path. defaults to <app directory>\env if not set
	/// </summary>
	extern String^ envPath();
	
	/// <summary>
	/// gets the config.json location. defaults to <app directory>\config.json if not set
	/// </summary>
	extern String^ configPath();

	namespace data {

		/// <summary>
		/// container for arguments passed to options. use operator^<key> to safely grab values
		/// arguments are each json value in the option structure.
		/// for arguments that are strings, they are stored directly. all other types are serialized as json strings
		/// </summary>
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
		/// <summary>
		/// container for bindings passed to options. use operator^<key> to safely grab values
		/// </summary>
		typedef optionArgs bindings;

		public interface class renderable {
			virtual String^ getString();
		};

		public interface class ShellToken : public renderable{};

		ref class option;

		/// <summary>
		/// entry in the option registry that pairs the option name with a creation function
		/// </summary>
		public ref class optionRegistryEntry {
		public:
			optionRegistryEntry(String^, option ^ (*)(optionArgs^));
			String^ name;
			option^ (*createOption)(optionArgs^);
		};

		/// <summary>
		/// base class for options
		/// </summary>
		public ref class option : public renderable
		{
		public:
			/// <summary>
			/// you shouldnt need to touch this directly, see option::rerender instead
			/// </summary>
			static event System::EventHandler^ rerenderE;
			
			/// <summary>
			/// rerender the selected scriptlet. options should call this whenever their data is changed
			/// </summary>
			static void rerender(Object^, System::EventArgs^);
			/// <summary>
			/// you shouldnt need to touch this directly, see option::registerOption instead
			/// </summary>
			static Generic::Dictionary<String^,optionRegistryEntry^> optionRegistry;
			/// <summary>
			/// adds an option to the registery given a name and a creation function for the option.
			/// there should be a call for this for each option put into main.cpp
			/// </summary>
			static void registerOption(String^, option ^ (*)(optionArgs^));
			/// <summary>
			/// creates an option in the registry given a name and option arguments
			/// </summary>
			static option^ createOption(String^, optionArgs^);
			/// <summary>
			/// the option's id
			/// </summary>
			String^ id;
			/// <summary>
			/// the option's name
			/// </summary>
			String^ name;
			/// <summary>
			/// the main control that represents the option
			/// </summary>
			Windows::Forms::Control^ control;
			virtual String^ getString() = 0;
			/// <summary>
			/// generate the control for this option
			/// </summary>
			virtual Windows::Forms::Control^ getControl(bindings^) = 0;
			option(String^, String^);
		};


		namespace shell {

			/// <summary>
			/// built in shell token. accessed as ${!tokenName}
			/// </summary>
			public ref class builtIn : public ShellToken {
			private:
				String^ token;
			public:
				builtIn(String^);
				virtual String^ getString();
			};

			/// <summary>
			/// token that refers to an option value. accessed as ${optionID}
			/// </summary>
			public ref class optionRef : public ShellToken {
			private:
				option^ target;
			public:
				optionRef(option^);
				virtual String^ getString();
			};

			/// <summary>
			/// plain text "token"
			/// </summary>
			public ref class text : public ShellToken {
			private:
				String^ content;
			public:
				text(String^);
				virtual String^ getString();
			};
		}

		/// <summary>
		/// a single scriptlet
		/// </summary>
		public ref class scriptlet : public renderable {
		public:
			String^ name;
			bool outputFromInput;
			Generic::List<option^> options;
			Generic::List<Generic::List<ShellToken^>^> shell;
			option^ getOption(String^ id);
			virtual String^ getString();
		};

		/// <summary>
		/// the top level data structure for config.json
		/// </summary>
		public ref class config {
		public:
			Generic::List<scriptlet^> scriptlets;
			Generic::List<String^>^ mapScriptlets();
		};
	}

	/// <summary>
	/// parse a file into a config object
	/// </summary>
	data::config^ parseConfig(String^ fName);
}

#include "options.h"