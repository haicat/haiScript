#include "UIMain.h"
#include "config.h"

using namespace System;

using namespace System::Windows::Forms;

using namespace config;


void processFlag(String^ flag, String^ value) {
    if (flag == "env" && value != nullptr) {
        setCharConfig(&setEnvPath, (gcnew FileInfo(value))->FullName);
    }
    if (flag == "config" && value != nullptr) {
        setCharConfig(&setConfigPath, (gcnew FileInfo(value))->FullName);
    }
}

[STAThread]
int main(array<String^>^ args)

{

    data::option::registerOption("file",data::options::file::create);
    data::option::registerOption("multi", data::options::multi::create);

    Application::EnableVisualStyles();

    Application::SetCompatibleTextRenderingDefault(false);

    Core::UIMain form;

    Collections::Generic::List<String^> lArgs(args);

    while (lArgs.Count > 0) {
        if (!lArgs[0]->StartsWith("--")) {
            break;
        }
        String^ arg = lArgs[0];
        lArgs.RemoveAt(0);
        arg = arg->Substring(2);
        int ePos = arg->IndexOf("=");
        String^ val = nullptr;
        if (ePos != -1) {
            val = arg->Substring(ePos+1);
            arg = arg->Substring(0, ePos);
        }
        processFlag(arg, val);
    }

    form.args = %lArgs;

    Application::Run(% form);

    freeConfig();

    return 0;
}

