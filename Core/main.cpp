#include "UIMain.h"
#include "config.h"

using namespace System;

using namespace System::Windows::Forms;



void processFlag(String^ flag, String^ value) {
    if (flag == "env" && value != nullptr) {
        config::setCharConfig(&config::setEnvPath, (gcnew FileInfo(value))->FullName);
    }
    if (flag == "config" && value != nullptr) {
        config::setCharConfig(&config::setConfigPath, (gcnew FileInfo(value))->FullName);
    }
}

[STAThread]
int main(array<String^>^ args)

{

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

    config::freeConfig();

    return 0;
}

