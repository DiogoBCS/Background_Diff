
//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <ctime>
using namespace cv;
using namespace std;
using namespace cuda;

// Global variables
Mat frame,original; //current frame
Mat fgMaskMOG2;  //fg mask fg mask generated by MOG2 method
Mat fgMaskMOGBlur;
Mat back;
Ptr<BackgroundSubtractor> pMOG2,pFGD; //MOG2 Background subtractor
int keyboard; //input from keyboard
Rect objectBoundingRectangle = Rect(0,0,0,0);
vector< vector<Point> > contours; 
int decision, camera;
void help();
void processVideo(char* videoFilename);
void help()
{
    cout
    << "--------------------------------------------------------------------------" << endl
    << "This program shows how to use background subtraction methods provided by "  << endl
    << " OpenCV. You can process both videos (-vid) and images (-img)."             << endl
                                                                                    << endl
    << "Usage:"                                                                     << endl
    << "./bs {-vid <video filename>|-img <image filename>}"                         << endl
    << "for example: ./bs -vid video.avi"                                           << endl
    << "or: ./bs -img /data/images/1.png"                                           << endl
    << "--------------------------------------------------------------------------" << endl
    << endl;
}
int main(int argc, char* argv[])
{
    //print help information
    help();
    //check for the input parameter correctness
    if(argc != 3) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    namedWindow("FG Mask MOG 2");
    //create Background Subtractor objects
      
    pMOG2 = createBackgroundSubtractorMOG2(500,16,false); //MOG2 approach  
   // pFGD  = createBackgroundSubtractorFGD( );

    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);

    }
    else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}

void processVideo(char* videoFilename) {
    //create the capture object
    //VideoCapture capture(videoFilename);

    camera = 3;
    VideoCapture capture(camera) ;
   // VideoCapture capture(videoFilename);
      


    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    //read input data. ESC or 'q' for quitting
    while( (char)keyboard != 'q' && (char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(original)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }


        cvtColor( original, frame, CV_BGR2GRAY );
        equalizeHist(frame,frame );

        decision = 1;
        switch(decision){

            decision = 1;

            case 1: {// MOG
                //update the background model
                pMOG2->apply(frame, fgMaskMOG2,0.4);
                pMOG2->getBackgroundImage(back);      
            
                blur(fgMaskMOG2, fgMaskMOGBlur,Size(5,5));
                threshold(fgMaskMOGBlur, fgMaskMOGBlur,60,255,THRESH_BINARY);
                Canny( fgMaskMOGBlur, fgMaskMOGBlur, 30, 90, 3 );
                findContours(fgMaskMOGBlur,contours,RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );

                if(contours.size() != 0){
                    for(int i =0; i < contours.size(); i++ ){
                        if(contourArea(contours[i]) > 10){
                            objectBoundingRectangle = boundingRect(contours[i]);
                            circle(original,Point(objectBoundingRectangle.x,objectBoundingRectangle.y)
                                                                        ,   50,Scalar(0,255,0),2);
                           // cout <<contourArea(contours[i]) << "\n";
                                                      
                        }

                    }              
                }
            }

            case 2:{// FGD


                   





            } 



            //cout << contours.size() << "\n";

            default:{}

         }

        //show the current frame and the fg masks
        imshow("Frame", original);
        imshow("FG Mask MOG", fgMaskMOG2);
        imshow("F Blur", fgMaskMOGBlur);

         //get the input from the keyboard
        keyboard = waitKey( 30 );   
       
    }
    //delete capture object
    capture.release();
}


