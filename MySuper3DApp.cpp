// MySuper3DApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Game.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")





int main()
{
	Game gameInstance = Game();
	gameInstance.Run();
    std::cout << "Hello World!\n";
}