#include <vector>   // vector, push_back, at
#include <string>   // string
#include <iostream> // cin, getline
#include <fstream>  // ofstream
#include <unistd.h> // getopt, exit, EXIT_FAILURE
#include <assert.h> // assert
#include <thread>   // thread, join
#include <sstream>  // stringstream

#include "BoundedBuffer.h" // BoundedBuffer class

#define MAX_MSG_LEN 256

using namespace std;

/************** Helper Function Declarations **************/

void parse_column_names(vector<string> &_colnames, const string &_opt_input);
void write_to_file(const string &_filename, const string &_text, bool _first_input = false);

/************** Thread Function Definitions **************/

// "primary thread will be a UI data entry point"
void ui_thread_function(BoundedBuffer *bb)
{
    // TODO: implement UI Thread Function
    string user_input; // create string to hold user input
    while (true)
    {
        cout << "enter data> ";
        getline(cin, user_input);

        // TODO: "Once the user has entered 'Exit', the main thread will
        // "send a signal through the message queue to stop the data thread"
        if (user_input == "Exit")
        {
            char exitMsg[] = "EXIT";
            bb->push(exitMsg, sizeof(exitMsg));
            break;
        }
        else
        {
            vector<char> input(user_input.begin(), user_input.end()); // vector to hold user input
            input.push_back('\0');                                    // Add null terminator
            bb->push(input.data(), input.size());                     // push to BoundedBuffer
        }
    }
}

// helper for output formatting
string format_csv_line(const vector<string> &data, size_t columns)
{
    stringstream ss;
    for (size_t i = 0; i < data.size(); ++i)
    {
        ss << data[i];
        // Add a comma and a space only if it's not the last item in the row
        // or if the row is incomplete and this is the last available item
        if (i < data.size() - 1 || (data.size() < columns && i == data.size() - 1))
        {
            ss << ", ";
        }
    }
    ss << "\n";
    return ss.str();
}

// "second thread will be the data processing thread"
// "will open, write to, and close a csv file"
void data_thread_function(BoundedBuffer *bb, string filename, const vector<string> &colnames)
{
    // TODO: implement Data Thread Function
    // (NOTE: use "write_to_file" function to write to file)
    char buffer[MAX_MSG_LEN];
    int length;
    bool first_input = true;    // first input flag
    vector<string> row_data;    // store individual data entries

    string header = format_csv_line(colnames, colnames.size());
    write_to_file(filename, header, first_input);

    while (true)
    {
        length = bb->pop(buffer, MAX_MSG_LEN);
        string data(buffer, length - 1); // "length - 1" to account for null terminator

        // EXIT signal handler
        if (data == "EXIT")
        {
            // handling incomplete data row entries
            if (!row_data.empty())
            {
                string csv_line = format_csv_line(row_data, colnames.size());

                // check for an remove extra newline
                if (!csv_line.empty() && csv_line.back() == '\n')
                {
                    csv_line.pop_back();
                }
                write_to_file(filename, csv_line, false);
            }
            break;
        }

        row_data.push_back(data);   // add data to current row

        // check if row is finished and ready to write
        if (row_data.size() == colnames.size())
        {
            string csv_line = format_csv_line(row_data, colnames.size());
            write_to_file(filename, csv_line, false); // write the row to file
            row_data.clear();                         // clear for next row
        }
    }
}

/************** Main Function **************/

int main(int argc, char *argv[])
{

    // variables to be used throughout the program
    vector<string> colnames;                  // column names
    string fname;                             // filename
    BoundedBuffer *bb = new BoundedBuffer(3); // BoundedBuffer with cap of 3

    // read flags from input
    int opt;
    while ((opt = getopt(argc, argv, "c:f:")) != -1)
    {
        switch (opt)
        {
        case 'c': // parse col names into vector "colnames"
            parse_column_names(colnames, optarg);
            break;
        case 'f':
            fname = optarg;
            break;
        default: // invalid input, based on https://linux.die.net/man/3/getopt
            fprintf(stderr, "Usage: %s [-c colnames] [-f filename]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // TODO: instantiate ui and data threads
    thread ui_thread(ui_thread_function, bb);
    thread data_thread(data_thread_function, bb, ref(fname), ref(colnames));

    // TODO: join ui_thread
    ui_thread.join();

    // TODO: join data thread
    data_thread.join();

    // CLEANUP: delete members on heap
    delete bb;
}

/************** Helper Function Definitions **************/

// function to parse column names into vector
// input: _colnames (vector of column name strings), _opt_input(input from optarg for -c)
void parse_column_names(vector<string> &_colnames, const string &_opt_input)
{
    stringstream sstream(_opt_input);
    string tmp;
    while (sstream >> tmp)
    {
        _colnames.push_back(tmp);
    }
}

// function to append "text" to end of file
// input: filename (name of file), text (text to add to file), first_input (whether or not this is the first input of the file)
void write_to_file(const string &_filename, const string &_text, bool _first_input)
{
    // based on https://stackoverflow.com/questions/26084885/appending-to-a-file-with-ofstream
    // open file to either append or clear file
    ofstream ofile;
    if (_first_input)
        ofile.open(_filename);
    else
        ofile.open(_filename, ofstream::app);
    if (!ofile.is_open())
    {
        perror("ofstream open");
        exit(-1);
    }

    // sleep for a random period up to 5 seconds
    usleep(rand() % 5000);

    // add data to csv
    ofile << _text;

    // close file
    ofile.close();
}