#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main() {
	//SPRINT #1 code
	//Airplane Management and Warranty System Client
	// This client will read user input and send it to the server
	string input;
	cout << "Welcome to the Airplane Management and Warranty System Client!" << endl;
	cout << "Please enter a command (type 'exit' to quit): ";
	while (true) {
		getline(cin, input);
		if (input == "exit") {
			break;
		}
		// Here you would normally send the input to the server and receive a response
		// For this example, we will just print the input back to the console
		cout << "You entered: " << input << endl;
		cout << "Please enter a command (type 'exit' to quit): ";
	}

	return 0;
}