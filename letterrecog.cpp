/*A diagnostic program that uses neural nets

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QtGui>
#include <math.h>
#include "neural.h"

using namespace std;

int* getSet();
const int SETLENGTH = 15;
int correct = 0;

//keep track of the command line input
//operation is "sample" "train" or "test"
//symbol is the letter entered when the operation is "sample"
string operation,symbol;

//reads in training sheet from heart.txt, builds a set of neural nets, trains them, and saves the weights to perceptron.txt
void train()
{
	//read the training data from heart.txt
	ifstream datafile;	//file object
	string line;		//lines will go here
	datafile.open("letter.txt");	//open the file
	//stop the program if the file isn't found
	if (!datafile.is_open())
	{
		cout<<"Couldn't open letter.txt"<<endl;
		return;
	}
	int linecount=0;	//keep track of how many samples are in the file
	//go through the file and just count the number of samples
	while(getline(datafile,line))
	{
		linecount++;
	}
	datafile.close();

	//make an array to hold the samples
	int sample_input[linecount][SETLENGTH];
	//make another array to hold the output diagnosis for each sample
	char sample_output[linecount];
	//reopen the file
	datafile.open("heart.txt");
	//for each sample,
	for(int i=0; i<linecount; i++)
	{
		//read it from the file
		getline(datafile,line);
		//the first character is the diagnosis
		sample_output[i]=line[0];
		//convert text 1s and 0s to integers
		for (int j=0; j<SETLENGTH; j++)
		{
			sample_input[i][j]=line[j+2]=='1'?1:0;
		}
	}
	datafile.close();

//TRAINING NETWORKS, THEN SAVING WEIGHTS TO PERCEPTRON ARRAY

	//create array of possible letters
	char numarray[2];
    numarray[0] = '0';
    numarray[1] = '1';
	int choices = 2;
	
	//Instantiate and fill perceptron array 
	Perceptron* perceptronarray[choices];
	for (int n = 0; n<choices; n++)
		perceptronarray[n] = new Perceptron(22);

	//Begin training of perceptrons
	cout<<"Begin training..."<<endl;
	for(int p = 0; p < choices; p++)
	{
		char trainednum = numarray[p];
		for (int q=0; q<100000; q++)
		{
			bool desired = false;
			bool trained = true;
			for(int r = 0; r < linecount; r++)
			{
				if(sample_output[r] == trainednum) desired = true;
				else desired = false;
				if (perceptronarray[p]->train(sample_input[r], desired) == false)
				{
					perceptronarray[p]->train(sample_input[r], desired);
					trained = false;
				}
			}
			if (trained == true)
				break;
		}
	}
	cout<<"...training complete."<<endl;
	

	//saving perceptron weights
	ofstream myfile;
	myfile.open("perceptron.txt", ios::out | ios::trunc);
	
	cout<<"Begin writing weight file..."<<endl;
	for (int s = 0; s < choices; s++)
	{
		for (int t = 0; t < perceptronarray[s]->size+1; t++)
		{
			myfile<<perceptronarray[s]->outputweight[t]<<endl;	
		}

		for (int u = 0; u < perceptronarray[s]->size; u++)
		{
			for(int v=0; v < perceptronarray[s]->size; v++)
			{
				myfile<<perceptronarray[s]->hiddenweight[u][v]<<endl;
			}
		
		}
	}
	myfile.close();
	cout<<"...weight file complete."<<endl;

}


//called on "./heart test", which is typed into the command screen
void test()
{

	//READ IN THE TEST FILE - SAME COMMENTS AS ABOVE

	
	ifstream datafile;	
	string line;	
	datafile.open("test.txt");	
	if (!datafile.is_open())
	{
		cout<<"Couldn't open test.txt"<<endl;
		return;
	}
	int testlinecount=0;
	while(getline(datafile,line))
	{
		testlinecount++;
	}

	datafile.close();

	int test_input[testlinecount][SETLENGTH];
	char test_output[testlinecount];
	datafile.open("test.txt");
	for(int i=0; i<testlinecount; i++)
	{
		getline(datafile,line);
		test_output[i]=line[0];
		for (int j=0; j<SETLENGTH; j++)
		{
			test_input[i][j]=line[j+2]=='1'?1:0;
		}
	}
	datafile.close();

	//OPENING THE PERCEPTRON WEIGHT FILE

	ifstream openfile;
	openfile.open("perceptron.txt", ios::in);
	
	Perceptron* testperceptronarray[10];
	for(int g =0; g<10; g++)
		testperceptronarray[g] = new Perceptron(22);
	
	for(int h = 0; h<10; h++)
	{
		for(int k = 0; k<23; k++)
			openfile>>testperceptronarray[h]->outputweight[k];
		for (int i = 0; i<22; i++)
		{
			for(int j=0; j<22; j++)
				openfile>>testperceptronarray[h]->hiddenweight[i][j];
		}
	}

	//READING THROUGH TEST FILE AND APPLYING TESTS TO EACH LINE

	for(int r=0; r<testlinecount; r++)
	{
		
		//creates sample made from line in test file
		char userline[22];
		for(int k=0; k<SETLENGTH; k++)
			userline[k] = test_input[r][k];
		int* s=new int[SETLENGTH];
		for(int i=0; i<SETLENGTH; i++)
	        s[i]=userline[i+2];

	    //runs test on resulting sample - tallies correct predictions
		int* predictionSet=s;
		
		float prediction0 = testperceptronarray[0]->getRawPrediction(predictionSet);
		float prediction1 = testperceptronarray[1]->getRawPrediction(predictionSet);
		
		cout<<"the prediction for 0 is:..."<<prediction0<<endl;
		cout<<"the prediction for 1 is:..."<<prediction1<<endl;
		if(prediction1 > prediction0)
		{
			cout<<"I predict 1"<<endl;
			if(test_output[r] == '1')
				correct++;
		}
		else
		{
			cout<<"I predict 0"<<endl;
			if(test_output[r] == '0')
				correct++;
		}
	
	}


	//CRUNCHING DTA
	float floatlinecount = (float)testlinecount;
	float floatcorrect = (float)correct;
	float percentcorrect = floatcorrect / floatlinecount;
	cout<<percentcorrect<<endl;


	openfile.close();
}

//determines what function is called based on the command line input
void doOperation()
{
	if (operation.compare("train")==0)
		train();
	else if (operation.compare("test")==0)
		test();
}


//program starts here
int main(int argc, char **argv)
{
	//save the command line arguments
	if(argc>=2)
		operation=argv[1];
	if(argc==3)
		symbol=argv[2];
	else
		symbol=" ";
	//if the user didn't give any, print out some help and stop
	if(argc==1)
	{
		cout << "Usage: " << endl;
		cout << " heart train" << endl;
		cout << " heart test" << endl;
		exit(0);
	}

	//seed the random number generator
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	//we don't need to make a window if the user selects "train"
	if(operation.compare("train")==0)
	{
		train();
		exit(0);
	}
	else if(operation.compare("test")==0)
	{
		test();
		exit(0);
	}
}
