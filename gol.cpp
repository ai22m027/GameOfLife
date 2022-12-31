#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "Timing.h"

std::vector<std::vector<char*>> getNeighbours(const int& rows, const int& cols, const int& rowP, const int& colP,std::vector<std::vector<std::vector<char>>> &grid)
{
    std::vector<std::vector<char*>> neighbors(2, std::vector<char*>());
    int rowN;
    int colN;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            rowN = rowP + i;
            colN = colP + j;

            if (rowP + i < 0)
            {
                rowN = rows-1;
            }
            if (rowP + i >= rows)
            {
                rowN = 0;
            }
            if (colP + j < 0)
            {
                colN = cols-1;
            }
            if (colP + j >= cols)
            {
                colN = 0;
            }

            neighbors[0].push_back(&grid[rowN][colN][0]);
            neighbors[1].push_back(&grid[rowN][colN][1]);
        }
    }
    return neighbors;
}

void loadGolFile(const std::string& filename, std::vector<std::vector<std::vector<char>>>&grid, std::vector<std::vector<std::vector<std::vector<char*>>>>&gridLink)
{
   
    std::ifstream file(filename);

    int numRows;
    int numCols;

    if (file.is_open()) 
    {
        // Read the first line to get the number of rows and columns
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        
        std::string numColsStr;
        if (std::getline(ss, numColsStr, ',') && (ss >> numRows)) 
        {
            numCols = std::stoi(numColsStr);

            // Resize the grid to the correct size
            grid.resize(numRows);
            for (auto& row : grid) 
            {
                row.resize(numCols);
                for (auto& fields : row)
                {
                    fields.resize(2);
                }
            }

            gridLink.resize(numRows);
            for (auto& row:gridLink)
            {
                row.resize(numCols);
                for (auto& fields : row)
                {
                    fields.resize(2);
                }
            }
            
            // Read the characters from the file into the grid
            for (int i = 0; i < numRows; i++) 
            {
                std::getline(file, line);
                for (int j = 0; j < numCols; j++) 
                {
                    grid[i][j][0] = line[j];
                    gridLink[i][j] = getNeighbours(numRows, numCols, i, j, grid);
                }
            }
        }
        else 
        {
            // First line of the file is invalid
        }
    }
    else 
    {
        // File could not be opened
    }
}

void storeGolFile(const std::vector<std::vector<std::vector<char>>>& grid, const std::string& filename, char &target) {
    std::ofstream file(filename);
    if (file.is_open()) {
        // Write the number of rows and columns in the first line
        file << grid[target].size() << ',' << grid.size() << '\n';

        // Write the characters from the grid to the file
        for (const auto& row : grid) {
            for (const auto& c : row) {
                file << c[target];
            }
            file << '\n';
        }
    }
    else {
        // File could not be opened
    }
}

char rulesOfLife(std::vector<char*, std::allocator<char*>>& neighborList, char &cell)
{
    int alive = 0;
    for (int i = 0; i < 8; i++)
    {
        if (*neighborList[i] == 'x')
        {
            alive += 1;
        }
    }
    if (cell =='x')
    {
        if (alive == 2 || alive == 3)
        {
            return 'x';
        }
        else
        {
            return '.';
        }
    }
    else
    {
        if (alive == 3)
        {
            return 'x';
        }
        else
        {
            return '.';
        }
    }
}


void startGameOfLife(std::vector<std::vector<std::vector<char>>> &gridIn, std::vector<std::vector<std::vector<std::vector<char*>>>> &neighbors, int nCycles, char &target)
{
    int nRows = gridIn.size();
    int nCols = gridIn[0].size();
    char t1 = 0;
    char t2 = 1;
 
    for (int k = 0; k < nCycles; k++)
    {
        for (int i = 0; i < nRows; i++)
        {
            for (int j = 0; j < nCols; j++)
            {
                gridIn[i][j][t2] = rulesOfLife(neighbors[i][j][t1], gridIn[i][j][t1]);
                target = t2;
                std::swap(t1, t2);
            }
        }
        /*
        for (int i = 0; i < nRows; i++)
        {
            for (int j = 0; j < nCols; j++)
            {
                gridIn[i][j][0] = gridIn[i][j][1];
            }
        }*/
    }
}

void printGoL(std::vector<std::vector<std::vector<char>>>& gridIn)
{
    for (int i = 0; i < gridIn.size(); i++)
    {
        for (int j = 0; j < gridIn[0].size(); j++)
        {
            std::cout << gridIn[0][i][0];
        }
        std::cout << std::endl;
    }

}

int main(int argc, char* argv[])
{
    std::string inputFile = "random250_in.gol";
    std::string outputFile = "test.gol";
    int generations = 250;
    char target = 0;
    bool time = false;

    std::cout << "You have entered " << argc << " arguments:" << "\n";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--load") {
            if (i + 1 < argc) {
                inputFile += argv[++i];
            }
        }
        else if (arg == "--save") {
            if (i + 1 < argc) 
            {
                outputFile.clear();
                outputFile += argv[++i];
            }
        }
        else if (arg == "--generations") {
            if (i + 1 < argc) 
            {
                generations = std::stoi(argv[i + 1]);
                ++i;
            }
        }
        else if (arg == "--measure") {
            time = true;
        }
    }

    if (time)
    {
        Timing* timing = Timing::getInstance();

        timing->startSetup();
        std::vector<std::vector<std::vector<char>>> gridIn;
        std::vector<std::vector<std::vector<std::vector<char*>>>> gridLink;
        loadGolFile(inputFile, gridIn, gridLink);
        timing->stopSetup();

        timing->startComputation();
        startGameOfLife(gridIn, gridLink,generations, target);
        timing->stopComputation();

        timing->startFinalization();
        storeGolFile(gridIn, outputFile, target);
        timing->stopFinalization();

        std::string output = timing->getResults();
        std::cout << output << std::endl;
    }
    {
        std::vector<std::vector<std::vector<char>>> gridIn;
        std::vector<std::vector<std::vector<std::vector<char*>>>> gridLink;
        loadGolFile(inputFile, gridIn, gridLink);
        startGameOfLife(gridIn, gridLink, generations, target);
        storeGolFile(gridIn, outputFile, target);
    }
    return 0;
}