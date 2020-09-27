#include <iostream>
#include "image.h"
#include <vector>
#include <string>

using namespace std;

int main()
{
    bool done = false;
    while (!done)
    {
        // Main menu
        cout << "Image Editor" << endl;
        cout << "1) Vignette" << endl;
        cout << "2) Clarendon" << endl;
        cout << "3) Grayscale" << endl;
        cout << "4) Rotate 90 Degrees" << endl;
        cout << "5) Rotate 90 Degrees n Times" << endl;
        cout << "6) Enlarge" << endl;
        cout << "7) High Contrast" << endl;
        cout << "8) Lighten" << endl;
        cout << "9) Darken" << endl;
        cout << "10) Black, white, red, green, and blue only" << endl;
        cout << "Please enter the number of your desired process (or Q to quit): ";

        int selection; // Take selection from user
        cin >> selection;

        if (cin.fail()) // If input fails, quit the loop
        {
            cout << "Quitting..." << endl;
            done = true;
        }

        else if (selection > 10)
        {
            cout << "Error! Invalid selection." << endl;
        }

        else
        {
            cout << endl << "Please enter the input BMP file name: ";
            string infile_name;
            cin >> infile_name;
            vector<vector<vector<int> > > input_image = read_image(infile_name);

            cout << "Please enter the output BMP file name: ";
            string outfile_name;
            cin >> outfile_name;

            switch (selection)
            {
                case 1 : write_image(outfile_name, process_1(input_image)); break;

                case 2 : write_image(outfile_name, process_2(input_image)); break;

                case 3 : write_image(outfile_name, process_3(input_image)); break;

                case 4 : write_image(outfile_name, process_4(input_image)); break;

                case 5 : 
                    cout << "Enter the number of rotations: "; 
                    int n; cin >> n;
                    write_image(outfile_name, process_5(input_image, n)); break;

                case 6 :
                    cout << "Enter the x scale, as an integer: ";
                    int x; cin >> x;
                    cout << "Enter the y scale, as an integer: ";
                    int y; cin >> y;
                    write_image(outfile_name, process_6(input_image, x, y)); break;

                case 7 :
                    write_image(outfile_name, process_7(input_image)); break;

                case 8 :
                    cout << "Enter a scaling factor between 0 and 1: ";
                    double scale; cin >> scale;
                    write_image(outfile_name, process_8(input_image, scale)); break;

                case 9 :
                    cout << "Enter a scaling factor between 0 and 1: ";
                    double scale_factor; cin >> scale_factor;
                    write_image(outfile_name, process_9(input_image, scale_factor)); break;

                case 10 : write_image(outfile_name, process_10(input_image)); break;
            }

            cout << endl << "Operation successful!" << endl << endl;
        }
        

    }
    return 0;
}