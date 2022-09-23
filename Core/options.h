#pragma once
#include "config.h"

namespace config {
	namespace data {
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
				virtual Windows::Forms::Control^ getControl(bindings^) override;
				static option^ create(optionArgs^);
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
				Generic::List<String^>^ mapChoices();
				virtual String^ getString() override;
				virtual Windows::Forms::Control^ getControl(bindings^) override;
				static option^ create(optionArgs^);
			};

			public ref class text : public option {
			public:
				text(String^, String^);
				String^ value;
				virtual String^ getString() override;
				virtual Windows::Forms::Control^ getControl(bindings^) override;;
				static option^ create(optionArgs^);
			};

			public ref class error : public option {
			public:
				error();
				String^ getString() override;
				virtual Windows::Forms::Control^ getControl(bindings^) override;
				static option^ create(optionArgs^);
			};
		}
	}
}