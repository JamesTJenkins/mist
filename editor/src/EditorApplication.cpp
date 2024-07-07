#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <Application.hpp>
#include "EditorLayer.hpp"

// TODO: fix cmake issues with editor make it just use main() and not shitty winmain()
// i cba rn to fix this shit

void Run() {
    mist::Application editor("Mist Editor");
    editor.PushLayer(new mistEditor::EditorLayer());
    editor.Run();
}

#if defined(_WIN32)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    Run();
    return 0;
}
#else
int main() {
    Run();
    return 0;
}
#endif