// C Libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <cstring>
// C++ Libraries
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define MAX_LINE 80

//parsing input string into args array
void parser(string& in, char* args[MAX_LINE/2+1]);

int main()
{
	int should_run = 1;
	string keyin;
	vector<string> historylist;
	char *args[MAX_LINE/2+1];
	for(int i=0; i<(MAX_LINE/2);i++) //initialize args into nullptr
		args[i]=nullptr;
	pid_t pid,pidt,pidc, pidp;

// loop for UI in shell level
	while(should_run)
	{

		try
		{
			// latch for the loop, 1: continue, 0: end
			cout << "osh > " ;
			getline(cin, keyin);

			//execute based on command
			if(keyin=="exit")
			{
				should_run=0;
			}
			else if(keyin=="history")
			{
				cout << endl;
				//vector<string>::reverse_iterator rit = historylist.rbegin();
				for(int i=historylist.size()-1; i>=0; i--)
					cout << "!" << i+1 << " " << historylist[i] << endl;
			}
			else if(keyin=="")
			{}
			else
			{
				int vecsize = historylist.size();
				if(keyin=="!!")
				{
					if(vecsize==0)  //error handling when empty vector
					{
						string error = "error: No commands in history.";
						throw error;
					}
					cout << "[parent] execute the latest command = " << historylist[vecsize-1] << endl;
					keyin = historylist[vecsize-1];  //replace input command with previous command
					historylist.push_back( historylist[vecsize-1] );
					if(vecsize==10)  //remove first element if overflow
						historylist.erase(historylist.begin());
				}
				else if( (keyin[0])=='!')
				{
					string numstr = keyin.substr(keyin.find("!")+1,2);
					int n = stol(numstr)-1;
					if( (n+1) > vecsize)  //error handling when no such command
					{
						string error = "error: No such commands in history.";
						throw error;
					}
					cout << "[parent] execute command #" << n+1 <<  " =>  " << historylist[n] << endl;
					keyin = historylist[n];  //replace input command with demanded history
					historylist.push_back( historylist[n] );
					if(vecsize==10)   //remove first element if overflow
						historylist.erase(historylist.begin());
				}
				else
				{
					if(vecsize==10)
						historylist.erase(historylist.begin());
					historylist.push_back(keyin);
				}

				//parsing input string into args array
				parser(keyin, args);

				// create parent and child processes
				pid = fork();

				// parent process
				if(pid > 0)
				{
					pidp=getpid();
					cout << "[parent] ...parent pID: " << pidp << endl;

					// determine to enable waiting feature or not
					if(keyin.find("&") != string::npos)
					{
						  cout << "[parent] ...child executing in parallel"<<endl;
					}
					else
					{
						cout << "[parent] ...waiting for child process to finish" << endl;
						pidt = wait(NULL);
						cout << endl << "[parent] Child process ends at !!" << pidt <<endl;
					}

					// Error handling for zombie children
					waitpid(-1,0,WNOHANG);

					//free the memory in args array
					for(int i=0; i<(MAX_LINE/2); i++)
						if(args[i]!=nullptr && args[i][0]!='\0')
						{
							delete [] args[i];
							args[i] = nullptr;
						}
				}

				// child process
				else if(pid == 0)
				{
					pidc=getpid();
					cout << "[child] Process start at " << pidc << endl << endl;


					// determine command is valid or not
					if (execvp(args[0], args) < 0)
						cout<< "[child] Failure of execvp() !!!" <<endl;

					exit(0);
				}

				// create child process failure
				else if(pid <0)
				{
					printf("[parent] Fail to create child process via fork() !! \n");
				}
			}
		}
		catch(string& err)
		{
			cout << "[parent] " << err << endl;
		}
		cout << endl;
	}

	cout << "[parent] end of program" << endl;
 return 0;
}

void parser(string& in, char * args[MAX_LINE/2+1])
{
	string temp="";
	int index=0;


	size_t found = in.find(" ");
	if(found==string::npos)  //case of no space existed
	{
		string para = in;
		args[0] = new char[para.size()+1];
		strcpy(args[0], para.c_str());
	}
	else  //case of space existed (with parameter)
	{
		for(int i=0; i<=in.size(); i++) //examine each character
		{
			if( (in[i])==' ' || i==in.size())  //found a space, append the substring into args[]
			{
				string para = temp;
				args[index] = new char[para.size()+1];
				strcpy(args[index], para.c_str());
				temp = "";
				index++;
			}
			else if(in[i]=='&')
			{
				i++;
			}
			else
			{
				temp += in[i];
			}
		}
	}
	//cout << args[0][0] <<args[0][1] << endl;
	//cout << (args[0]==nullptr) << endl;
	//cout << (args[1]==nullptr) << endl;
	//cout << (args[2]==nullptr) << endl;
}
//ps -ax | grep Skype
