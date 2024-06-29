#include <Application.hpp>

// TODO: fix cmake issues with editor make it just use main() and not shitty winmain()
// i cba rn to fix this shit

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include<windows.h>

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
) {
    mist::Application editor("Mist Editor");
    editor.Run();
    return 0;
}
#else
int main(int argc, char** argv) {
    mist::Application editor("Mist Editor");
    editor.Run();
    return 0;
}
#endif