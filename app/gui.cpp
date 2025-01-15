#include "gui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		WS_EX_LAYERED,  // Keep the window layered for transparency
		"class001",
		windowName,
		WS_POPUP,  // Borderless window
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	// Set transparency to make the window invisible but still interactive
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), 0, LWA_COLORKEY);

	// Remove the always-on-top feature, by not using SetWindowPos
	// SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);  // REMOVE this line

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}
void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();
	ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Caveat-VariableFont_wght.ttf", 30.0f);
	globals::iconFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\teevtodolist1.ttf", 30.0f);
	//ImFont* defaultFont = io.Fonts->AddFontFromFileTTF("app\\fonts\\Caveat-VariableFont_wght.ttf", 30.0f);
	//globals::iconFont = io.Fonts->AddFontFromFileTTF("app\\fonts\\teevtodolist1.ttf", 30.0f);
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
void gui::applyStyles() {
	ImVec4 transparent = ImVec4(0.0, 0.0, 0.0, 0.0);
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 20;
	style.FrameRounding = 15;
	style.Colors[ImGuiCol_WindowBg] = ImVec4(
		255.0f / 255.0f,
		192.0f / 255.0f,
		203.0f / 255.0f,
		1.0f
	);
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(
		100.0f / 255.0f,
		78.0f / 255.0f,
		82.0f / 255.0f,
		1.0f
	);
	//button styling
	style.Colors[ImGuiCol_Button] = transparent;
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(
		10.0f / 255.0f,
		10.0f / 255.0f,
		10.0f / 255.0f,
		25.0f / 255.0f
	);;
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(
		10.0f / 255.0f,
		10.0f / 255.0f,
		10.0f / 255.0f,
		35.0f / 255.0f
	);;

	//scroll styling
	style.Colors[ImGuiCol_ScrollbarBg] = transparent;
	style.Colors[ImGuiCol_ScrollbarGrab] = transparent;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = transparent;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = transparent;

}
void gui::taskList() noexcept
{
	std::vector<std::string> allTasks = globals::tasks;
	allTasks.insert(
		allTasks.end(),
		std::make_move_iterator(globals::tasksCompleted.begin()),
		std::make_move_iterator(globals::tasksCompleted.end())
	);//combine all tasks for iteration purposes
	bool isNotEmpty = false;
	globals::tasks = FileSys::loadFile("\\teevToDoList\\tasks.txt");
	globals::tasksCompleted = FileSys::loadFile("\\teevToDoList\\complete.txt");

	for (size_t index = 0; index < allTasks.size(); ++index) {
		const std::string& task = allTasks[index];

		//checks if the current task is checked as completed
		bool isCompleted = std::find(globals::tasksCompleted.begin(), globals::tasksCompleted.end(), task) != globals::tasksCompleted.end();

		std::string buttonLabel;
		//diffrent char's based on task status
		if (isCompleted) { buttonLabel = "-"; }
		else { buttonLabel = "="; }
		buttonLabel += "###" + std::to_string(index);

		ImGui::PushFont(globals::iconFont); //render button char's as icons
		if (ImGui::Button(buttonLabel.c_str(), ImVec2(40, 40))) {//change the state of a task
			if (isCompleted) {
				globals::tasksCompleted.erase(remove(globals::tasksCompleted.begin(), globals::tasksCompleted.end(), task), globals::tasksCompleted.end());
				globals::tasks.push_back(task);
			}
			else {
				globals::tasks.erase(remove(globals::tasks.begin(), globals::tasks.end(), task), globals::tasks.end());
				globals::tasksCompleted.push_back(task);
			}

			FileSys::saveFile(globals::tasks, "\\teevToDoList\\tasks.txt");
			FileSys::saveFile(globals::tasksCompleted, "\\teevToDoList\\complete.txt");
		}
		ImGui::PopFont();//return to default font

		ImGui::SameLine();//render the text next to the button
		if (isCompleted) {
			//create a button with strikethrough text using a modified ImGui:;Button method
			if(ImGui::StrikeThroughButton((task + "###Completed" + std::to_string(index)).c_str())) {
				//delete the complete task once it has been clicked on
				globals::tasksCompleted.erase(remove(globals::tasksCompleted.begin(), globals::tasksCompleted.end(), task), globals::tasksCompleted.end());;
				FileSys::saveFile(globals::tasks, "\\teevToDoList\\tasks.txt");
				FileSys::saveFile(globals::tasksCompleted, "\\teevToDoList\\complete.txt");
			}
		}
		else {
			ImGui::Text("%s", task.c_str());
		}
		ImGui::PushFont(globals::iconFont);

		//render the add task button aligned to the right top corner of the window, if there are tasks
		if (index == 0) {
			ImGui::SameLine(ImGui::GetWindowWidth() - 60);
			if (ImGui::Button("+"))
				globals::screen = 1;
			isNotEmpty = true;
		}
		ImGui::PopFont();

	}
	ImGui::PushFont(globals::iconFont);
	//render the add task button if there are no tasks
	if (!isNotEmpty) {
		ImGui::SameLine(ImGui::GetWindowWidth() - 60);
		if (ImGui::Button("+"))
			globals::screen = 1;
		isNotEmpty = true;
	}
	ImGui::PopFont();
}
void gui::addTaskScreen() noexcept
{
	if (ImGui::Button("cancel"))
		globals::screen = 0;
	ImGui::InputText("##taskName", &globals::taskName);
	ImGui::SameLine();
	ImGui::PushFont(globals::iconFont);
	if (ImGui::Button("+")) {
		globals::tasks.push_back(globals::taskName);
		FileSys::saveFile(globals::tasks, "\\teevToDoList\\tasks.txt");
		FileSys::saveFile(globals::tasksCompleted, "\\teevToDoList\\complete.txt");
		globals::taskName = "";
		globals::screen = 0;
	}
	ImGui::PopFont();
}
void gui::Render() noexcept
{
	gui::applyStyles();

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"teev's todo list",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar
	);
	//get user's appdata folder
	char* buffer = nullptr;
	size_t bufferSize = 0;
	if (_dupenv_s(&buffer, &bufferSize, "APPDATA") == 0 && buffer != nullptr) {
		globals::appData = std::string(buffer);
		free(buffer);
	}
	else { globals::screen = -1; }

	switch (globals::screen)
	{
	case 0:
		gui::taskList();
		break;
	case 1:
		gui::addTaskScreen();
		break;
	case -1:
		ImGui::Text("could not gett APPDATA! error: -1");
		break;
	default:
		ImGui::Text("an unknown error has occured");
		break;
	}

	ImGui::End();
}