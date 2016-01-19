/**
 * $Id: generator.cpp 701 2014-09-18 16:23:03Z klugeflo $
 * @file generator.cpp
 * @brief Task set generator
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <iostream>
#include <vector>
#include <core/scobjects.h>
#include <xmlio/tasksetwriter.h>
#include <xmlio/xmlutils.h>
#include <taskmodels/taskmodels.h>
#include <utility/aggregators.h>
#include <utility/calculators.h>

#include <generator/mkgenerator.h>

using namespace std;
using namespace tmssim;

/**
 * Parses the command-line arguments and sets the output filename if specified
 * @param argc Count of the arguments
 * @param argv Pointer to the char-array with the arguments
 * @param filename The file to which the generated xml should be written
 * @return false on error, true otherwise
 */
bool parseArgs(int argc, char *argv[], string& filename, int& num);

/**
 * Generates an xml-taskset
 */
int main(int argc, char*argv[]) {

  unsigned int seed = time(NULL);
  //if (argc > 1)
    //seed = strtoul(argv[1], NULL, 10);

  MKGenerator mkgen(seed);

  // to add some randomization to the system
  srand(seed);

    string filename;
    int num=30;
    if(parseArgs(argc,argv,filename, num)) {

        // Create Taskset
        UtilityCalculator* ucPtr = new UCFirmRT();
        //UtilityAggregator* uaPtr = new UAExp(0.1);
        UtilityAggregator* uaPtr = new UAWMean(10);
        //UtilityAggregator* uaPtr = new UAMean();

        vector<Task*> taskset;

        // Periodic Tasks
        for(int taskNo=1; taskNo<=num; taskNo++) {
            int et = rand() % 5 + 1;
            int ct = et;
            int period = rand() % 20 + 1;
            int prio = 1;
            int matrixSize = rand() % 50 + 1;
            int offset = 0;
            taskset.push_back(new PeriodicLoadTask(taskNo,
                                                   period,
                                                   et,
                                                   ct,
                                                   ucPtr->clone(),
                                                   uaPtr->clone(),
                                                   offset,
                                                   prio,
                                                   matrixSize));
        }

        // Write Taskset
        TasksetWriter* writerPtr = TasksetWriter::getInstance();
        if (!writerPtr->write(filename, taskset)) {
            cerr << "Error while writing tasks to file: " << filename << "!" << endl;
        }
        cout << "Wrote Tasks #" << taskset.size() << " to file: " << filename << endl;

        // Free Taskset
        for (std::vector<Task*>::iterator it = taskset.begin(); it != taskset.end(); ++it)
            delete *it;
        delete ucPtr;
        delete uaPtr;

        // Free xmlWriter
        delete writerPtr;
    }
  /*
   */
    return 0;
}

bool parseArgs(int argc, char *argv[], string& filename, int& num) {

    char *str;
    char sw;
    int p = 1;

    while (p < argc) {
        if (argv[p][0] == '-') {
            sw = argv[p][1];
            if (sw == '\0') {
                // empty parameter?
                cerr << "Received stray parameter: -\n";
                return false;
            }

            if (argv[p][2] != '\0') { // -sstring
                str = &argv[p][2];
            } else {
                if (++p < argc) { // -s string
                    str = argv[p];
                } else {
                    // ???? problem!!!
                    str = NULL;
                    // TODO: this can lead to SIGSEGV below!
                }
            }

            switch (sw) {
                case 'o': // Ouput selection
                {
                    filename = str;
                    break;
                }
                case 'n':
                {
                  num = atoi(str);
                  break; 
                }
                default:
                {
                    cerr << "Unknown option -" << sw << "!" << endl;
                    return false;
                }
            }
            ++p;
        } else {
            // failure
            cerr << "Invalid paramter format" << endl;
            return false;
        }
    } // end while

    return true;
}
