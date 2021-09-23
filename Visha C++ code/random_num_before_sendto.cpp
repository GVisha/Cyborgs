#include <iostream>

using namespace std;

int rand_number(){
	
	int max = 1;
	int min = 0;	//use when implementing at-least-once or at-most-once invocation
//	int min = 1;   // use when not implementing any invocation
	int randNum = std::rand() % (max - min + 1) + min;
	
	return randNum;	
} //have been put into the code

int main(int argc, char *argv[]) {
	
	srand( (unsigned)time(NULL) ); // i have put it outside the while loop, it should be working
	
//	for (int i; i<5; i++)
	while (true)
	{	

		// Ganesan, I did not put the part below into your code yet
		// START: add by Yuxin without testing//
		
		int randNum = rand_number();
		if (randNum == 1)
		{
			int sendOk = sendto(in, ch, lengthOfBytes, 0, (sockaddr*)&client, clientLength);
//			cout << randNum << endl;
		}
		// END: add by Yuxin without testing//
	}
}