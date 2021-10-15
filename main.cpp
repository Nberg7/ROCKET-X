#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>

//Used to create stages
class Stage
{
	public:
		//Attributes
		double sF, sISP, sM0, sMF, sA, sTWR, sDV, sT, sG;
	int stageNumber;
	std::string StageName;
	//Functions
	void set_values(double, double, double, double, double);
	void create_File(std::string, int);
	void remove_File(std::string, int);
};

//Used to set the values for a stage
void Stage::set_values(double M0, double MF, double ISP, double T, double G)
{
	sF = M0 - MF;
	sISP = ISP;
	sM0 = M0;
	sMF = MF;
	sA = T / MF;
	sTWR = sA / G;
	sDV = 9.80665 *ISP* log(M0 / MF);;
	sT = T;
	sG = G;
}

//Used to put the stage in dynamic permanent storage
void Stage::create_File(std::string SN, int code)
{
	StageName = SN;
	stageNumber = code;
	auto codeS = std::to_string(code);
	std::ofstream outfile
	{
		StageName + codeS + ".txt" };
	outfile << StageName << std::endl;
	outfile << stageNumber << std::endl;
	outfile << sF << std::endl;
	outfile << sISP << std::endl;
	outfile << sM0 << std::endl;
	outfile << sMF << std::endl;
	outfile << sA << std::endl;
	outfile << sTWR << std::endl;
	outfile << sDV << std::endl;
	outfile << sT << std::endl;
	outfile << sG << std::endl;

	std::fstream fileNamePermanentStorage;
	fileNamePermanentStorage.open("FileName Permanent Storage.txt", std::ios::app);
	fileNamePermanentStorage << StageName << stageNumber << ".txt\n";

}

//This will delete stages for clean-up
void Stage::remove_File(std::string SN, int code)
{
	auto codeS = std::to_string(code);
	if (remove((SN + codeS + ".txt").c_str()) != 0)
		perror("Error deleting file");
	else
		puts("File successfully deleted");
}

class Rocket
{
	public:
		//Attributes
		std::string rocketName;
	int rocketNumber;
	std::vector<Stage> stageList;
	//Functions
	void returnValues(std::string, int);
	void reCalculate();
};

//Used for stages, this can read a specific line in a file (NOT MADE BY ME)
std::fstream &GotoLine(std::fstream &file, unsigned int num)
{
	file.seekg(std::ios::beg);
	for (int i = 0; i < num - 1; ++i)
	{
		file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	return file;
}

//This checks if a file is empty
bool is_empty(std::ifstream &pFile)
{
	return pFile.peek() != EOF;
}

//This is updateCin. It does the same as a regular cin, but it give the option to re-use a value from permanent storage
double UpdatedCin(std::string input_name, std::string variable_name)
{
	double Outputvalue;
	double fileContents;
	bool choicebool;

	std::ifstream variablePermanentStorageIF;
	variablePermanentStorageIF.open(input_name, std::ios:: in);
	if (variablePermanentStorageIF.peek() != EOF)
	{
		variablePermanentStorageIF >> fileContents;
		std::cout << "You already have inputed a value for " << variable_name << " . Would you like to use: " << fileContents << " again? 1 for yes, 0 for no.\n";
		std::cin >> choicebool;
		if (!choicebool)
		{
			std::cout << "You have opted to re-enter the value\n";
			std::cin >> Outputvalue;
		}
		else
		{
			std::cout << "You have opted to re-use the value\n";
			Outputvalue = fileContents;
		}
	}
	else
	{
		std::cin >> Outputvalue;
	}

	variablePermanentStorageIF.close();
	return Outputvalue;
}

//This is RecordInfo. It puts the value of any variable into permanent storage 
void RecordInfo(double value, std::string input_name)
{
	std::string ValueToStorage;
	bool choicebool;
	double x;
	std::ofstream variablePermanentStorageOF;
	variablePermanentStorageOF.open(input_name, std::ios::out);
	if (!variablePermanentStorageOF)
	{
		std::cout << "Unable to open file";
		exit(1);	// terminate with error
	}

	ValueToStorage = std::to_string(value);
	variablePermanentStorageOF << ValueToStorage << std::endl;
	variablePermanentStorageOF.close();
}

//Used to remove speciic strings from a .txt file. This is to remove a duplicate from FileName Permanent Storage
void erase_line(std::string fileName)
{
	std::string line, stringtobeSearched = fileName;
	std::ifstream inFile("FileName Permanent Storage.txt");
	std::ofstream outFile("output.txt");
	if (inFile)
	{
		while (getline(inFile, line, '\n'))
		{
			//if the line read is not same as string searched for then write it into the output.txt file
			if (line != stringtobeSearched)
			{
				outFile << line << "\n";
			}
		}
	}
	else
	{
		std::cout << "File could not be read" << std::endl;
	}

	remove("FileName Permanent Storage.txt");
	rename("output.txt", "FileName Permanent Storage.txt");
}

int main()
{
	int choice;
	int choice2;
	int choiceBackup;
	int number_of_lines = 0;
	int stageCount = 0;

	std::string stringChoice;
	std::string line;

	double doubeChoice;
	float floatChoice[10];

	bool NotgoToMainMenu = true;
	bool redoLoop = true;
	bool boolChoice;
	bool repeatStage = true;

	std::vector<Stage> stageVector;
	std::vector<Rocket> rocketVector;

	std::fstream FileNames;
	FileNames.open("FileName Permanent Storage.txt");

	std::fstream RocketNames;
	RocketNames.open("RocketName Permanent Storage.txt");

	Rocket RocketObject;
	Stage stageObject;

	double G0, G, F, FLLS, ISP, M0, MF, CW, AEV;	//G0 is standard gravity (9.80665 m/s/s), G is current gravity (m/s/s), F is fuel (l), FLLS is fuel lost (l/s), ISP is specific impulse (s), M0 is wet mass (t), MF is dry mass (t), CW is current weight (t), and AEV is average effective velocity (m/s) 
	double MoF, M1LF, FLKGS, EEV, A, TWR, DV;	//Mof is Mass of Fuel (KG), M1LF is Mass of 1 liter of fuel (T), FLKGS is fuel lost (KG/S), EEV is effective exhaust velocity (M/S), A is acceleration (M/S/S), TWR is thrust to weight ratio, FLLS is fuel lost (L/s), and DV is delta V (M/S)
	double FAAN, AP, PAN, T;	//FAAN is flow area at nozzle (m2), AP is ambient pressure (kPa), PAN is pressure at nozzle (kPa), and T is Thrust (KN), 

	//goes through filename permanent storage, opens the files, and then creates objects 
	//for each one, so the vector is always accurate with the stages created

	std::string variablelist[20] = { "G0",
		"G",
		"F",
		"FLLS",
		"ISP",
		"M0",
		"MF",
		"CW",
		"AEV",
		"MoF",
		"M1LF",
		"FLKGS",
		"EEV",
		"A",
		"TWR",
		"DV",
		"FAAN",
		"AP",
		"PAN",
		"T" };

	while (std::getline(FileNames, line))
	{
		++number_of_lines;
	}

	FileNames.clear();
	FileNames.seekg(0);
	for (int k = 0; k < number_of_lines; k++)
	{
		GotoLine(FileNames, k + 1);
		std::getline(FileNames, line);
		std::ifstream StageFiles;
		StageFiles.open(line);
		FileNames.clear();
		FileNames.seekg(0);
		std::getline(StageFiles, line);
		stageObject.StageName = line;
		std::getline(StageFiles, line);
		stageObject.stageNumber = stoi(line);
		// sF
		std::getline(StageFiles, line);
		stageObject.sF = stoi(line);
		// sIsp
		std::getline(StageFiles, line);
		stageObject.sISP = stoi(line);
		// sM0
		std::getline(StageFiles, line);
		stageObject.sM0 = stoi(line);
		// sMF
		std::getline(StageFiles, line);
		stageObject.sMF = stoi(line);
		// sA
		std::getline(StageFiles, line);
		stageObject.sA = stoi(line);
		// sTWR
		std::getline(StageFiles, line);
		stageObject.sTWR = stoi(line);
		// sDV
		std::getline(StageFiles, line);
		stageObject.sDV = stoi(line);
		// sG
		std::getline(StageFiles, line);
		stageObject.sG = stoi(line);
		// sT
		std::getline(StageFiles, line);
		stageObject.sT = stoi(line);

		stageVector.push_back(stageObject);
	}

	RocketNames.clear();
	RocketNames.seekg(0);

	while (std::getline(RocketNames, line))
	{
		++number_of_lines;
	}

	RocketNames.clear();
	RocketNames.seekg(0);
	for (int k = 0; k < number_of_lines; k++)
	{
		GotoLine(RocketNames, k + 1);
		std::getline(RocketNames, line);
		std::ifstream RocketFiles;
		RocketFiles.open(line);
		RocketNames.clear();
		RocketNames.seekg(0);
		std::getline(RocketFiles, line);
		RocketObject.rocketName = line;
		std::getline(RocketFiles, line);
		//RocketObject.rocketNumber = stoi(line);
		while (std::getline(RocketFiles, line))
		{
			//RocketObject.push_back(line);
		}

		rocketVector.push_back(RocketObject);
	}

	while (true)
	{
		// To reset the choice to quit or go back to main menu
		NotgoToMainMenu = true;
		std::cout << "\033[2J\033[0;0H";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
		std::cout << "Welcome to ROCKET-X!\n";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
		std::cout << "This calculator is still in it's early form, so please forgive any lacking features.\n";
		std::cout << "\nThis calculator has 2 views currently. 1 is calculations, and 2 is stage view. Please enter either 1 or 2 to choose a workspace to open. Enter 3 to quit.\n";
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

		std::cin >> choice;
		if (choice == 1)
		{
			std::cout << "\nYou have chosen to use the calculator. To begin, please choose an option in the list provided.\n";
			std::cout << "1) calculate ∆v\n";
			std::cout << "2) calculate Mass of fuel (Tons)\n";
			std::cout << "3) calculate Mass of 1 liter of fuel (T)\n";
			std::cout << "4) calculate Fuel Lost (KG/S)\n";
			std::cout << "5) calculate Exhaust Velocity (M/S)\n";
			std::cout << "6) calculate Aceleration (M/S/S)\n";
			std::cout << "7) calculate TWR (Thrust to weight ratio)\n";
			std::cout << "8) calculate ISP or Specific Impulse (Seconds)\n";
			std::cout << "9) calculate Fuel lost (L/S)\n";
			std::cout << "10) calculate Thrust (KN)\n\n";
			std::cout << "11) View all my values\n";
			std::cout << "12) Info\n";
			std::cout << "13) Exit back to main menu\n";
			std::cout << "14) Exit ROCKET-X\n";

			std::cin >> choice;

			switch (choice)
			{
				case 1:
					{
						std::cout << "Which method would you like to use?\n1: Standard of Gravity, Specific Impulse, Wet mass, Dry mass\n";
						std::cout << "2: Effective Exhaust Velocity, Wet mass, Dry mass\n";
						std::cin >> choice;
						if (choice == 1)
						{
							std::cout << "You will need to enter Standard of Gravity (m/s/s), Specific Impulse (S), Wet Mass (t), and Dry mass (t).\n";
							std::cout << "Start with Gravity (m/s/s): ";
							G0 = UpdatedCin("G0 PermanentStorage", "Gravity (m/s/s)");
							std::cout << std::endl << std::endl;
							std::cout << "Now enter Specific Impulse (S): ";
							ISP = UpdatedCin("ISP PermanentStorage", "Specific Impulse (S)");
							std::cout << std::endl << std::endl;
							std::cout << "Now enter Wet Mass (t): ";
							M0 = UpdatedCin("M0 PermanentStorage", "Wet Mass (t)");
							std::cout << std::endl << std::endl;
							std::cout << "Now enter Dry Mass (t): ";
							MF = UpdatedCin("MF PermanentStorage", " Dry Mass (t)");
							std::cout << std::endl << std::endl;
							DV = G0 *ISP* log(M0 / MF);
							std::cout << "Your Delta V is a total of: " << DV << "M/S.\n";
							RecordInfo(G0, "G0 PermanentStorage");
							RecordInfo(ISP, "ISP PermanentStorage");
							RecordInfo(M0, "M0 PermanentStorage");
							RecordInfo(MF, "MF PermanentStorage");
							RecordInfo(DV, "DV PermanentStorage");
						}
						else if (choice == 2)
						{
							std::cout << "You will need to enter Effective Exhaust Velocity (m/s), Wet Mass (t), and Dry mass (t).\n";
							std::cout << "Start with Effective Exhaust Velociity (m/s): ";
							EEV = UpdatedCin("EEV PermanentStorage", "Effective Exhaust Velociity (m/s)");
							std::cout << std::endl << std::endl;
							std::cout << "Now enter Wet Mass (t): ";
							M0 = UpdatedCin("M0 PermanentStorage", "Wet Mass (t)");
							std::cout << std::endl << std::endl;
							std::cout << "Now enter Dry Mass (t): ";
							MF = UpdatedCin("MF PermanentStorage", " Dry Mass (t)");
							std::cout << std::endl << std::endl;
							DV = EEV* log(M0 / MF);
							std::cout << "Your Delta V is a total of: " << DV << "M/S.\n";
							RecordInfo(EEV, "EEV PermanentStorage");
							RecordInfo(M0, "M0 PermanentStorage");
							RecordInfo(MF, "MF PermanentStorage");
							RecordInfo(MF, "DV PermanentStorage");
						}

						break;
					}

				case 2:
					{
						std::cout << "You will need to enter Wet Mass (t), and Dry Mass (t)\n";
						std::cout << "Start with Wet Mass (t): ";
						M0 = UpdatedCin("M0 PermanentStorage", "Wet Mass (t)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Dry Mass (t): ";
						MF = UpdatedCin("MF PermanentStorage", "Dry Mass (t)");
						std::cout << std::endl << std::endl;
						MoF = M0 - MF;
						std::cout << "The total mass of your fuel is: " << MoF << " Tons.\n";
						RecordInfo(M0, "M0 PermanentStorage");
						RecordInfo(MF, "MF PermanentStorage");
						RecordInfo(MoF, "MoF PermanentStorage");
						break;
					}

				case 3:
					{
						std::cout << "You will need to enter Mass of fuel (t), and Fuel (l)\n";
						std::cout << "Start with Mass of fuel (t): ";
						MoF = UpdatedCin("MoF PermanentStorage", "Mass of fuel (t)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Fuel (l): ";
						F = UpdatedCin("F PermanentStorage", "Fuel (l)");
						std::cout << std::endl << std::endl;
						M1LF = MoF / F;
						M1LF *= 1000;
						std::cout << "The mass of your fuel per liter is: " << M1LF << " KG.\n";
						RecordInfo(MoF, "MoF PermanentStorage");
						RecordInfo(F, "F PermanentStorage");
						RecordInfo(M1LF, "M1LF PermanentStorage");
						break;
					}

				case 4:
					{
						std::cout << "You will need to enter Mass of 1 liter of fuel (KG), and Fuel Lost (L/S)\n";
						std::cout << "Start with Mass of 1 liter of fuel (KG): ";
						M1LF = UpdatedCin("M1LF PermanentStorage", "Mass of 1 liter of fuel (KG)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Fuel Lost (L/S): ";
						FLLS = UpdatedCin("FLLS PermanentStorage", "Fuel Lost (L/S)");
						std::cout << std::endl << std::endl;
						FLKGS = M1LF * FLLS;
						std::cout << "The amount of fuel lost per second is: " << FLKGS << " KG.\n";
						RecordInfo(M1LF, "M1LF PermanentStorage");
						RecordInfo(FLLS, "FLLS PermanentStorage");
						RecordInfo(FLKGS, "FLKGS PermanentStorage");
						break;
					}

				case 5:
					{
						std::cout << "You will need to enter Thrust (KN), and Fuel lost (KG/s).\n";
						std::cout << "Start with Thrust (KN): ";
						T = UpdatedCin("T PermanentStorage", "Thrust (KN)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Fuel Lost (KG/s): ";
						FLKGS = UpdatedCin("FLKGS PermanentStorage", "Fuel lost (KG/s)");
						std::cout << std::endl << std::endl;
						EEV = T / (FLKGS / 1000);
						std::cout << "Your effective exhaust velocity is " << EEV << "M/S.\n";
						RecordInfo(T, "T PermanentStorage");
						RecordInfo(FLKGS, "FLKGS PermanentStorage");
						RecordInfo(EEV, "EEV PermanentStorage");
						break;
					}

				case 6:
					{
						std::cout << "You will need to enter Thrust (KN), Current Weight (t)\n";
						std::cout << "Start with Thrust (KN): ";
						T = UpdatedCin("T PermanentStorage", "Thrust (KN)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Current Weight (t): ";
						CW = UpdatedCin("CW PermanentStorage", "Current Weight (t)");
						std::cout << std::endl << std::endl;
						A = T / CW;
						std::cout << "Your acceleration is: " << A << " m/s/s.\n";
						RecordInfo(T, "T PermanentStorage");
						RecordInfo(CW, "CW PermanentStorage");
						RecordInfo(A, "A PermanentStorage");
						break;
					}

				case 7:
					{
						std::cout << "You will need to enter Acceleration (m/s/s), and Gravity (m/s/s)\n";
						std::cout << "Start with Acceleration (m/s/s): ";
						A = UpdatedCin("A PermanentStorage", "Acceleration (m/s/s)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter with Acceleration (m/s/s): ";
						G = UpdatedCin("G PermanentStorage", "Gravity (m/s/s)");
						std::cout << std::endl << std::endl;
						TWR = A / G;
						std::cout << "Your TWR is: " << TWR << ".\n";
						RecordInfo(A, "A PermanentStorage");
						RecordInfo(G, "G PermanentStorage");
						RecordInfo(TWR, "TWR PermanentStorage");
						break;
					}

				case 8:
					{
						std::cout << "You will need to enter Average Exhaust Velocity (m/s), and Standard of Gravity (m/s/s)\n";
						std::cout << "Start with Average Exhaust Velocity (m/s)\n";
						AEV = UpdatedCin("AEV PermanentStorage", "Average Exhaust Velocity (m/s)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Gravity (m/s/s): ";
						G0 = UpdatedCin("G PermanentStorage", "Gravity (m/s/s)");
						std::cout << std::endl << std::endl;
						ISP = AEV / G0;
						std::cout << "The ISP of the engine is: " << ISP << " seconds.\n";
						RecordInfo(AEV, "AEV PermanentStorage");
						RecordInfo(G0, "G0 PermanentStorage");
						RecordInfo(ISP, "ISP PermanentStorage");
						break;
					}

				case 9:
					{
						std::cout << "You will need to enter Fuel Lost (KG/s), and Mass of 1 liter of fuel (KG)\n";
						std::cout << "Start with Fuel Lost (KG/s): ";
						FLKGS = UpdatedCin("FLKGS PermanentStorage", "Fuel Lost (KG/s)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Mass of 1 liter of fuel (KG): ";
						M1LF = UpdatedCin("M1LF PermanentStorage", "Mass of 1 liter of fuel (KG)");
						std::cout << std::endl << std::endl;
						FLLS = FLKGS / M1LF;
						std::cout << "\n\nThe fuel lost per second is: " << FLLS << " L/s.\n";
						RecordInfo(FLKGS, "FLKGS PermanentStorage");
						RecordInfo(M1LF, "M1LF PermanentStorage");
						RecordInfo(FLLS, "FLLS PermanentStorage");
						break;
					}

				case 10:
					{
						std::cout << "You will need to enter Average Exhaust Velocity (m/s), Flow area at nozzle (M2), Fuel lost (KG/S), Ambient Pressure (kPa), and Pressure at Nozzle (kPa)\n";
						std::cout << "Start with Average Exhaust Velocity (m/s)\n";
						AEV = UpdatedCin("AEV PermanentStorage", "Average Exhaust Velocity (m/s)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Flow area at nozzle (M2): ";
						FAAN = UpdatedCin("FAAN PermanentStorage", "Flow area at nozzle (m2)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Fuel lost (KG/S): ";
						FLKGS = UpdatedCin("FLKGS PermanentStorage", "Fuel lost (KG/S)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Ambient Pressure (kPa): ";
						AP = UpdatedCin("AP PermanentStorage", "Ambient Pressure (kPa)");
						std::cout << std::endl << std::endl;
						std::cout << "Now enter Pressure at Nozzle (kPa): ";
						PAN = UpdatedCin("PAN PermanentStorage", "Pressure at Nozzle (kPa)");
						std::cout << std::endl << std::endl;
						T = FLKGS *AEV + FAAN *(PAN - AP);
						std::cout << "The total thrust of your rocket is: " << T / 1000 << " KN.\n";
						RecordInfo(AEV, "AEV PermanentStorage");
						RecordInfo(FAAN, "FAAN PermanentStorage");
						RecordInfo(AP, "AP PermanentStorage");
						RecordInfo(T, "T PermanentStorage");
						RecordInfo(PAN, "PAN PermanentStorage");
						RecordInfo(FLKGS, "FLKGS PermanentStorage");
						break;
					}

				case 11:
					{
						std::cout << "\nHere are all of the values you currently have in Permanent Storage:\n\n";
						std::ifstream variablePermanentStorageIF;
						for (int g = 0; g < 20; g++)
						{
							variablePermanentStorageIF.open(variablelist[g] + " PermanentStorage", std::ios:: in);
							if (!variablePermanentStorageIF)
							{
								std::cout << "There is no file!\n";
								exit(1);	// terminate with error
							}

							if (is_empty(variablePermanentStorageIF))
							{
								std::getline(variablePermanentStorageIF, line);
								std::cout << variablelist[g] + ": " << line << std::endl;
							}
							else
							{
								std::cout << variablelist[g] + " is empty." << std::endl;
							}

							variablePermanentStorageIF.close();
						}

						redoLoop = true;
						std::cout << "Would you like to change any values? 1: Yes, 2: No\n";
						std::cin >> choice;
						if (choice == 1)
						{
							while (redoLoop)
							{
								variablePermanentStorageIF.close();
								if (choice == 1)
								{
									std::cout << "What value would you like to change?\n";
									std::cin >> stringChoice;
									variablePermanentStorageIF.open(stringChoice + " PermanentStorage", std::ios:: in);
									if (!variablePermanentStorageIF)
									{
										std::cout << "Sorry, that is not a value.\n";
									}
									else
									{
										std::cout << "What would you like to addjust the value of " << stringChoice << " to?\n";
										std::cin >> doubeChoice;
										RecordInfo(doubeChoice, stringChoice + " PermanentStorage");
										std::cout << stringChoice << " has been sucesfully changed to " << doubeChoice << std::endl;
									}

									std::cout << "\nWould you like to change any other values? 1: Yes, 2: No\n";
									std::cin >> choice;
									if (choice == 2)
									{
										redoLoop = false;
									}
								}
							}
						}

						variablePermanentStorageIF.close();
						break;
					}

				case 12:
					{
						std::cout << "\033[2J\033[0;0H";
						std::cout << "Welcome to the info page!\n";
						std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
						std::cout << "This calculator has been made with lots of effort, and lots of complex formulas, equations, and general math.\n";
						std::cout << "In this information page, you can find all the information needed to use ROCKET-X to it's fullest.\n";
						std::cout << "What would you like to inquire about? \n";
						std::cout << "1) What formulas are used? \n";
						std::cout << "2) What do the variable names mean? \n";
						std::cout << "3) What sources can I use to find out more? \n";
						std::cout << "4) What is planned for the future of ROCKET-X? \n";
						std::cout << "5) Who is the author of this program? \n";
						std::cin >> choice;

						switch (choice)
						{
							case 1:
								{
									std::cout << "\033[2J\033[0;0H";
									std::cout << "This whole program was based off of an idea for a program that can do rocketry equations for you. I have amassed multiple formulas, and here is a list of all of them.\n";
									std::cout << "\n1) The rocket equation: ∆v = EEV* LOG(M0/MF) = ISP *G0* LOG(M0/MF)\n";
									std::cout << "2) Mass of total fuel: MoF = M0 - MF\n";
									std::cout << "3) Mass of 1 liter of fuel: M1LF = MoF / F\n";
									std::cout << "4) Fuel lost in kg/s: FLKGS = M1LF * FLLS;\n";
									std::cout << "5) Effective exhaust velocity: EEV = T / (FLKGS / 1000)). The '/1000' is for unit conversions.\n";
									std::cout << "6) Acceleration: A = T / CW. This is possibly a simplified calculation and only calculates instantanious acceleration, it doesn't take into account jerk, or average acceleration\n";
									std::cout << "7) Thrust to weight ratio: TWR = A / G\n";
									std::cout << "8) Specific Impulse: ISP = AEV / G0\n";
									std::cout << "9) Fuel lost in l/s: FLLS = FLKGS / M1LF\n";
									std::cout << "10) Thrust: T = FLKGS *AEV + FAAN *(PAN - AP)\n";
									std::cout << "\nThese are all of the equations used in this program. If you love math, you can try to do do them by hand and check your awnsers with the program.\n\n";
									break;
								}

							case 2:
								{
									std::cout << "\033[2J\033[0;0H";
									std::cout << "What do the variable names mean? Here is a total list of all the variable names used and what they stand for: \n\n";
									std::cout << "G0 is standard gravity (9.80665 m/s/s)\nG is current gravity (m/s/s)\nF is fuel (l)\nFLLS is fuel lost (l/s)\nISP is specific impulse (s)\nM0 is wet mass (t)\nMF is dry mass (t)\nCW is current weight (t)\nAEV is average effective velocity (m/s)";
									std::cout << "\nMof is Mass of Fuel (KG)\nM1LF is Mass of 1 liter of fuel (T)\nFLKGS is fuel lost (KG/S)\nEEV is effective exhaust velocity (M/S)\nA is acceleration (M/S/S)\nTWR is thrust to weight ratio\nFLLS is fuel lost (L/s)\nDV is delta V (M/S)";
									std::cout << "\nFAAN is flow area at nozzle (m2)\nAP is ambient pressure (kPa)\nPAN is pressure at nozzle (kPa)\nand T is Thrust (KN)\n\n";
									std::cout << "There are lots of variables in rocket science, so now you have a list to refer back to if confused.\n\n";
									std::cout << "Units:\nT: tons\nm/s/s is amount of meters traveled per second increased per second\nl is liters\nl/s is liters per second\ns is seconds\nm/s is meters traveled per second\nKG is kilograms\nm2 is meters squared\nkPa is kilopascals\nKG/S is kilograms per second\nAnd finnaly KN is kilonewton.\n";
									std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
									break;
								}

							case 3:
								{
									break;
								}

							case 4:
								{
									std::cout << "\033[2J\033[0;0H";
									std::cout << "What is planned for the future of ROCKET-X?\n";
									std::cout << "I have quite a few thoughts about where I will take ROCKET-X, but before I continue, I would like to warn you that there is great chance that ROCKET-X never becomes anything greater than what it is now, I have plans, but no guarantees that I will be working on ROCKET-X much longer than I have already.\n";
									std::cout << "With that out of the way, here are some of the possible things which could get developped!\n";
									std::cout << "1) A ∆v budgeting area\n";
									std::cout << "2) A improved staging workspace\n";
									std::cout << "3) More complex forumlas and air resistance\n";
									std::cout << "4) Launch window plotter\n";
									std::cout << "5) Orbit tracker/Burn time calculator\n";
									std::cout << "If you have any more ideas of things to add, contact me at nathan.berglas@gmail.com!\n";
									break;
								}

							case 5:
								{
									std::cout << "\033[2J\033[0;0H";
									std::cout << "Hello, I am the author of ROCKET-X! My name is Nate Berglas, and I love all rocket science. ";
									std::cout << "I made this over the course of a semester in grade 11 computer science course. I studied all sorts of equations, ";
									std::cout << "varibles, and rocket models. I got my inspiration to  make this from Scott Manley's video on doing rocket science ";
									std::cout << "in a spreedsheet. I transfered the math over to C++ and then continued to build from there. If you have any questions ";
									std::cout << "or would like to reach out, you can at nathan.berglas@gmail.com. Thanks for checking out ROCKET-X, and I wish you luck ";
									std::cout << "in your journey of rocket science.\n\n";
									break;
								}
						}

						break;
					}

				case 13:
					{
						NotgoToMainMenu = false;
						break;
					}

				case 14:
					{
						std::cout << "\033[2J\033[0;0H";
						std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
						return 0;
					}
			}

			if (NotgoToMainMenu)
			{
				std::cout << "Would you like to run this program again? 1: Yes, 2: No:\n";
				std::cin >> choice;
				if (choice == 2)
				{
					std::cout << "\033[2J\033[0;0H";
					std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
					return 0;
				}
			}

			NotgoToMainMenu = true;
		}
		else if (choice == 2)
		{
			std::cout << "\nYou have opted to use stage view. To begin, please choose an option.\n";
			std::cout << "1) Create Stages\n";
			std::cout << "2) View stages created\n";
			std::cout << "3) Create rockets\n";
			std::cout << "4) View rockets created\n";
			std::cout << "5) View budgeting menu\n";
			std::cout << "6) Exit back to main menu\n";
			std::cout << "7) Exit ROCKET-X\n";
			std::cin >> choice;

			switch (choice)
			{
				case 1:
					{
						std::cout << "\033[2J\033[0;0H";
						std::cout << "You have opted to create a new stage. The stages will be stored in files, so they save permantly (unless deleted).\n";
						std::cout << "All stages have a name Ex. Eagle. They will also have a code, Ex. 1234. \n\nPlease enter in the name of the stage you want to create.\n";
						std::cin >> stringChoice;
						std::cout << "Now please enter a stage code. (Integer value only 6 digits maximum)\n";
						std::cin >> choice;
						choiceBackup = choice;
						auto choiceS = std::to_string(choice);
						std::ifstream myfile(stringChoice + choiceS + ".txt");
						if (myfile.is_open())
						{
							std::cout << "WARNING, you already have a stage called: " << stringChoice << " with a code: " << choice << ". Do you want to overwrite it? 1: Yes, 0: No\n";
							std::cin >> choice;
							if (!choice)
							{
								break;
							}
							else
							{
								auto choiceBackupS = std::to_string(choiceBackup);
								erase_line(stringChoice + choiceBackupS.c_str() + ".txt");
								stageVector.erase(stageVector.end());
							}
						}

						stageObject.StageName = stringChoice;
						stageObject.stageNumber = choiceBackup;

						std::cout << "Wet mass (t): \n";
						floatChoice[0] = UpdatedCin("M0 PermanentStorage", "Wet mass (t)");
						RecordInfo(floatChoice[0], "M0 PermanentStorage");
						std::cout << "\nDry mass (t): ";
						floatChoice[1] = UpdatedCin("MF PermanentStorage", "Dry mass (t)");
						RecordInfo(floatChoice[1], "MF PermanentStorage");
						std::cout << "\nSpecific impulse (s): ";
						floatChoice[2] = UpdatedCin("ISP PermanentStorage", "Specific Impulse (s)");
						RecordInfo(floatChoice[2], "ISP PermanentStorage");
						std::cout << "\nThrust (KN): ";
						floatChoice[3] = UpdatedCin("T PermanentStorage", "Thrust  (KN)");
						RecordInfo(floatChoice[3], "T PermanentStorage");
						std::cout << "\nCurrent Gravity: ";
						floatChoice[4] = UpdatedCin("G PermanentStorage", "Current gravity (m/s/s)");
						RecordInfo(floatChoice[4], "G PermanentStorage");
						stageObject.set_values(floatChoice[0], floatChoice[1], floatChoice[2], floatChoice[3], floatChoice[4]);

						stageVector.push_back(stageObject);
						stageObject.create_File(stringChoice, choiceBackup);

						std::cout << "You have succesfully created the stage: " << stageObject.StageName << std::endl;
						std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
						std::cin >> choice;
						if (choice == 1)
						{
							break;
						}
						else
						{
							std::cout << "\033[2J\033[0;0H";
							std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
							return 0;
						}

						break;
					}

				case 2:
					{
						std::cout << "Here are all the stages you have create so far, and their values: \n";
						for (int i = 0; i < stageVector.size(); i++)
						{
							std::cout << i + 1 << ") ";
							std::cout << stageVector[i].StageName << ", ";
							std::cout << stageVector[i].stageNumber << std::endl;
						}

						std::cout << "What action would you like to take?\n";
						std::cout << "1) Delete a stage\n";
						std::cout << "2) View a stage\n";
						std::cout << "3) Edit a stage\n";
						std::cout << "4) Take no action\n";
						std::cin >> choice;

						if (choice == 1)
						{
							std::cout << "Which stage would you like to remove? (Please in their number in order, not their code or name)\n";
							std::cin >> choice;
							if (choice < stageVector.size() + 1 && choice > 0)
							{
								stageVector[choice - 1].remove_File(stageVector[choice - 1].StageName, stageVector[choice - 1].stageNumber);
								auto codeS = std::to_string(stageVector[choice - 1].stageNumber);
								erase_line((stageVector[choice - 1].StageName + codeS + ".txt").c_str());
								stageVector.erase(stageVector.begin() + choice - 1);
							}
							else
							{
								std::cout << "Removing the stage failed, please input a correct value.\n";
							}
						}
						else if (choice == 2)
						{
							//View stage
						}
						else if (choice == 3)
						{
							//Edit stage
						} 
						else if (choice == 4) 
						{
							//Do nothing I guess idk
						}
						break;
					}

				case 3:
					{
						std::cout << "You have decided to create a rocket! Rockets are collections of stages that are assembled together in an order.\n";
						std::cout << "All the stages are then re-calculated with their stacking order in mind, and you will have the full understanding of a rocket\n";
						std::cout << "The first step is to name your rocket. What name would you like to give?\n";
						std::cin >> stringChoice;
						std::cout << "Every rocket has a rocket code, please enter yours now. (Integer values only)\n";
						std::cin >> choice;
						auto codeS = std::to_string(choice);
						std::ofstream outfile
						{
							stringChoice + codeS + ".txt" };
						outfile << stringChoice << std::endl;
						outfile << codeS << std::endl;
						std::cout << "And now for the fun part, please choose your stages in order from bottom to top.\n";
						std::cout << "Here is a list of all of your stages:\n";
						for (int i = 0; i < stageVector.size(); i++)
						{
							std::cout << i + 1 << ") ";
							std::cout << stageVector[i].StageName << ", ";
							std::cout << stageVector[i].stageNumber << std::endl;
						}

						do {
							std::cout << "Please enter in the number of the stage you want to add. Put 0 to end. (in order, and enter their number in the order not their code or name)\n";
							std::cin >> choice2;
							if (choice2 < stageVector.size() + 1 && choice2 != 0)
							{
								outfile << stageVector[choice2 - 1].StageName << stageVector[choice2 - 1].stageNumber << ".txt" << std::endl;
								RocketObject.stageList.push_back(stageVector[choice2 - 1]);
								std::cout << "Adding the stage succeded!, please input another value to add another stage. Enter 0 to stop.\n";
								stageCount++;
							}
							else if (choice2 != 0)
							{
								std::cout << "Adding the stage failed, please input a correct value.\n";
							}
						} while (choice2);
						rocketVector.push_back(RocketObject);
						std::fstream fileNamePermanentStorage;
						fileNamePermanentStorage.open("FileName Permanent Storage.txt", std::ios::app);
						fileNamePermanentStorage << RocketObject.rocketName << RocketObject.rocketNumber << ".txt\n";

						std::cout << "Your rocket has succesfully been created!\n";
						std::cout << "Name: " << std::endl;
						std::cout << "Code: " << std::endl;
						std::cout << "Stages in Order: " << std::endl;
						std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
						std::cin >> choice;
						if (choice == 1)
						{
							break;
						}
						else
						{
							std::cout << "\033[2J\033[0;0H";
							std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
							return 0;
						}

						break;
					}

				case 4:
					{
						break;
					}

				case 5:
					{
						break;
					}

				case 6:
					{
						NotgoToMainMenu = false;
						break;
					}

				case 7:
					{
						std::cout << "\033[2J\033[0;0H";
						std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
						return 0;
						break;
					}

				default:
					{
						if (NotgoToMainMenu)
						{
							std::cout << "Would you like to run this program again? 1: Yes, 2: No:\n";
							std::cin >> choice;
							if (choice == 2)
							{
								std::cout << "\033[2J\033[0;0H";
								std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
								return 0;
							}
						}

						NotgoToMainMenu = true;
						break;
					}
			}
		}
		else if (choice == 3)
		{
			std::cout << "\033[2J\033[0;0H";
			std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n";
			return 0;
		}
		else
		{
			std::cout << "\033[2J\033[0;0H";
			std::cout << "You have not chosen either 1 or 2, so you have automaticly quit the program.\n";
			std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n";
			return 0;
		}
	}
}

/*
TODO Long term
-Give sources for more details
-Make rocket creater
-Make rocket viewer
-Add budgeting menu (distance this stage can travel)
-Ability to edit stages

TODO short term
-Make rocket permanenet storage work
-make a rocket viewer
-make rocket showing names number and other stuff work

IDEAS
-For stages, there should be a rocket, and then you can choose premade stages you have made into the rocket, and order them around.
So what this means, is that you can make a stage, put in all the values for that stage, then make as many as you want.
Then you can go to the stage editor and then create a new rocket, and put all of the stages into the rocket in any order you want.
You will need to be able to remove stages, move stages around, delete and create rockets, replace stages, and delete and create new stages
All of this will be stored in files that will be created and destroyed. There will be a file for each rocket, and each stage. 
These will include details needed for the rocket and the stage alike. 

-Expand to a thid part where it calculates orbits, planet tracking, 
launch window, everything to do with spacetravel!

I have no idea how to do this, but elevate the program, to a visual kinda think with like openGL or some other graphics library
*/