#pragma once
#include "config.h"
namespace Core {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for UIMain
	/// </summary>
	public ref class UIMain : public System::Windows::Forms::Form
	{
	public:
		config::data::config^ cfg;
		int selectedScriptlet = 0;
		TextBox^ fileReturn = nullptr;
		Collections::Generic::List<String^>^ args;

		void selectScriptlet(int index) {
			optionPanel->Controls->Clear();
			this->selectedScriptlet = index;

			auto scriptlet = cfg->scriptlets[selectedScriptlet];

			config::data::bindings^ binds = gcnew config::data::bindings();

			if (args->Count == 1) {
				binds->Add("input", args[0]);
				binds->Add("output", args[0]);
			}
			else if (args->Count > 1) {
				binds->Add("input", args[0]);
				binds->Add("output", args[1]);
			}

			for each (config::data::option ^ opt in scriptlet->options) {
				Panel^ optPanel = gcnew Panel();
				optPanel->Width = 170;
				optPanel->Height = 24;

				Label^ optLabel = gcnew Label();
				optLabel->Text = opt->name;
				optLabel->Width = 70;

				optPanel->Controls->Add(optLabel);

				Control^ ctl = opt->getControl(binds);

				Panel^ ctlPanel = gcnew Panel();
				ctlPanel->Width = 100;
				ctlPanel->Location = Point(70,0);

				ctlPanel->Controls->Add(ctl);
				optPanel->Controls->Add(ctlPanel);

				optionPanel->Controls->Add(optPanel);
			}

			renderScriptlet();

		}

#pragma region Event Handlers

		void scriptletChanged(Object^ sender, EventArgs^ e) {
			auto cScriptlet = (ComboBox^)sender;
			selectScriptlet(cScriptlet->SelectedIndex);
		}
		System::EventHandler^ scriptletChangedE = gcnew System::EventHandler(this, &Core::UIMain::scriptletChanged);

		void runPressed(Object^ sender, EventArgs^ e) {
			renderAndRun();
		}
		System::EventHandler^ runPressedE = gcnew System::EventHandler(this, &Core::UIMain::runPressed);

		System::Void UIMain_Load(System::Object^ sender, System::EventArgs^ e) {
			loadCFG(config::configPath());
			config::data::option::rerenderE += renderScriptletE;
		}

		void scriptletOutput(Object^ sender, Diagnostics::DataReceivedEventArgs^ e) {
			consoleWrite(e->Data);
		}
		Diagnostics::DataReceivedEventHandler^ scriptletOutputE = gcnew Diagnostics::DataReceivedEventHandler(this, &Core::UIMain::scriptletOutput);

#pragma endregion

#pragma region Console write
		delegate void consoleWriteD(String^, TextBox^);
		void consoleWrite(String^ out) {
			if (consoleOut->InvokeRequired) {
				consoleWriteD^ d = gcnew consoleWriteD(_consoleWrite);
				consoleOut->Invoke(d, gcnew array<Object^>{ out, consoleOut });
				return;
			}
			consoleOut->Text += out + "\r\n";
		}
		static void _consoleWrite(String^ out, TextBox^ to) {
			to->Text += out + "\r\n";
			//consoleWrite(out);
		}
#pragma endregion

		void loadCFG(String^ fName) {
			scriptletSelect->SelectedIndexChanged -= scriptletChangedE;

			cfg = config::parseConfig(fName);

			scriptletSelect->Items->Clear();
			scriptletSelect->Items->AddRange(cfg->mapScriptlets()->ToArray());

			scriptletSelect->SelectedIndex = 0;

			scriptletSelect->SelectedIndexChanged += scriptletChangedE;

			selectScriptlet(0);
		}

		inline String^ renderScriptlet() {
			String^ rendered = cfg->scriptlets[selectedScriptlet]->getString();
			this->consoleIn->Text = rendered;
			return rendered;
		}

		void renderScriptlet(Object^ sender, System::EventArgs^ e) {
			renderScriptlet();
		}
		System::EventHandler^ renderScriptletE = gcnew System::EventHandler(this, &Core::UIMain::renderScriptlet);

		void renderAndRun() {
			String^ envPath = config::envPath();
			Directory::Delete(envPath,true);
			Directory::CreateDirectory(envPath);
			String^ shell = renderScriptlet();
			StreamWriter^ sw = gcnew StreamWriter(envPath + "\\scriptlet.bat");
			sw->Write("@echo off\r\n" + shell);
			sw->Close();

			
			Diagnostics::ProcessStartInfo^ pInfo = gcnew Diagnostics::ProcessStartInfo(envPath + "\\scriptlet.bat");

			pInfo->UseShellExecute = false;
			pInfo->RedirectStandardOutput = true;
			String^ cd = Directory::GetCurrentDirectory();

			pInfo->WorkingDirectory = cd;

			Diagnostics::Process^ p = Diagnostics::Process::Start(pInfo);

			p->OutputDataReceived += scriptletOutputE;
			p->BeginOutputReadLine();
			//p->WaitForExit();
			

		}

		UIMain(void)
		{
			InitializeComponent();

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~UIMain()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::Windows::Forms::TabControl^ tabs;
		System::Windows::Forms::TabPage^ tabRun;
		System::Windows::Forms::TabPage^ tabConfig;
		System::Windows::Forms::SplitContainer^ splitContainer;
		System::Windows::Forms::TextBox^ consoleOut;
		System::Windows::Forms::TextBox^ consoleIn;
		System::Windows::Forms::FlowLayoutPanel^ optionPanel;
		System::Windows::Forms::ComboBox^ scriptletSelect;
		System::Windows::Forms::Button^ runButton;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabs = (gcnew System::Windows::Forms::TabControl());
			this->tabRun = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer = (gcnew System::Windows::Forms::SplitContainer());
			this->scriptletSelect = (gcnew System::Windows::Forms::ComboBox());
			this->runButton = (gcnew System::Windows::Forms::Button());
			this->consoleIn = (gcnew System::Windows::Forms::TextBox());
			this->consoleOut = (gcnew System::Windows::Forms::TextBox());
			this->tabConfig = (gcnew System::Windows::Forms::TabPage());
			this->optionPanel = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->tabs->SuspendLayout();
			this->tabRun->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->BeginInit();
			this->splitContainer->Panel1->SuspendLayout();
			this->splitContainer->Panel2->SuspendLayout();
			this->splitContainer->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabs
			// 
			this->tabs->Controls->Add(this->tabRun);
			this->tabs->Controls->Add(this->tabConfig);
			this->tabs->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabs->Location = System::Drawing::Point(0, 0);
			this->tabs->Name = L"tabs";
			this->tabs->SelectedIndex = 0;
			this->tabs->Size = System::Drawing::Size(742, 380);
			this->tabs->SizeMode = System::Windows::Forms::TabSizeMode::Fixed;
			this->tabs->TabIndex = 0;
			// 
			// tabRun
			// 
			this->tabRun->Controls->Add(this->splitContainer);
			this->tabRun->Location = System::Drawing::Point(4, 22);
			this->tabRun->Name = L"tabRun";
			this->tabRun->Padding = System::Windows::Forms::Padding(3);
			this->tabRun->Size = System::Drawing::Size(734, 354);
			this->tabRun->TabIndex = 0;
			this->tabRun->Text = L"Run";
			this->tabRun->UseVisualStyleBackColor = true;
			// 
			// splitContainer
			// 
			this->splitContainer->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer->Location = System::Drawing::Point(3, 3);
			this->splitContainer->Name = L"splitContainer";
			// 
			// splitContainer.Panel1
			// 
			this->splitContainer->Panel1->Controls->Add(this->optionPanel);
			this->splitContainer->Panel1->Controls->Add(this->scriptletSelect);
			this->splitContainer->Panel1->Controls->Add(this->runButton);
			// 
			// splitContainer.Panel2
			// 
			this->splitContainer->Panel2->Controls->Add(this->consoleIn);
			this->splitContainer->Panel2->Controls->Add(this->consoleOut);
			this->splitContainer->Size = System::Drawing::Size(728, 348);
			this->splitContainer->SplitterDistance = 450;
			this->splitContainer->TabIndex = 0;
			// 
			// scriptletSelect
			// 
			this->scriptletSelect->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->scriptletSelect->FormattingEnabled = true;
			this->scriptletSelect->Location = System::Drawing::Point(5, 324);
			this->scriptletSelect->Name = L"scriptletSelect";
			this->scriptletSelect->Size = System::Drawing::Size(361, 21);
			this->scriptletSelect->TabIndex = 2;
			this->scriptletSelect->DropDownStyle = ComboBoxStyle::DropDownList;
			// 
			// runButton
			// 
			this->runButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->runButton->Location = System::Drawing::Point(372, 322);
			this->runButton->Name = L"runButton";
			this->runButton->Size = System::Drawing::Size(75, 23);
			this->runButton->TabIndex = 1;
			this->runButton->Text = L"Run";
			this->runButton->UseVisualStyleBackColor = true;
			this->runButton->Click += runPressedE;
			// 
			// consoleIn
			// 
			this->consoleIn->AcceptsReturn = true;
			this->consoleIn->AcceptsTab = true;
			this->consoleIn->ReadOnly = true;
			this->consoleIn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->consoleIn->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->consoleIn->Location = System::Drawing::Point(3, 210);
			this->consoleIn->Multiline = true;
			this->consoleIn->Name = L"consoleIn";
			this->consoleIn->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->consoleIn->Size = System::Drawing::Size(268, 134);
			this->consoleIn->TabIndex = 1;
			// 
			// consoleOut
			// 
			this->consoleOut->AcceptsReturn = true;
			this->consoleOut->AcceptsTab = true;
			this->consoleOut->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->consoleOut->Font = (gcnew System::Drawing::Font(L"Consolas", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->consoleOut->Location = System::Drawing::Point(3, 3);
			this->consoleOut->Multiline = true;
			this->consoleOut->Name = L"consoleOut";
			this->consoleOut->ReadOnly = true;
			this->consoleOut->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->consoleOut->Size = System::Drawing::Size(268, 200);
			this->consoleOut->TabIndex = 0;
			// 
			// tabConfig
			// 
			this->tabConfig->Location = System::Drawing::Point(4, 22);
			this->tabConfig->Name = L"tabConfig";
			this->tabConfig->Padding = System::Windows::Forms::Padding(3);
			this->tabConfig->Size = System::Drawing::Size(734, 354);
			this->tabConfig->TabIndex = 1;
			this->tabConfig->Text = L"Config";
			this->tabConfig->UseVisualStyleBackColor = true;
			// 
			// optionPanel
			// 
			this->optionPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->optionPanel->Location = System::Drawing::Point(5, 3);
			this->optionPanel->Name = L"optionPanel";
			this->optionPanel->Size = System::Drawing::Size(442, 313);
			this->optionPanel->TabIndex = 3;
			// 
			// UIMain
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(742, 380);
			this->MinimumSize = System::Drawing::Size(340, 300);
			this->Controls->Add(this->tabs);
			this->Name = L"UIMain";
			this->ShowIcon = false;
			this->Text = L"haiScript";
			this->Load += gcnew System::EventHandler(this, &UIMain::UIMain_Load);
			this->tabs->ResumeLayout(false);
			this->tabRun->ResumeLayout(false);
			this->splitContainer->Panel1->ResumeLayout(false);
			this->splitContainer->Panel2->ResumeLayout(false);
			this->splitContainer->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->EndInit();
			this->splitContainer->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}
