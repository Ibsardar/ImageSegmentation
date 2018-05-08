/*
*   Author: Ibrahim Sardar
*   04/21/2018
*/

#include "pnmio.h"  /* REQUIRES C++11 */
//#include "llfd.h"   /* LOW LEVEL FEATURE DETECTION FUNCTIONS HERE */
//#include "llip.h"   /* LOW LEVEL PROCESSING FUNCTIONS HERE */
#include "llis.h"   /* LOW LEVEL IMAGE SEGMENTATION FUNCTIONS HERE */

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include "math.h"

// helpers
void say(std::string s) {std::cout<<s<<std::endl;}
void save(Image img, std::string original, pnmio image_manager) {
    // ask for new filename for processed image
    say("Enter a name for the processed version of " + original);
    std::string newfilename;
    std::getline(std::cin, newfilename);
    try {
        image_manager.store_image(newfilename, img);
        say("\""+newfilename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+newfilename+"\" failed to save.");
    }
}
void quicksave(Image img, std::string filename, std::string addon, pnmio image_manager) {
    // insert add-on before ".xxx" and save
    int dot_index = (int) filename.find_last_of(".");
    try {
        image_manager.store_image(filename.insert(dot_index, addon), img);
        say("\""+filename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+filename+"\" failed to save.");
    }
}
std::string i2s(int i) {
	std::stringstream ss;
	ss << i;
	return ss.str();
}
std::vector<std::string> batch_load_images(std::vector<Image>& imgs) {

    std::vector<std::string> filenames;

    while (true) {

        say("\nEnter next filename:");
        std::string filename;
        std::getline(std::cin, filename);

        if (filename == "q" || filename == "quit")
            break;

        pnmio manager;
        Image input;
        std::ifstream iftmp(filename.c_str());
        if (iftmp.good()) {
            try {input = manager.load_image(filename);}
            catch (std::exception &e) {say(e.what()); break;};
        } else {
            say("File does not exist...");
            continue;
        }

        if (input.type == 1) {
            manager.convert_rgb_to_gray(input);
            say("RGB image detected; converted to gray-scale for compatibility.");
        } else if (input.type == 0) {
            say("Gray-scale image detected.");
        } else {
            say("Unknown image-type detected; some features may behave unexpectedly.");
        }

        imgs.push_back(input);
        filenames.push_back(filename);
    }
    return filenames;
}

int main() {

    // print instructions
    say("Welcome to the Image Segmentation Descriptor.\n"
        "This program offers 1 image segmentation function that outputs multiple items.\n"
        "Type 'q' or 'quit' to quit.");

    // menu
    while (true) {

        // ask for filename to test
        say("\nEnter a filename (in this exe's directory) of an image to process:");
        std::string filename;
        std::getline(std::cin, filename);

        // check quit flag
        if (filename == "q" || filename == "quit")
            break;

        // read image
        pnmio manager;
        Image input;
        try {input = manager.load_image(filename);}
        catch (std::exception &e) {say(e.what()); break;};

        // notify the user the image is being converted to gray-scale if it is RGB; warn/notify otherwise
        if (input.type == 1) {
            manager.convert_rgb_to_gray(input);
            say("RGB image detected; converted to gray-scale for compatibility.");
        } else if (input.type == 0) {
            say("Gray-scale image detected.");
        } else {
            say("Unknown image-type detected; some features may behave unexpectedly.");
        }

        // ask for processing type
        say("\nEnter a processing type: (type the index)\n"
            "0: Batch Image Segmentation (multiple outputs for multiple images - press q to stop)\n"
            "1: Image Segmentation (multiple outputs)\n");
        std::string ptype;
        std::getline(std::cin, ptype);
        say("");

        // check quit flag
        if (filename == "quit" || filename == "QUIT")
            break;

        // process image and save it as a new image file
        switch (std::stoi(ptype)) {
        case 0: {
            // get all the user images
            std::vector<Image> batch;
            batch.push_back(input);
            std::vector<std::string> batch_filenames = batch_load_images(batch);

            for (int j=0; j<batch.size(); j++) {
                std::string f;
                if (j>0)
                    f = batch_filenames[j-1];
                else
                    f = filename;
                std::vector<Image> step_results = llis::segment_image(batch[j], f+".descriptors.txt");
                // save all result images
                for (int i=0; i<step_results.size(); i++)
                    if (j>0)
                        quicksave(step_results[i], batch_filenames[j-1], "."+i2s(i), manager);
                    else
                        quicksave(step_results[i], filename, "."+i2s(i), manager);
            }
            break;

        } case 1: {
            std::vector<Image> step_results = llis::segment_image(input, filename+".descriptors.txt");
            // save all result images
            for (int i=0; i<step_results.size(); i++)
                quicksave(step_results[i], filename, "."+i2s(i), manager);
            break;

        } default:
            say("Input could not be understood.");
            break;
        }//end switch
    }

    // end program
    return 0;
}
