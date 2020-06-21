#include <iostream>
#include <string>
#include <exception>
#include <readline/readline.h>
#include "vector.hpp"
#include "map.hpp"


using namespace std;

void fPrintHelp()
{
	cout << "Positioned commands:\n"
	        "\tTxy : Tries at field xy\n"
	        "\tXxy : Try non-flagged fields around xy\n"
	        "\tFxy : Flags field xy\n"
	        "\t?xy : Marks field xy\n"
	        "\tX-Coordinates are Letters, Y-Coordinates are Numbers.\n"
		"\tThe order does not matter\n"
	        "General commands:\n"
	        "\tR : Restart the Game from the same settings\n"
	        "\tV : Retype the Playfield\n"
	        "\tQ : Quit\n"
	        "\nAll commands and parameters are case insensitive\n";
}

CVector fParsePosition(string input)
{
	if (input.size() < 1)
		throw invalid_argument("#0 at fParsePosition");
	int x = -1, y = -1;
	for (auto it = ++input.cbegin(); it != input.cend(); it++)
	{
		char c = *it;
		if (isalpha(c) && x < 0) // X Coordinate
			x = (int)toupper(c) - 65;
		else if (isdigit(c) && y < 0) // Y Coordinate
			y = (int)c - 48;
	}
	if (x < 0 || y < 0)
		throw runtime_error("Missing or incomplete Position");
	return CVector(x, y);
}

bool fContinueQuestion()
{
	cout << "Do you want to continue with the same Settings? [y/N]";
	char inchar = ' ';
	cin >> inchar;
	return (inchar == 'y' || inchar == 'Y');
}

int main(int argc, char* args[])
{
	int iXSize = 8; // Default values for the playfield
	int iYSize = 8;
	int iMines = 10;
	for (size_t a = 0; a < argc; a++) // CLI-Argument parsing
	{
		char* arg = args[a]; // Get the first char
		if (*arg == '-')
		{
			switch(arg[1]) //Decide based on the second char
			{
			case 'x': // X Size
				a++; // Get the next argument
				if (a >= argc) break; // Sanity check
				iXSize = atoi(args[a]); // Turn it into an Integer
				break;
			case 'y': // Y Size
				a++;
				if (a >= argc) break;
				iYSize = atoi(args[a]);
				break;
			case 'm': // Number of mines
				a++;
				if (a >= argc) break;
				iMines = atoi(args[a]);
				break;
			default: // Error and Exit on unknown argument
				cout << "Unrecognised argument " << arg[1] << "!\n";
				return 1;
				break;
			}
		}
	}

	// Sanity checks for the Parameters
	if (iXSize <= 1 || iXSize > 26)
	{
		cout << "Invalid value for -x: " << iXSize << endl;
		return 1;
	}
	if (iYSize <= 1 || iYSize > 10)
	{
		cout << "Invalid value for -y: " << iYSize << endl;
		return 1;
	}
	if (iMines < 1 || iMines >= iXSize*iYSize)
	{
		cout << "Invalid value for -m: " << iMines << endl;
		return 1;
	}

	cout << "\x1b]2;Termsweeper\x1b\\";

	CMap Map;
	Map.Init(CVector(iXSize, iYSize), iMines); // Initializing Map
	bool bContinueLoop = true;
	Map.printMap();
	while (bContinueLoop)
	{
		Map.printMessages();
		char* line = readline("Please enter command (help with H): ");
		string sCommand(line);
		delete[] line;

		if (sCommand.size() < 1) continue;
		switch (sCommand.at(0)) // Command parsing
		{
		case 'T':
		case 't':
			try
			{
				if (!Map.Try(fParsePosition(sCommand)))
				{
					Map.printMap(true);
					cout << "Game Over!\n";
					if (fContinueQuestion())
					{
						Map.Quit();
						Map.Init(CVector(iXSize, iYSize), iMines);
						Map.printMap();
					} else {
						bContinueLoop = false;
					}
				} else {
					Map.printMap();
				}
			} catch (runtime_error& rte) {
				cout << rte.what() << endl;
				break;
			} catch (exception& e) {
				cout << "Unknown Exception: " << e.what() << endl;
				break;
			}
			break;
		case 'F':
		case 'f':
			try
			{
				Map.Flag(fParsePosition(sCommand));
				Map.printMap();
			} catch (runtime_error& rte) {
				cout << rte.what() << endl;
				break;
			} catch (exception& e) {
				cout << "Unknown Exception: " << e.what() << endl;
				break;
			}
			break;
		case '?':
			try
			{
				Map.Mark(fParsePosition(sCommand));
				Map.printMap();
			} catch (runtime_error& rte) {
				cout << rte.what() << endl;
				break;
			} catch (exception& e) {
				cout << "Unknown Exception: " << e.what() << endl;
				break;
			}
			break;
		case 'X':
		case 'x':
			try
			{
				if (!Map.TryAround(fParsePosition(sCommand)))
				{
					Map.printMap(true);
					cout << "Game Over!\n";
					if (fContinueQuestion())
					{
						Map.Quit();
						Map.Init(CVector(iXSize, iYSize), iMines);
						Map.printMap();
					} else {
						bContinueLoop = false;
					}
				} else {
					Map.printMap();
				}
			} catch (runtime_error& rte) {
				cout << rte.what() << endl;
				break;
			} catch (exception& e) {
				cout << "Unknown Exception: " << e.what() << endl;
				break;
			}
			break;
		case 'Q':
		case 'q':
			bContinueLoop = false;
			break;
		case 'H':
		case 'h':
			fPrintHelp();
			break;
		case 'V':
		case 'v':
			Map.printMap();
			break;
		case 'r':
		case 'R':
			Map.Quit();
			Map.Init(CVector(iXSize, iYSize), iMines);
			Map.printMap();
			break;
		}
		if (Map.GameWon())
		{
			cout << "Game Completed!\n";
			if (fContinueQuestion())
			{
				Map.Quit();
				Map.Init(CVector(iXSize, iYSize), iMines);
				Map.printMap();
			} else {
				break;
			}
		}
	}
	Map.Quit();
	return 0;
}
