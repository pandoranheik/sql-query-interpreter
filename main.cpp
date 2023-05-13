#include <iostream>
#include <string>
#include <fstream>
#define ARSIZE 5500
#define COLMAX 37
// Used to convert query into an array for each segment
#define wordMax 1000
class Query{
    public:
        // Col count
        void setColCount(int value){
            colCount = value;
        }
        int getColCount(){
            return colCount;
        }
        // Where count
        void setWhereCount(int value){
            whereCount = value;
        }
        int getWhereCount(){
            return whereCount;
        }
        // ANDOR count
        void setANDORcount(int value){
            ANDORcount = value;
        }
        int getANDORcount(){
            return ANDORcount;
        }
        // Col list
        void setColList(std::string value, int index){
            colList[index] = value;
        }
        std::string getColList(int index){
            return colList[index];
        }
        // Where left list
        void setWhereLeft(std::string value, int index){
            whereLeft[index] = value;
        }
        std::string getWhereLeftList(int index){
            return whereLeft[index];
        }
        // Where right list
        void setWhereRight(std::string value, int index){
            whereRight[index] = value;
        }
        std::string getWhereRightList(int index){
            return whereRight[index];
        }
        // Logic list
        void setLogicList(std::string value, int index){
            logicList[index] = value;
        }
        std::string getLogicList(int index){
            return logicList[index];
        }
        // And Or list
        void setANDORlist(std::string value, int index){
            ANDORlist[index] = value;
        }
        std::string getANDORlist(int index){
            return ANDORlist[index];
        }
        // Keywords
        bool findKeywords(std::string key){
            int wordSize = sizeof(keywords) / sizeof(std::string);
            for(int count = 0; count < wordSize; count++)
                if(keywords[count] == key) return true;
            return false;
        }
        // Logic Keywords
        bool findLogicKeywords(std::string key){
            int wordSize = sizeof(logicKeywords) / sizeof(std::string);
            for(int count = 0; count < wordSize; count++)
                if(logicKeywords[count] == key) return true;
            return false;
        }
        // Logic Operators
        bool findLogicOperators(std::string key){
            int wordSize = sizeof(logicOperators) / sizeof(std::string);
            for(int count = 0; count < wordSize; count++)
                if(logicOperators[count] == key) return true;
            return false;
        }
    private:
        int colCount = 0;
        int whereCount = 0;
        int ANDORcount = 0;
        std::string colList[COLMAX] = {"","","","","","","","","","",
                                "","","","","","","","","","",
                                "","","","","","","","","","",
                                "","","","","","",""};
        std::string whereLeft[5] = {"","","","",""};
        std::string whereRight[5] = {"","","","",""};
        std::string logicList[5] = {"","","","",""};
        std::string ANDORlist[4] = {"", "", "", ""};
        // SQL Keys to look for
        std::string keywords[3] = {"select","from","where"};
        std::string logicKeywords[2] = {"and","or"};
        std::string logicOperators[6] ={"==","!=",">=","<=",">","<"}; 
};
std::string rawData[ARSIZE][COLMAX];
char delimiter = '\t';
//reads in our data, data is separated by tabs and new lines
void readData(std::string [ARSIZE][COLMAX]);
//Opens file output.txt and writes output. 
void writeData(std::string [ARSIZE][COLMAX]);
//lowercases a given std::string, useful for error checking
std::string lower(std::string);
void runQuery(std::string &, std::string [][COLMAX]);
void seperate(std::string , std::string [wordMax], int&);
//Checks the syntax of the query, returns 1 if error, 0 if gucci
//has already been lower case so can check easier. will return
//as soon as error is detected.
// Returns false if there is no closing quote
bool findClosing(std::string);
bool checkSyntax(std::string);
//Fills struct in with all the query data, neatly organized.
//could have done this when checking syntax, but meh
void parseQuerytoStruct(Query&, std::string);
std::string output[ARSIZE][COLMAX];
//makes our life easier
int getColNum(std::string, std::string [][COLMAX]);
//Creates output with results
bool generateResults(Query, std::string [][COLMAX]);
// Returns true is it finds a matching row
bool match(int, int[ARSIZE]);
/* Finds all where conditions given the OR operator
   Then updates chosenRows and chosenRowsCount */
void findOrs(int, int, int&, int [ARSIZE],
             std::string [][COLMAX], int [COLMAX],Query);
//First we find the matching rows then we print out
//only the rows selected, unless colList is 0 then we print
//all of them out.
//Note if a col name doesn't match we just error out
//if your local output array segfaults, make it global like I put above
int main(){
    readData(rawData);
    writeData(rawData); //output.txt
    std::cout << "Welcome to the Satellite Database at home"<< std::endl;
    std::string query = "";
    for(int i=0; i < ARSIZE; i++) //Clean up output global array
    for(int j=0; j < COLMAX; j++) output[i][j] = "";
    std::ofstream fout; fout.open("queryoutput.txt"); //Cleanup
    fout << "Queries:" << std::endl; fout.close(); //Cleanup output file
    //query="SELECT \"Users\" FROM DB;"; //Debug
    //runQuery(query, rawData); return 0;//Debug
    while(query != "q"){
    std::cout << "Please enter your query:" << std::endl;
    std::getline(std::cin, query);
    runQuery(query, rawData); //pass by ref
    }
    return 0;
}
void readData(std::string rawData[ARSIZE][COLMAX]){
    std::ifstream inFile;
    inFile.open("UCS-Satellite-Database-5-1-2022.txt");
    // Used to store each value in the file
    std::string cell = "";
    // Check for error opening file
    if(inFile.fail())(std::cerr << "\nError opening file\n", exit(1));
    /*
    Loops throught the file and creates a 2D array
    where there will a constant row size of 5500
    and a constant column size of 37, we have filled
    exactly 37 columns in a row then we move on to the next row
    using a newline as a delimiter instead of the tab delimiter
    */
    for(int row = 0; row < ARSIZE; row++){
        for(int col = 0; col < COLMAX; col++){
            /* 
            Using the tab delimiter, we can get
            the values of the file
            */ 
            std::getline(inFile, cell, delimiter);
            // Save cell value into the matrix
            rawData[row][col] = cell;
        } 
        // Moves onto the next line in the file
        getline(inFile, cell);
    }
    inFile.close();
}
void writeData(std::string rawData[ARSIZE][COLMAX]){
    std::ofstream outFile("output.txt");
    /*
    Loops throught the rawData matrix
    created by the readData function and writes
    the values into a file called (output.txt).
    This was used to to check if my data was clean
    since it had to exactly match what was on the excel
    sheet
    */
    for(int row = 0; row < ARSIZE; row++) {
        for(int col = 0; col < COLMAX; col++){
            /*
            Write to the output.txt file with the delimiter
            to seperate values being a tab
            */
            outFile << rawData[row][col] << "*N*";
            }
        // Create a new line to indicate a new row
        outFile << "\n*Z*";
    }  
    outFile.close();
}
// Converts std::string to all lowercase
std::string lower(std::string query) {
    for(int count = 0; count < query.length(); count++)
        query.at(count) = tolower(query.at(count));
    return query;
}
void runQuery(std::string &query, std::string db[][COLMAX]){
    if(lower(query) == "q"){
        query = "q"; //lowercase it so while in main picks it up
        return;
    }//Exit command so we are done here.
    //lowercase version so it's ez
    if(checkSyntax(lower(query))) {
        std::cout << "Error: Invalid Query Syntax. "
        << "Get motivated. Try Again!" << std::endl;
        return; //if true there is syntax error
    }
    //ok now that we know query is ok we can parse and prepare
    Query q; //the struct with the data to query
    parseQuerytoStruct(q, query); //remember to handle separately *
    // now we can actually return the values for it.
    if(!generateResults(q, db))
        std::cout << "Error: Invalid Query Semantic. "
        << "Get motivated. Try Again!" << std::endl;
    return;
};
void seperate(std::string text, 
              std::string list[wordMax], 
              int& lastIndex){
    int currentListIndex = 0;
    while(true){
        std::string token;
        // If unable to find another delimiter then the loop ends
        if(text.find(' ') != std::string::npos){
                if(text[0] == '"'){
                    // Find and delete the first quotation from the text
                    text.erase(0, text.find('"') + 1);
                    // Set token to keyword
                    token = text.substr(0, text.find('"')+1);
                    // Find and delete the whole keyword from the text
                    text.erase(0, text.find('"') + 1);
                    // If there is no comma then move out of list
                    if(text[0] != ',') text.erase(0, 1); 
                    // Set the query array to the token
                    list[currentListIndex] = '"' + token;
                }else{  
                    // find the first character that is a space
                    token = text.substr(0, text.find(' '));
                    // Push list to token
                    list[currentListIndex] = token;
                    // Get rid of what was put into the list
                    // Then move on
                    text.erase(0, text.find(' ') + 1);
                }
            // SELECT "User Hero" FROM db
            currentListIndex++;
        }else{
            if(text[0] == '"'){
                    // Find and delete the first quotation from the text
                    text.erase(0, text.find('"') + 1);
                    // Set token to keyword
                    token = text.substr(0, text.find('"') + 1);
                    // Find and delete the whole keyword from the text 
                    // Except the last character
                    text.erase(0, text.find('"') + 1);
                    // Push list to token
                    list[currentListIndex] = '"' + token;
            }else{
                // Add keyword to list omitting the semicolon
                 list[currentListIndex] = text.substr(
                        0, text.find(';')
                    );
                 text.erase(0, text.find(';'));
            }
            currentListIndex++;
            // Push the last character of the std::string text to list
            // To find end of list
            list[currentListIndex] = text[0];
            lastIndex = currentListIndex;
            break;
        }
    }
}
// Find closing quotation
bool findClosing(std::string text){
    text.erase(text.find('"'),1);
    if(text.find('"') == std::string::npos) return false;
    return true;
}
bool checkSyntax(std::string query){
    // Initialize for list of keywords and operators
    std::string queryList[wordMax];
    int lastIndex = 0;
    // Run to make an array of keywords and operators
    seperate(query, queryList, lastIndex);
    // Check if any keywords indicators match at give position
    if(queryList[0] != "select" || queryList[lastIndex] != ";" ||
        (queryList[1][0] != '"' && queryList[1] != "*")
    ){
        // Return true conditions are not met
        // This throws an error
        return true;
    }
    // Checks for closing quotation
    if(queryList[1][0] == '"')
        if((!findClosing(queryList[1]))) return true;
    bool selectSectionDone = false;
    bool fromSectionDone = false;
    bool whereSectionDone = false;
    /* 
    If there is no comma in the 
    third index then assume the 
    select section is done
    */
    if(queryList[2][0] != ',') selectSectionDone = true;
    // Start at section where a comma or the FROM keyword would be
    for(int count = 2; true; count++){
        if(queryList[count][0] == ',' && !selectSectionDone){
            count ++;
            if(queryList[count][0] != '"') return true;
            if(!findClosing(queryList[count])) return true;
            // Moves to next column header or FROM keyword
            if(queryList[count + 1] == "from"){
                selectSectionDone = true;
            }
            continue;
        }
        /*
        If FROM is found at the given index of count
        then increment by one to find the database name
        and if the database name exist or matches db then
        finish the from section
        */
        if(queryList[count] == "from" && 
                  selectSectionDone && 
                  !fromSectionDone)
        {
            count++;
            if(queryList[count] != "db") return true;
            fromSectionDone = true;
            if(queryList[count + 1] == ";") return false;
            continue;
        } 
        // Throw error is FROM is not found at given index
        else if(queryList[count] != "from" && 
                  selectSectionDone && 
                  !fromSectionDone)
        {
            return true;
        }
        /*
        Finds WHERE clause at the given index of count.
        If where does not exist then throw an error
        */
        if(((queryList[count] == "where" && 
                  selectSectionDone && 
                  fromSectionDone) &&
                  !whereSectionDone))
        {
            // Loops through all conditions and checks their syntax
            while(true)
            {
                // Moves on to keyword
                count++;
                if(queryList[count][0] != '"') return true;
                if((!findClosing(queryList[count]))) return true;
                // Moves on to logical operator
                count++;
                if(queryList[count] == "==" 
                || queryList[count] == ">=" 
                || queryList[count] == "<=" 
                || queryList[count] == ">" 
                || queryList[count] == "<")
                {    
                    // Moves on to last keyword of the pattern
                    count++;
                    if(queryList[count][0] != '"') return true;
                    if((!findClosing(queryList[count]))) return true;
                    // Check to see if and or or exist
                    count++;
                    if(queryList[count] == "and" 
                    || queryList[count] == "or") continue;
                    // Expect a semicolon to end
                    if(queryList[count] == ";") return false;
                }else{
                    return true;
                }
            } 
        }
        // Throw error is FROM is not found at given index
        if(queryList[count] != "where" && 
                  selectSectionDone && 
                  fromSectionDone)
        {
            return true;
        }
    }
    return false;
}
// Used to get rid of quotations parsed by seperate function
std::string trashQuotes(std::string text){
    return text.substr(1, text.size() - 2);
}
/*
Converts query into an array for each keyword
then saves values into the Query class
*/
void parseQuerytoStruct(Query& q, std::string query){
    // Initialize for list of keywords and operators
    std::string queryList[wordMax];
    int lastIndex = 0;
    bool selectorSectionDone = false;
    bool doesWhereSectionExist = false;
    // Make an array of keywords and operators
    seperate(lower(query), queryList, lastIndex);
    if(queryList[1] == "*"){
        for(int count = 0; count < COLMAX; count++)
             q.setColList(rawData[0][count], count);  
        q.setColCount(COLMAX);
        selectorSectionDone = true;
    }
    for(int count = 1; queryList[count] != ";"; count++){
        if(queryList[count][0] == '"' && !selectorSectionDone){
            // Save value into given index
            q.setColList(
            // Get rid of quotations made by seperate() function
            trashQuotes(queryList[count]),
            q.getColCount()
            );
            q.setColCount(q.getColCount() + 1 );
            if(queryList[count + 1] == ",") {
            count ++;
            }
            continue;
        }
        selectorSectionDone = true;
        if(lower(queryList[count]) == "where" || 
            doesWhereSectionExist == true ){
            // Move index to find next key
            if(lower(queryList[count]) == "where") count++;
            // Used to run this if statement if where exist
            doesWhereSectionExist = true;
            q.setWhereLeft(trashQuotes(queryList[count])
                , q.getWhereCount());
            // Next index should be a logical operator
            count++;
            q.setLogicList(queryList[count], q.getWhereCount());
            // Next index should be the key that we are searching for
            count++;
            q.setWhereRight(trashQuotes(queryList[count])
                , q.getWhereCount());
            q.setWhereCount(q.getWhereCount() + 1);
            if(q.findLogicKeywords(queryList[count + 1])){
                // Move index to logic keyword
                count++;
                q.setANDORlist(queryList[count], q.getANDORcount());
                q.setANDORcount(q.getANDORcount() + 1);
            }else{
                doesWhereSectionExist = false;
            }
        }
    }
};
// Used to get the column header index
int getColNum(std::string key, std::string db[][COLMAX]){
    /* 
    Loops through the whole db matrix
    and locates the index of the key
    which should be a column header
    */
    for(int row = 0; row < ARSIZE; row++)
        for(int col = 0; col < COLMAX; col++)
            if(lower(key) == lower(db[row][col])) return col;
    return -1;
}
// Verifies if a row already exist
bool match(int rowCount, int chosenRows[ARSIZE]){
    for(int row = 0; row < ARSIZE; row++){
        if(rowCount == chosenRows[row]){
            return true;
        }
    }
    // Returns false if row does not already exist
    return false;
}
bool generateResults(Query q, std::string db[][COLMAX]){
    // Used to store all indexes that match the colList value
    int colIndex[COLMAX];
    /* 
    Loops through colList and finds all columns that match
    the first row of the db matrix which should be the 
    column header
    */
    for(int colCount = 0; colCount < 
        q.getColCount(); colCount++){
        colIndex[colCount] = getColNum(
            q.getColList(colCount), db);
        /* 
        If the column header does not exist in the db
        then throw an error/return false
        */
        if (colIndex[colCount] == -1) return false;
    }
    // Used to store all indexes that match the whereLeft value
    int whereColIndex[COLMAX];
    for(int cols = 0; cols < COLMAX; cols++) whereColIndex[cols] = -1;
    /* 
    Loops through whereLeft and finds all columns that match
    the first row of the db matrix
    */
    for(int colCount = 0; colCount < q.getWhereCount(); colCount++){
        whereColIndex[colCount] = getColNum(
            q.getWhereLeftList(colCount), db);
        /* 
        If the column header does not exist in the db
        then throw an error/return false
        */
        if (whereColIndex[colCount] == -1) return false;
    }
    // Rows to be used for the output array
    int chosenRows[ARSIZE];
    // Clean array
    for(int rows = 0; rows < ARSIZE; rows++) chosenRows[rows] = -1;
    // Keeps track of how many rows matched the given condition
    int chosenRowsCount = 0;
    // Loop used to find all matching rows
    for(int rowCount = 0; rowCount < ARSIZE; rowCount++){
        for(int colCount = 0; colCount < q.getWhereCount(); colCount++){
            // Find all rows that meet the given where condition
            // And operator is OR
            findOrs(rowCount,colCount, chosenRowsCount,
            chosenRows, db, whereColIndex, q);
        }
    }
    /* 
    Checks if there where any WHERE conditions in the query 
    and if not then update chosen rows to be all rows
    */
    if(q.getWhereCount() == 0){
        for(int row = 0; row < ARSIZE; row++){
            // Invalidates column headers
            if(row == 0) continue;
            chosenRows[chosenRowsCount] = row;
            chosenRowsCount++;
        }
    }
    /*
    After we have grabbed all the column indexes and where indexes
    then we will begin to create the output array in order to
    write to the output file 
    */
    for(int rowCount = 0; rowCount < chosenRowsCount; rowCount++){
        for(int colCount = 0; colCount < q.getColCount(); colCount++){
            // Create the output array
            // We will use it write to the output file 
            if(colCount > 30 && 
            /*  Since the rows sometimes duplicated after
                a certain amount of columns, I added this
                guard to make sure there can be no duplicates
                for cells that are supposed to be empty values
            */
               db[chosenRows[rowCount]][colIndex[colCount]] == 
               db[chosenRows[rowCount]][colIndex[colCount - 1]]){
               output[rowCount][colCount] = "";
            }else{
                // Write values to output array
                output[rowCount][colCount] = 
                db[chosenRows[rowCount]][colIndex[colCount]];
            }
        }
    }
    // Output to queryoutput.txt file
    std::ofstream outFile;
    // Create header for output file
    outFile.open("queryoutput.txt");
    std::string line;
    outFile << "Queries:\nQuery:\n";
    // Display all columns that were chosen
    for(int count = 0; count < q.getColCount(); count++){
        outFile << db[0][colIndex[count]] << delimiter;
    }
    outFile << '\n';
    // Write to output file
    // With the indexes that we grabbed before
    for(int rowCount = 0; rowCount < chosenRowsCount; rowCount++){
        for(int colCount = 0; colCount < q.getColCount(); colCount++){
            outFile << output[rowCount][colCount]  << delimiter;
        }
        outFile << '\n';
        if(rowCount == chosenRowsCount - 1) 
        outFile  << "Total Rows: " << chosenRowsCount;
    }
    outFile.close();
    return true;
}
/* Finds all where conditions given the OR operator
   Then updates chosenRows and chosenRowsCount */
void findOrs(int rowCount, int colCount, 
             int& chosenRowsCount, int chosenRows[ARSIZE],
             std::string db[][COLMAX], int whereColIndex[COLMAX],
             Query q)
{
    // Values used to compare strings
    // Ten was chosen to match codegrade
    int positiveValue = 10;
    int negativeValue = -10;
    // Checks for the equality operator of the query
    if(q.getLogicList(colCount) == "=="){
        if(lower(db[rowCount][whereColIndex[colCount]]) == 
            lower(q.getWhereRightList(colCount)))
        {
        if(match(rowCount, chosenRows) && colCount != 0) return;
        chosenRows[chosenRowsCount] = rowCount;
        chosenRowsCount++;
        }
    }
    /* 
    Checks for the greater than 
    or equal to operator of the query
    */
    if(q.getLogicList(colCount) == "<="){
        if(rowCount < 1) return;
        std::string whereLeft = db[rowCount][whereColIndex[colCount]];
        std::string whereRight = q.getWhereRightList(colCount);
        if(whereLeft.compare(whereRight) <= positiveValue){
            /* 
            End function if column is not equal to zero
            and match is true to avoid having duplicate rows
            */
            if(match(rowCount, chosenRows) && colCount != 0) return;
            chosenRows[chosenRowsCount] = rowCount;
            chosenRowsCount++;
        }
        return;
    }
    /* 
    Checks for the less than 
    or equal to operator of the query
    */
    if(q.getLogicList(colCount) == ">="){
        if(rowCount < 1) return;
        std::string whereLeft = 
                db[rowCount][whereColIndex[colCount]];
        std::string whereRight = q.getWhereRightList(colCount);
        if(whereLeft.compare(whereRight) >= negativeValue){
            if(match(rowCount, chosenRows) && colCount != 0) return;
            chosenRows[chosenRowsCount] = rowCount;
            chosenRowsCount++;
        }
        return;
    }
    /* 
    Checks for the greater than 
    operator of the query
    */
    if(q.getLogicList(colCount) == "<"){
        if(rowCount < 1) return;
        std::string whereLeft = db[rowCount][whereColIndex[colCount]];
        std::string whereRight = q.getWhereRightList(colCount);
        if(whereLeft.compare(whereRight) < positiveValue){
            if(match(rowCount, chosenRows) && colCount != 0) return;
            chosenRows[chosenRowsCount] = rowCount;
            chosenRowsCount++;
        }
        return;
    }
    /* 
    Checks for the less than 
    operator of the query
    */
    if(q.getLogicList(colCount) == ">"){
        if(rowCount < 1) return;
        std::string whereLeft = db[rowCount][whereColIndex[colCount]];
        std::string whereRight = q.getWhereRightList(colCount);
        if(whereLeft.compare(whereRight) > negativeValue){
            if(match(rowCount, chosenRows) && colCount != 0) return;
            chosenRows[chosenRowsCount] = rowCount;
            chosenRowsCount++;
        }
        return;
    }
};
