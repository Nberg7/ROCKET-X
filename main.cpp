#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <iomanip>

//Used to create stages
class Stage {
  public:
    //Attributes
    double sF, sISP, sM0, sMF, sA, sTWR, sDV, sT, sG;
  int stageNumber;
  std::string StageName;
  //Functions
  void set_values(double, double, double, double, double);
  void create_File(std::string, int, bool);
  void remove_File(std::string, int);
};

//Used to set the values for a stage
void Stage::set_values(double M0, double MF, double ISP, double T, double G) {
  sF = M0 - MF;
  sISP = ISP;
  sM0 = M0;
  sMF = MF;
  sA = T / MF;
  sTWR = sA / G;
  sDV = 9.80665 * ISP * log(M0 / MF);
  sT = T;
  sG = G;
}

//Used to put the stage in dynamic permanent storage
void Stage::create_File(std::string SN, int code, bool store) {
  StageName = SN;
  stageNumber = code;
  auto codeS = std::to_string(code);
  //creates file
  std::ofstream outfile {
    StageName + codeS + ".txt"
  };
  //writes values into text file
  outfile << std::fixed; //To disallow scientific notation, which the program can't read
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
  //If editing, you don't want to add directory again, so you need an if loop to check
  if (store) {
    std::fstream fileNamePermanentStorage;
    fileNamePermanentStorage.open("FileName Permanent Storage.txt", std::ios::app);
    fileNamePermanentStorage << "\n" << StageName << stageNumber << ".txt";
  }
}

//This will delete stages for clean-up
void Stage::remove_File(std::string SN, int code) {
  auto codeS = std::to_string(code);
  if (remove((SN + codeS + ".txt").c_str()) != 0)
    perror("Error deleting file");
}

//Used for creating rockets
class Rocket {
  public:
    //Attributes
    std::string rocketName;
  int rocketNumber;
  std::vector < double > additionalMass; //Additional mass that is included inbetween each stage
  std::vector < Stage > stageList; //Each rocket includes a vector of stages comprising it's parts
  double RocketStats[6]; //Stats in order are  M0, MF, A, DV, T, TWR
  double wetMassAbove, dryMassAbove; //Same as RocketStats[0] and [1], but it can be changed
  //Functions
  void Calculate();
  void remove_File_Rocket(std::string, int);
  void reset();
};

void Rocket::Calculate() {
  // M0
  for (int m = 0; m <= stageList.size(); m++) {
    RocketStats[0] += stageList[m].sM0; //Adds each stages wet mass up
    RocketStats[0] += additionalMass[m];
  }
  //MF
  for (int m = 0; m <= stageList.size(); m++) {
    RocketStats[1] += stageList[m].sMF; //Adds each stages dry mass up
    RocketStats[1] += additionalMass[m];
  }
  //A
  for (int m = 0; m <= stageList.size(); m++) {
    RocketStats[2] += stageList[m].sT / stageList[m].sM0; //Takes the thrust of the bottom stage and divides by total weight
  }
  //DV
  RocketStats[3] = 0;
  for (int g = 0; g < stageList.size(); g++) { //For each stage
    wetMassAbove = 0;
    dryMassAbove = 0;
    for (int k = g; k < stageList.size(); k++) { //Adds up weight above it
      wetMassAbove += stageList[k].sM0;
      dryMassAbove += stageList[k].sMF;
    }
    RocketStats[3] += 9.80665 * stageList.front().sISP * log(wetMassAbove / dryMassAbove); //Uses the rocket for Tsiolkovsky rocket equation
  }
  //T
  for (int m = 0; m <= stageList.size(); m++) {
    RocketStats[4] += stageList[m].sT;
  }
  //TWR
  RocketStats[5] = RocketStats[2] / 9.80665; //Takes rockets acceleration  and divides by gravity (Doesn't have to add each stage up because accleeration already does that)
}

void Rocket::remove_File_Rocket(std::string SN, int code) {
  auto codeS = std::to_string(code);
  if (remove((SN + codeS + "R.txt").c_str()) != 0)
    perror("Error deleting file");
  else
    puts("File successfully deleted");
}

//This is used to reset RocketObject each time a new rocket is created so the old rocket doesn't overlap
void Rocket::reset() {
  rocketName = "";
  int rocketNumber = 0;
  additionalMass.clear();
  stageList.clear();
}

//Used for stages, this can read a specific line in a file (NOT MADE BY ME)
std::fstream & GotoLine(std::fstream & file, unsigned int num) {
  file.seekg(std::ios::beg);
  for (int i = 0; i < num - 1; ++i) {
    file.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');
  }

  return file;
}

//This checks if a file is empty
bool is_empty(std::ifstream & pFile) {
  return pFile.peek() != EOF;
}

//This is updateCin. It does the same as a regular cin, but it give the option to re-use a value from permanent storage
double UpdatedCin(std::string input_name, std::string variable_name) {
  double Outputvalue;
  double fileContents;
  bool choicebool;

  std::ifstream variablePermanentStorageIF;
  variablePermanentStorageIF.open(input_name, std::ios:: in );
  if (variablePermanentStorageIF.peek() != EOF) {
    variablePermanentStorageIF >> fileContents;
    std::cout << "You already have inputed a value for " << variable_name << " . Would you like to use: " << fileContents << " again? 1 for yes, 0 for no.\n";
    std::cin >> choicebool;
    if (!choicebool) {
      std::cout << "You have opted to re-enter the value\n";
      std::cin >> Outputvalue;
    } else {
      std::cout << "You have opted to re-use the value\n";
      Outputvalue = fileContents;
    }
  } else {
    std::cin >> Outputvalue;
  }

  variablePermanentStorageIF.close();
  return Outputvalue;
}

//This is RecordInfo. It puts the value of any variable into permanent storage 
void RecordInfo(double value, std::string input_name) {
  std::string ValueToStorage;
  bool choicebool;
  double x;
  std::ofstream variablePermanentStorageOF;
  variablePermanentStorageOF.open(input_name, std::ios::out);
  if (!variablePermanentStorageOF) {
    std::cout << "Unable to open file";
    exit(1); // terminate with error
  }

  ValueToStorage = std::to_string(value);
  variablePermanentStorageOF << ValueToStorage << std::endl;
  variablePermanentStorageOF.close();
}

//Used to remove speciic strings from Filename permanenet storage. This is to remove a duplicate from FileName Permanent Storage
void erase_line(std::string fileName) {
  std::string line, stringtobeSearched = fileName;
  std::ifstream inFile("FileName Permanent Storage.txt");
  std::ofstream outFile("output.txt");
  if (inFile) {
    while (getline(inFile, line, '\n')) {
      //if the line read is not same as string searched for then write it into the output.txt file
      if (line != stringtobeSearched) {
        outFile << line << "\n";
      }
    }
  } else {
    std::cout << "File could not be read" << std::endl;
  }

  remove("FileName Permanent Storage.txt");
  rename("output.txt", "FileName Permanent Storage.txt");
}

void erase_line_rocket(std::string fileName) {
  std::string line, stringtobeSearched = fileName;
  std::ifstream inFile("RocketName Permanent Storage.txt");
  std::ofstream outFile("output.txt");
  if (inFile) {
    while (getline(inFile, line, '\n')) {
      //if the line read is not same as string searched for then write it into the output.txt file
      if (line != stringtobeSearched) {
        outFile << line << "\n";
      }
    }
    remove("RocketName Permanent Storage.txt");
    rename("output.txt", "RocketName Permanent Storage.txt");
  } else {
    std::cout << "File could not be read" << std::endl;
    exit(1); // terminate with error
  }
}

int main() {

  //Used for outputting 2 decimal points EX. 2.00 not
  std::cout << std::setprecision(2);

  //Used to prevent scientific notation from being used
  std::cout << std::fixed;

  //Integer variables
  int choice;
  int choice2;
  int choiceBackup;
  int number_of_lines = 0;

  //String variables
  std::string stringChoice;
  std::string line;

  //Double and float variables
  double doubleChoice;
  float floatChoice[10];

  //Bool varialbes
  bool NotgoToMainMenu = true;
  bool redoLoop = true;
  bool boolChoice;
  bool repeatStage = true;

  //Vectors
  std::vector < Stage > stageVector;
  std::vector < Rocket > rocketVector;

  //Opening permanent storage text files
  std::fstream FileNames;
  FileNames.open("FileName Permanent Storage.txt");

  std::fstream RocketNames;
  RocketNames.open("RocketName Permanent Storage.txt");

  //Declaring objects for each class. These objects are then pushed back into the vector for storage
  Rocket RocketObject;
  Stage stageObject;

  //Rocketry variables
  double G0, G, F, FLLS, ISP, M0, MF, CW, AEV; //G0 is standard gravity (9.80665 m/s/s), G is current gravity (m/s/s), F is fuel (l), FLLS is fuel lost (l/s), ISP is specific impulse (s), M0 is wet mass (t), MF is dry mass (t), CW is current weight (t), and AEV is average effective velocity (m/s) 
  double MoF, M1LF, FLKGS, EEV, A, TWR, DV; //Mof is Mass of Fuel (KG), M1LF is Mass of 1 liter of fuel (T), FLKGS is fuel lost (KG/S), EEV is effective exhaust velocity (M/S), A is acceleration (M/S/S), TWR is thrust to weight ratio, FLLS is fuel lost (L/s), and DV is delta V (M/S)
  double FAAN, AP, PAN, T; //FAAN is flow area at nozzle (m2), AP is ambient pressure (kPa), PAN is pressure at nozzle (kPa), and T is Thrust (KN), 

  //goes through filename permanent storage, opens the files, and then creates objects 
  //for each one, so the vector is always accurate with the stages created

  std::string variablelist[20] = {
    "G0",
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
    "T"
  };

  //Stages and rockets are held in text files, but to acces them, you need another text file that holds the directorys
  //This function reads through each line of that file, and the # of lines is equal to how many stages/rockets there are to read
  while (std::getline(FileNames, line)) {
    ++number_of_lines;
  }

  //Goes through each stage in permanent storage, creates an object for them, and then pushes it back into a vector
  FileNames.clear();
  FileNames.seekg(0);
  for (int k = 0; k < number_of_lines; k++) {
    //std::getline(std::setprecision(2));
    GotoLine(FileNames, k + 1);
    std::getline(FileNames, line);
    std::ifstream StageFiles;
    StageFiles.open(line);
    FileNames.clear();
    FileNames.seekg(0);
    std::getline(StageFiles, line);
    stageObject.StageName = line;
    std::getline(StageFiles, line);
    stageObject.stageNumber = atof(line.c_str());
    // sF
    std::getline(StageFiles, line);
    stageObject.sF = atof(line.c_str());
    // sIsp
    std::getline(StageFiles, line);
    stageObject.sISP = atof(line.c_str());
    // sM0
    std::getline(StageFiles, line);
    stageObject.sM0 = atof(line.c_str());
    // sMF
    std::getline(StageFiles, line);
    stageObject.sMF = atof(line.c_str());
    // sA
    std::getline(StageFiles, line);
    stageObject.sA = atof(line.c_str());
    // sTWR
    std::getline(StageFiles, line);
    stageObject.sTWR = atof(line.c_str());
    // sDV
    std::getline(StageFiles, line);
    stageObject.sDV = atof(line.c_str());
    // sT
    std::getline(StageFiles, line);
    stageObject.sT = atof(line.c_str());
    // sG
    std::getline(StageFiles, line);
    stageObject.sG = atof(line.c_str());

    stageVector.push_back(stageObject);
  }
  number_of_lines = 0;

  //Does the same thing, but for rockets not stages
  RocketNames.clear();
  RocketNames.seekg(0);

  while (std::getline(RocketNames, line)) {
    ++number_of_lines;
  }

  RocketNames.clear();
  RocketNames.seekg(0);
  for (int k = 0; k < number_of_lines; k++) {
    if (!RocketNames) {
      RocketNames.open("RocketName Permanent Storage.txt");
    }
    GotoLine(RocketNames, k + 1);
    std::getline(RocketNames, line);
    std::ifstream RocketFiles;
    RocketNames.close();
    RocketFiles.open(line);
    RocketNames.clear();
    RocketNames.seekg(0);
    std::getline(RocketFiles, line);
    RocketObject.rocketName = line;
    std::getline(RocketFiles, line);
    RocketObject.rocketNumber = atof(line.c_str());;
    RocketObject.stageList.clear();
    while (std::getline(RocketFiles, line)) {
      RocketObject.stageList.push_back(stageVector[atof(line.c_str())]);
      std::getline(RocketFiles, line);
      RocketObject.additionalMass.push_back(atof(line.c_str()));
    }
    rocketVector.push_back(RocketObject);
    RocketFiles.close();
    rocketVector.back().Calculate();
  }
  number_of_lines = 0;

  //This is the main loop for the whole program, this is used so that the user can run the program multiple times without closing and re-opening
  //Mainmenu label is for goto satement on line 866 to break out of a multi nested loop
  MAINMENU: while (true) {
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
    if (choice == 1) {
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

      switch (choice) {
      case 1: {
        std::cout << "Which method would you like to use?\n1: Standard of Gravity, Specific Impulse, Wet mass, Dry mass\n";
        std::cout << "2: Effective Exhaust Velocity, Wet mass, Dry mass\n";
        std::cin >> choice;
        if (choice == 1) {
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
          //This is the math:
          DV = G0 * ISP * log(M0 / MF);
          std::cout << "Your Delta V is a total of: " << DV << "M/S.\n";
          RecordInfo(G0, "G0 PermanentStorage");
          RecordInfo(ISP, "ISP PermanentStorage");
          RecordInfo(M0, "M0 PermanentStorage");
          RecordInfo(MF, "MF PermanentStorage");
          RecordInfo(DV, "DV PermanentStorage");
        } else if (choice == 2) {
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
          //This is the math:
          DV = EEV * log(M0 / MF);
          std::cout << "Your Delta V is a total of: " << DV << "M/S.\n";
          RecordInfo(EEV, "EEV PermanentStorage");
          RecordInfo(M0, "M0 PermanentStorage");
          RecordInfo(MF, "MF PermanentStorage");
          RecordInfo(MF, "DV PermanentStorage");
        }

        break;
      }

      case 2: {
        std::cout << "You will need to enter Wet Mass (t), and Dry Mass (t)\n";
        std::cout << "Start with Wet Mass (t): ";
        M0 = UpdatedCin("M0 PermanentStorage", "Wet Mass (t)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Dry Mass (t): ";
        MF = UpdatedCin("MF PermanentStorage", "Dry Mass (t)");
        std::cout << std::endl << std::endl;
        //This is the math:
        MoF = M0 - MF;
        std::cout << "The total mass of your fuel is: " << MoF << " Tons.\n";
        RecordInfo(M0, "M0 PermanentStorage");
        RecordInfo(MF, "MF PermanentStorage");
        RecordInfo(MoF, "MoF PermanentStorage");
        break;
      }

      case 3: {
        std::cout << "You will need to enter Mass of fuel (t), and Fuel (l)\n";
        std::cout << "Start with Mass of fuel (t): ";
        MoF = UpdatedCin("MoF PermanentStorage", "Mass of fuel (t)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Fuel (l): ";
        F = UpdatedCin("F PermanentStorage", "Fuel (l)");
        std::cout << std::endl << std::endl;
        //This is the math:
        M1LF = MoF / F;
        M1LF *= 1000;
        std::cout << "The mass of your fuel per liter is: " << M1LF << " KG.\n";
        RecordInfo(MoF, "MoF PermanentStorage");
        RecordInfo(F, "F PermanentStorage");
        RecordInfo(M1LF, "M1LF PermanentStorage");
        break;
      }

      case 4: {
        std::cout << "You will need to enter Mass of 1 liter of fuel (KG), and Fuel Lost (L/S)\n";
        std::cout << "Start with Mass of 1 liter of fuel (KG): ";
        M1LF = UpdatedCin("M1LF PermanentStorage", "Mass of 1 liter of fuel (KG)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Fuel Lost (L/S): ";
        FLLS = UpdatedCin("FLLS PermanentStorage", "Fuel Lost (L/S)");
        std::cout << std::endl << std::endl;
        //This is the math:
        FLKGS = M1LF * FLLS;
        std::cout << "The amount of fuel lost per second is: " << FLKGS << " KG.\n";
        RecordInfo(M1LF, "M1LF PermanentStorage");
        RecordInfo(FLLS, "FLLS PermanentStorage");
        RecordInfo(FLKGS, "FLKGS PermanentStorage");
        break;
      }

      case 5: {
        std::cout << "You will need to enter Thrust (KN), and Fuel lost (KG/s).\n";
        std::cout << "Start with Thrust (KN): ";
        T = UpdatedCin("T PermanentStorage", "Thrust (KN)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Fuel Lost (KG/s): ";
        FLKGS = UpdatedCin("FLKGS PermanentStorage", "Fuel lost (KG/s)");
        std::cout << std::endl << std::endl;
        //This is the math:
        EEV = T / (FLKGS / 1000);
        std::cout << "Your effective exhaust velocity is " << EEV << "M/S.\n";
        RecordInfo(T, "T PermanentStorage");
        RecordInfo(FLKGS, "FLKGS PermanentStorage");
        RecordInfo(EEV, "EEV PermanentStorage");
        break;
      }

      case 6: {
        std::cout << "You will need to enter Thrust (KN), Current Weight (t)\n";
        std::cout << "Start with Thrust (KN): ";
        T = UpdatedCin("T PermanentStorage", "Thrust (KN)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Current Weight (t): ";
        CW = UpdatedCin("CW PermanentStorage", "Current Weight (t)");
        std::cout << std::endl << std::endl;
        //This is the math:
        A = T / CW;
        std::cout << "Your acceleration is: " << A << " m/s/s.\n";
        RecordInfo(T, "T PermanentStorage");
        RecordInfo(CW, "CW PermanentStorage");
        RecordInfo(A, "A PermanentStorage");
        break;
      }

      case 7: {
        std::cout << "You will need to enter Acceleration (m/s/s), and Gravity (m/s/s)\n";
        std::cout << "Start with Acceleration (m/s/s): ";
        A = UpdatedCin("A PermanentStorage", "Acceleration (m/s/s)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter with Acceleration (m/s/s): ";
        G = UpdatedCin("G PermanentStorage", "Gravity (m/s/s)");
        std::cout << std::endl << std::endl;
        //This is the math:
        TWR = A / G;
        std::cout << "Your TWR is: " << TWR << ".\n";
        RecordInfo(A, "A PermanentStorage");
        RecordInfo(G, "G PermanentStorage");
        RecordInfo(TWR, "TWR PermanentStorage");
        break;
      }

      case 8: {
        std::cout << "You will need to enter Average Exhaust Velocity (m/s), and Standard of Gravity (m/s/s)\n";
        std::cout << "Start with Average Exhaust Velocity (m/s)\n";
        AEV = UpdatedCin("AEV PermanentStorage", "Average Exhaust Velocity (m/s)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Gravity (m/s/s): ";
        G0 = UpdatedCin("G PermanentStorage", "Gravity (m/s/s)");
        std::cout << std::endl << std::endl;
        //This is the math:
        ISP = AEV / G0;
        std::cout << "The ISP of the engine is: " << ISP << " seconds.\n";
        RecordInfo(AEV, "AEV PermanentStorage");
        RecordInfo(G0, "G0 PermanentStorage");
        RecordInfo(ISP, "ISP PermanentStorage");
        break;
      }

      case 9: {
        std::cout << "You will need to enter Fuel Lost (KG/s), and Mass of 1 liter of fuel (KG)\n";
        std::cout << "Start with Fuel Lost (KG/s): ";
        FLKGS = UpdatedCin("FLKGS PermanentStorage", "Fuel Lost (KG/s)");
        std::cout << std::endl << std::endl;
        std::cout << "Now enter Mass of 1 liter of fuel (KG): ";
        M1LF = UpdatedCin("M1LF PermanentStorage", "Mass of 1 liter of fuel (KG)");
        std::cout << std::endl << std::endl;
        //This is the math:
        FLLS = FLKGS / M1LF;
        std::cout << "\n\nThe fuel lost per second is: " << FLLS << " L/s.\n";
        RecordInfo(FLKGS, "FLKGS PermanentStorage");
        RecordInfo(M1LF, "M1LF PermanentStorage");
        RecordInfo(FLLS, "FLLS PermanentStorage");
        break;
      }

      case 10: {
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
        //This is the math:
        T = FLKGS * AEV + FAAN * (PAN - AP);
        std::cout << "The total thrust of your rocket is: " << T / 1000 << " KN.\n";
        RecordInfo(AEV, "AEV PermanentStorage");
        RecordInfo(FAAN, "FAAN PermanentStorage");
        RecordInfo(AP, "AP PermanentStorage");
        RecordInfo(T, "T PermanentStorage");
        RecordInfo(PAN, "PAN PermanentStorage");
        RecordInfo(FLKGS, "FLKGS PermanentStorage");
        break;
      }

      case 11: {
        //For looking thorugh each value stored in permanent storage
        std::cout << "\nHere are all of the values you currently have in Permanent Storage:\n\n";
        std::ifstream variablePermanentStorageIF;
        for (int g = 0; g < 20; g++) {
          variablePermanentStorageIF.open(variablelist[g] + " PermanentStorage", std::ios:: in );
          if (!variablePermanentStorageIF) {
            std::cout << "There is no file!\n";
            exit(1); // terminate with error
          }

          if (is_empty(variablePermanentStorageIF)) {
            std::getline(variablePermanentStorageIF, line);
            std::cout << variablelist[g] + ": " << line << std::endl;
          } else {
            std::cout << variablelist[g] + " is empty." << std::endl;
          }

          variablePermanentStorageIF.close();
        }

        redoLoop = true;
        std::cout << "Would you like to change any values? 1: Yes, 2: No\n";
        std::cin >> choice;
        if (choice == 1) {
          while (redoLoop) {
            variablePermanentStorageIF.close();
            if (choice == 1) {
              std::cout << "What value would you like to change?\n";
              std::cin >> stringChoice;
              variablePermanentStorageIF.open(stringChoice + " PermanentStorage", std::ios:: in );
              if (!variablePermanentStorageIF) {
                std::cout << "Sorry, that is not a value.\n";
              } else {
                std::cout << "What would you like to addjust the value of " << stringChoice << " to?\n";
                std::cin >> doubleChoice;
                RecordInfo(doubleChoice, stringChoice + " PermanentStorage");
                std::cout << stringChoice << " has been sucesfully changed to " << doubleChoice << std::endl;
              }

              std::cout << "\nWould you like to change any other values? 1: Yes, 2: No\n";
              std::cin >> choice;
              if (choice == 2) {
                redoLoop = false;
              }
            }
          }
        }

        variablePermanentStorageIF.close();
        break;
      }

      case 12: {
        //This is the info page. It doesn't do any math, but I thought it was a fun addition to have
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

        switch (choice) {
        case 1: {
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

        case 2: {
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

        case 3: { // I have yet to collect all the sources used. Most are from wikipedia, online threads, nasa pages, or stack overflow
          break;
        }

        case 4: {
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

        case 5: {
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

      case 13: {
        //This makes sure that the user goes directly back to the main menu, when they would usually be asked first. 
        NotgoToMainMenu = false;
        break;
      }

      case 14: {
        //Quits the program
        std::cout << "\033[2J\033[0;0H";
        std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
        return 0;
      }
      }

      //Checks if the program needs to ask the user
      if (NotgoToMainMenu) {
        std::cout << "Would you like to run this program again? 1: Yes, 2: No\n";
        std::cin >> choice;
        if (choice == 2) {
          std::cout << "\033[2J\033[0;0H";
          std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
          return 0;
        }
      }

      //Resets this value, so that the user would be asked next time unless once again told not to be.
      NotgoToMainMenu = true;
    } else if (choice == 2) // This is the start of the second part of the program, things get very complicated around here.
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

      switch (choice) {
      case 1: {
        std::cout << "\033[2J\033[0;0H";
        std::cout << "You have opted to create a new stage. The stages will be stored in files, so they save permantly (unless deleted).\n";
        std::cout << "All stages have a name Ex. Eagle. They will also have a code, Ex. 1234. \n\nPlease enter in the name of the stage you want to create.\n";
        std::cin >> stringChoice;
        std::cout << "Now please enter a stage code. (Integer value only 6 digits maximum)\n";
        std::cin >> choice;
        choiceBackup = choice;
        auto choiceS = std::to_string(choice);
        std::ifstream myfile(stringChoice + choiceS + ".txt");
        //Checks to make sure that the user is not overwriting a stage without realizing it. if the file is open, that means that a file already exists when it shouldn't
        if (myfile.is_open()) {
          std::cout << "WARNING, you already have a stage called: " << stringChoice << " with a code: " << choice << ". Do you want to overwrite it? 1: Yes, 0: No\n";
          std::cin >> choice;
          if (!choice) {
            break;
          } else {
            auto choiceBackupS = std::to_string(choiceBackup);
            erase_line(stringChoice + choiceBackupS.c_str() + ".txt"); // Removes the old directory from permanent storage to not have multiple directorys for the same overwriten stages
            stageVector.erase(stageVector.end());
          }
        }
        //This is all the same as a regular stage creation
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
        stageObject.create_File(stringChoice, choiceBackup, 1);
        myfile.close();
        std::cout << "You have succesfully created the stage: " << stageObject.StageName << std::endl;
        std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
        std::cin >> choice;
        if (choice == 1) {
          break;
        } else {
          std::cout << "\033[2J\033[0;0H";
          std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
          return 0;
        }

        break;
      }

      case 2: {
        std::cout << "Here are all the stages you have create so far, and their values: \n";
        for (int i = 0; i < stageVector.size(); i++) //Checks size of vector, then runs through each item
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

        if (choice == 1) {
          std::cout << "Which stage would you like to remove? (Please in their number in order, not their code or name)\n";
          std::cin >> choice;
          for (int l = 0; l < rocketVector.size(); l++) {
            for (int g = 0; g < rocketVector[l].stageList.size(); g++) {
              if (rocketVector[l].stageList[g].StageName == stageVector[choice - 1].StageName && rocketVector[l].stageList[g].stageNumber == stageVector[choice - 1].stageNumber) {
                std::cout << "This stage is currently being used in a rocket. You  must first remove this stage from the rocket to continue.\n";
                std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
                std::cin >> choice;
                if (choice == 1) {
                  goto MAINMENU;
                } else {
                  std::cout << "\033[2J\033[0;0H";
                  std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
                  return 0;
                }
              }
            }
          }
          if (choice < stageVector.size() + 1 && choice > 0) {
            // This code both deletes the file, removes it from the vector, and removes directory from permanent storage
            stageVector[choice - 1].remove_File(stageVector[choice - 1].StageName, stageVector[choice - 1].stageNumber);
            auto codeS = std::to_string(stageVector[choice - 1].stageNumber);
            erase_line((stageVector[choice - 1].StageName + codeS + ".txt").c_str());
            stageVector.erase(stageVector.begin() + choice - 1);
          } else {
            std::cout << "Removing the stage failed, please input a correct value.\n";
            std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
            std::cin >> choice;
            if (choice == 1) {
              break;
            } else {
              std::cout << "\033[2J\033[0;0H";
              std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
              return 0;
            }
          }
        } else if (choice == 2) {
          //Very simple, the program just runs over every value and cout's it
          std::cout << "What stage would you like to view?\n";
          std::cin >> choice;
          std::cout << std::fixed;
          std::cout << std::setprecision(2);
          if (choice > 0 && choice < stageVector.size() + 1) {
            std::cout << "Stage Name: ";
            std::cout << stageVector[choice - 1].StageName << "\n";
            std::cout << "Stage Number: ";
            std::cout << stageVector[choice - 1].stageNumber << "\n";
            std::cout << "Fuel Amount (t): ";
            std::cout << stageVector[choice - 1].sF << "\n";
            std::cout << "Specific Impulse (s): ";
            std::cout << stageVector[choice - 1].sISP << "\n";
            std::cout << "Wet Mass (t): ";
            std::cout << stageVector[choice - 1].sM0 << "\n";
            std::cout << "Dry Mass (t): ";
            std::cout << stageVector[choice - 1].sMF << "\n";
            std::cout << "Acceleration (m/s/s): ";
            std::cout << stageVector[choice - 1].sA << "\n";
            std::cout << "Thrust to weight ratio: ";
            std::cout << stageVector[choice - 1].sTWR << "\n";
            std::cout << "Delta V (m/s): ";
            std::cout << stageVector[choice - 1].sDV << "\n";
            std::cout << "Thrust: ";
            std::cout << stageVector[choice - 1].sT << "\n";
            std::cout << "Current Gravity: ";
            std::cout << stageVector[choice - 1].sG << "\n";
            std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
            std::cin >> choice;
            if (choice == 1) {
              break;
            } else {
              std::cout << "\033[2J\033[0;0H";
              std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
              return 0;
            }
          } else {
            std::cout << "You did not input a viable value\n";
            std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
            std::cin >> choice;
            if (choice == 1) {
              break;
            } else {
              std::cout << "\033[2J\033[0;0H";
              std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
              return 0;
            }
            break;
          }
        } else if (choice == 3) {
          std::cout << "What stage would you like to edit?\n";
          std::cin >> choice;
          std::cout << "Put in the new values for the stage:\n";
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
          stageVector[choice - 1].set_values(floatChoice[0], floatChoice[1], floatChoice[2], floatChoice[3], floatChoice[4]); //This re-sets the values for the vector
          stageVector[choice - 1].remove_File(stageVector[choice - 1].StageName, stageVector[choice - 1].stageNumber); // This removes the old file, so there isn't two files
          stageVector[choice - 1].create_File(stageVector[choice - 1].StageName, stageVector[choice - 1].stageNumber, 0); // Creates a new file with new values
          //The program doesn't have to worry about 2 directorys in permanent storage, since that was taken care of in the create_file function
        } else if (choice == 4) {
          //Do nothing I guess idk. I feel like I maybe should, but idc really
        }
        break;
      }

      case 3: {
        RocketObject.reset();
        //Rockets in theory are very similar to stages, but they do differ. Most of the code here is the same as in the stage segment.
        std::cout << "You have decided to create a rocket! Rockets are collections of stages that are assembled together in an order.\n";
        std::cout << "All the stages are then re-calculated with their stacking order in mind, and you will have the full understanding of a rocket\n";
        std::cout << "The first step is to name your rocket. What name would you like to give?\n";
        std::cin >> stringChoice;
        RocketObject.rocketName = stringChoice;
        std::cout << "Every rocket has a rocket code, please enter yours now. (Integer values only)\n";
        std::cin >> choice;
        RocketObject.rocketNumber = choice;
        auto choiceS2 = std::to_string(choice);
        std::ifstream myfile(stringChoice + choiceS2 + "R.txt");
        //Checks to make sure that the user is not overwriting a rocket without realizing it. if the file is open, that means that a file already exists when it shouldn't
        if (myfile.is_open()) {
          std::cout << "WARNING, you already have a rocket called: " << stringChoice << " with a code: " << choice << ". Do you want to overwrite it? 1: Yes, 0: No\n";
          std::cin >> choice2;
          if (!choice2) {
            break;
          } else {
            auto choiceBackupS = std::to_string(choice);
            erase_line_rocket(stringChoice + choiceBackupS.c_str() + "R.txt"); // Removes the old directory from permanent storage to not have multiple directorys for the same overwriten stages
          }
        }
        auto codeS = std::to_string(choice);
        std::ofstream outfile {
          stringChoice + codeS + "R.txt"
        };
        outfile << stringChoice << std::endl;
        outfile << choice << std::endl;
        std::cout << "And now for the fun part, please choose your stages in order from bottom to top.\n";
        std::cout << "Here is a list of all of your stages:\n";
        for (int i = 0; i < stageVector.size(); i++) {
          std::cout << i + 1 << ") ";
          std::cout << stageVector[i].StageName << ", ";
          std::cout << stageVector[i].stageNumber << std::endl;
        }

        std::cout << "Please enter in the number of the stage you want to add. Put 0 to end. (in order, and enter their number in the order not their code or name)\n";
        do {
          std::cin >> choice2;
          if (choice2 < stageVector.size() + 1 && choice2 != 0) {
            outfile << choice2 - 1 << std::endl;
            RocketObject.stageList.push_back(stageVector[choice2 - 1]);
            std::cout << "Adding the stage succeded!, now enter the additional mass inbetween this stage and the next (Not included in the mass of the stage).\n If this is the final stage you wish to add, then this will serve as the mass of the payload\n";
            std::cin >> doubleChoice;
            RocketObject.additionalMass.push_back(doubleChoice);
            outfile << doubleChoice << std::endl;
            std::cout << "Additional mass has been added. Please enter your next stage, or 0 to stop.\n";
          } else if (choice2 != 0) {
            std::cout << "Adding the stage failed, please input a correct value.\n";
          }
        } while (choice2);
        RocketObject.Calculate();
        rocketVector.push_back(RocketObject);
        std::fstream rocketNamePermanentStorage;
        rocketNamePermanentStorage.open("RocketName Permanent Storage.txt", std::ios::app);
        rocketNamePermanentStorage << RocketObject.rocketName << RocketObject.rocketNumber << "R.txt\n";
        rocketNamePermanentStorage.close();
        std::cout << "Your rocket has succesfully been created!\n";
        std::cout << "Name: " << RocketObject.rocketName << std::endl;
        std::cout << "Code: " << RocketObject.rocketNumber << std::endl;
        std::cout << "Stages in Order: \n";
        for (int p = 0; p <= rocketVector.back().stageList.size(); p++) {
          std::cout << rocketVector.back().stageList[p].StageName << ", " << rocketVector.back().stageList[p].stageNumber << "\n";
        }
        myfile.close();
        std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
        std::cin >> choice;
        if (choice == 1) {
          break;
        } else {
          std::cout << "\033[2J\033[0;0H";
          std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
          return 0;
        }
        break;
      }

      case 4: {
        std::cout << "You have decided to view your created rockets! Here is a list of all your rockets you have created:\n";
        //Nested for loops to display rockets, and stages attached
        for (int s = 0; s < rocketVector.size(); s++) {
          std::cout << s + 1 << ") " << rocketVector[s].rocketName << rocketVector[s].rocketNumber << " (";
          for (int v = 0; v < rocketVector[s].stageList.size(); v++) {
            std::cout << rocketVector[s].stageList[v].StageName << rocketVector[s].stageList[v].stageNumber;
            if (v + 1 < rocketVector[s].stageList.size()) // Makes sure that it isn't the last time runnig the loop 
            {
              std::cout << ", ";
            }
          }
          std::cout << ")\n";
        }
        std::cout << "What action would you like to take?\n";
        std::cout << "1) Delete a rocket\n";
        std::cout << "2) View a rocket\n";
        std::cout << "3) Edit a rocket\n";
        std::cout << "4) Take no action\n";
        std::cin >> choice;
        if (choice == 1) {
          //Delete the rocket (Same as stage, code for code but stage is replaced with rocket)
          std::cout << "Which stage would you like to remove? (Please in their number in order, not their code or name)\n";
          std::cin >> choice;
          if (choice < rocketVector.size() + 1 && choice > 0) {
            // This code both deletes the file, removes it from the vector, and removes directory from permanent storage
            rocketVector[choice - 1].remove_File_Rocket(rocketVector[choice - 1].rocketName, rocketVector[choice - 1].rocketNumber);
            auto codeS = std::to_string(rocketVector[choice - 1].rocketNumber);
            erase_line_rocket((rocketVector[choice - 1].rocketName + codeS + "R.txt").c_str());
            rocketVector.erase(rocketVector.begin() + choice - 1);
          } else {
            std::cout << "Removing the rocket has failed, please input a correct value.\n";
            std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
            std::cin >> choice;
            if (choice == 1) {
              break;
            } else {
              std::cout << "\033[2J\033[0;0H";
              std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
              return 0;
            }
          }
        } else if (choice == 2) {
          std::cout << "You have chosen to view a rocket. Which rocket would you like to view?\n";
          for (int s = 0; s < rocketVector.size(); s++) {
            std::cout << s + 1 << ") " << rocketVector[s].rocketName << rocketVector[s].rocketNumber << " (";
            for (int v = 0; v < rocketVector[s].stageList.size(); v++) {
              std::cout << rocketVector[s].stageList[v].StageName << rocketVector[s].stageList[v].stageNumber;
              if (v + 1 < rocketVector[s].stageList.size()) // Makes sure that it isn't the last time runnig the loop 
              {
                std::cout << ", ";
              }
            }
            std::cout << ")\n";
          }
          std::cin >> choice;
          if (choice < 1 || choice > rocketVector.size()) {
            std::cout << "That is not a rocket, please input a correct value.\n";
            std::cout << "Would you like return to the main menu (1), or quit the program (2)?\n";
            std::cin >> choice;
            if (choice == 1) {
              break;
            } else {
              std::cout << "\033[2J\033[0;0H";
              std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
              return 0;
            }
          }
          std::cout << "\n\nYou have opted to view the rocket " << rocketVector[choice - 1].rocketName << " " << rocketVector[choice - 1].rocketNumber;
          std::cout << "\nRocket Name: " << rocketVector[choice - 1].rocketName;
          std::cout << "\nRocket Number: " << rocketVector[choice - 1].rocketNumber;
          std::cout << "\nStages In Order: (";
          for (int b = 0; b < rocketVector[choice - 1].stageList.size(); b++) {
            if (b + 1 < rocketVector[choice - 1].stageList.size()) { // Makes sure that it isn't the last time runnig the loop 
              std::cout << rocketVector[choice - 1].stageList[b].StageName << rocketVector[choice - 1].stageList[b].stageNumber << ", ";
            } else {
              std::cout << rocketVector[choice - 1].stageList[b].StageName << rocketVector[choice - 1].stageList[b].stageNumber << ")\n";
            }
          }
          std::cout << "\n~~~ Rocket Statistics (For the entire rocket combined) ~~~~ ";
          std::cout << "\nWet Mass: " << rocketVector[choice - 1].RocketStats[0] << " Tonnes";
          std::cout << "\nDry Mass: " << rocketVector[choice - 1].RocketStats[1] << " Tonnes";
          //std::cout << "\nAcceleration: " << rocketVector[choice - 1].RocketStats[2] << " M/S/S"; //Not usefull for a rocket combined
          std::cout << "\n∆V: " << rocketVector[choice - 1].RocketStats[3] << " M/S";
          std::cout << "\nThrust: " << rocketVector[choice - 1].RocketStats[4] << " KN";
          //std::cout << "\nThrust to weight ratio: " << rocketVector[choice - 1].RocketStats[5]; //Not usefull for a rocket combined

          std::cout << "\n\n~~~ Stage Indepth View ~~~\n";

          std::cout << "\n\nWould you like return to the main menu (1), or quit the program (2)?\n";
          std::cin >> choice;
          if (choice == 1) {
            break;
          } else {
            std::cout << "\033[2J\033[0;0H";
            std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
            return 0;
          }
          //You have chosen to view a rocket. Which rocket would you like to view?
          //1) Saturn 5 (SIC1, SII1, SIVB1)
          //2) Saturn 6 (SIC1, SIC1, SII1, SII1, SIVB1, SIVB1)
          //3) Saturn 7 (SIC1, SII1, SII1, SIVB1)
          //1
          //
          //
          //You have opted to view the rocket Saturn 5.
          //Rocket Name: Saturn
          //Rocket Number: 5
          //Stages in order: (SIC1, SII1, SIVB1)
          //Rocket Statistics:
          //Wet Mass:
          //Dry Mass:
          //Acceleration:
          //∆V:
          //Thrust:
          //Thrust to weight ratio:

          //Stage Indepth View:
          //(1) SIC1
          //Fuel:
          //Specific impulse:
          //Wet mass:
          //Dry mass:
          //Acceleration: 
          //Thrust to weight ratio:
          //∆V:
          //Thrust:
          //Current gravity:
          //
          //(2)
          //(3)
          //
          //What action would you like to take? (Editing, re-organizing, and removing stages can all be done in the editing menu, not here)
          //(1) View rocket once in flight (Once the lower stage(s) have been discarded)
          //(3) Export rocket as a text file
          //(4) Go to main menu
          //(5) Exit Rocket-X 
          //1
          //What stage would you like to be the current firing stage?
          //(1) SIC1
          //(2) SII1
          //(3) SIVB1
          //2
          //You have now calculated for these stages (in order): SII1, SIVB1.
          //Rocket Number: 5
          //Stages in order: (SIC1, SII1, SIVB1)
          //Rocket Statistics:
          //Wet Mass:
          //Dry Mass:
          //Acceleration:
          //∆V:
          //Thrust:
          //Thrust to weight ratio:
          //
          //Would you like to run this program again? 1: Yes, 2: No
          //2
          //Thank you for using ROCKET-X, Goodbye!

        } else if (choice == 3) {
          //Edit a rocket (Hell on earth)
        } else if (choice == 4) {
          //Take no action (already doing that)
        }
        break;
      }

      case 5: {
        break;
      }

      case 6: {
        NotgoToMainMenu = false;
        break;
      }

      case 7: {
        std::cout << "\033[2J\033[0;0H";
        std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
        return 0;
        break;
      }

      default: {
        if (NotgoToMainMenu) {
          std::cout << "Would you like to run this program again? 1: Yes, 2: No\n";
          std::cin >> choice;
          if (choice == 2) {
            std::cout << "\033[2J\033[0;0H";
            std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n\n";
            return 0;
          }
        }

        NotgoToMainMenu = true;
        break;
      }
      }
    } else if (choice == 3) {
      std::cout << "\033[2J\033[0;0H";
      std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n";
      return 0;
    } else {
      std::cout << "\033[2J\033[0;0H";
      std::cout << "You have not chosen either 1 or 2, so you have automaticly quit the program.\n";
      std::cout << "Thank you for using ROCKET-X, Goodbye!\n\n";
      return 0;
    }
  }
}

/*
Reminders:
-Do not use RocketObject unless needed to! Use rocketVector instead
-COMMENT!
-For next week: Correct the issues, edit rockets! THis includes moving stages, removing stages, adding stages, 
renaming the rocket, perhaps renaming the stages! 
-Once editing rockets are done, and calculating	 rockets are done, ROCKET-X will be almost complete!
-Do some heavy bugfinding, then when the rocket and stages system is airtight, move on to budgeting menu

Issues:
-Overwriting stages and rockets just generally are flawed and need fixing
-There are lots of issues, if I push the program basicly at all it just returns to the main menu, it's as 
brittle as slate, we need this thing to be iron by the end of the semester! That means lots of bugfinding and
bugfixing to do. Also, accounting for user stupidity like entering a string where it should be an integer
-MAKE EXTRA WEIGHT WORK !! (This is important for DV calculations for the rocket)
-Deleting rockets creates a whitespace in rocket name permanent storage which causes seg fault

TODO Long term
-Give sources for more details
-Add budgeting menu (distance this stage can travel)

TODO short term
-Make rocket editor
-Rename a stage

IDEAS
-For stages, there should be a rocket, and then you can choose premade stages you have made into the rocket, and order them around.
So what this means, is that you can make a stage, put in all the values for that stage, then make as many as you want.
Then you can go to the stage editor and then create a new rocket, and put all of the stages into the rocket in any order you want.
You will need to be able to remove stages, move stages around, delete and create rockets, replace stages, and delete and create new stages
All of this will be stored in files that will be created and destroyed. There will be a file for each rocket, and each stage. 
These will include details needed for the rocket and the stage alike. 
-It is calculated at runtime, not stored in the text file (for simplicity)

-Have an option to add additional weight inbetween stages.

-For rocket calculations, the rocket needs to know the values for all these variables, for every stage. This does not mean
the specific stage isolated, but included, so stage 1 would be the full rocket, and stage 2 would be the full rocket without stage 1
and stage 3 would be the full rocket without stages 1 and 2...
-The variables I want to have calculated: 
    M0 (Wet mass)
    MF (Dry mass)
    A (Acceleration)
    TWR (Thrust to weight ratio)
    DV (Delta V)
    T (Thrust)
This could be stored in a array, where it retains the same order. This array is part of the rocket class and is
updated with the Calculate() function. The most recent values of this are then writen to the rocket permanenet storage

-You could make an option and it creates a human-readable text file that they can download which details the rocket and stages

-I could have an advanced view where you see every single variable calculated even dumb ones like M1LF

-Expand to a thid part where it calculates orbits, planet tracking, 
launch window, everything to do with spacetravel!

-I have no idea how to do this, but elevate the program, to a visual kinda think with like openGL or some other graphics library

*/