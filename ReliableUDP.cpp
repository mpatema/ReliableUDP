/*
	Reliability and Flow Control Example
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Net.h"

//#define SHOW_ACKS

using namespace std;
using namespace net;

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 30.0f;
const float SendRate = 1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 256;

const int FileNameLength = 256;

class FlowControl
{
public:

	FlowControl()
	{
		printf("flow control initialized\n");
		Reset();
	}

	void Reset()
	{
		mode = Bad;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}

	void Update(float deltaTime, float rtt)
	{
		const float RTT_Threshold = 250.0f;

		if (mode == Good)
		{
			if (rtt > RTT_Threshold)
			{
				printf("*** dropping to bad mode ***\n");
				mode = Bad;
				if (good_conditions_time < 10.0f && penalty_time < 60.0f)
				{
					penalty_time *= 2.0f;
					if (penalty_time > 60.0f)
						penalty_time = 60.0f;
					printf("penalty time increased to %.1f\n", penalty_time);
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}

			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;

			if (penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f)
			{
				penalty_time /= 2.0f;
				if (penalty_time < 1.0f)
					penalty_time = 1.0f;
				printf("penalty time reduced to %.1f\n", penalty_time);
				penalty_reduction_accumulator = 0.0f;
			}
		}

		if (mode == Bad)
		{
			if (rtt <= RTT_Threshold)
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;

			if (good_conditions_time > penalty_time)
			{
				printf("*** upgrading to good mode ***\n");
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = Good;
				return;
			}
		}
	}

	float GetSendRate()
	{
		return mode == Good ? 30.0f : 10.0f;
	}

private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};

// ----------------------------------------------

int main(int argc, char* argv[])
{


	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;

	// need to tweak this functionality to accept and parse the new set of parameters and their formats. 
	// add a call to a FileOperations.cpp function that will open a file based upon the first
	// parameter. This task will be done clientside.
	// 
	// Will parse and open desired file before parsing the socket number, IP and port number to open 
	// a connection. 
	// 
	// when metadata has been successfully parsed and opened, relay to client that connection was opened
	// (tweak current feedback line) 
	// e.g. "Connection was opened at [IP], port [port num.], at socket [socket num.]"
	// 
	//

		// command line arguments should be included. users should be able to configure their 
	// connections using the command line arguments, giving essential metadata for 
	// transmission. 
	// 
	// **First argument should include name of desired file to transmit.**
	// allow both relative and explicit filepaths. 
	//
	// Remember to include reasonable defaults, and add user feedback showing their currently 
	// configured parameters.
	// ask for socket, IP, port, respectively. 
	// FORMAT WILL BE AS FOLLOWS:
	// ReliableUDP.exe [file] [IP] [port num.]

	for (int i = 0; i <= argc; i++) {
		//loop through args. skip the first.
		switch (argc) {
		case 1:
			// program name.
			break;
		case 2:
			//filename.
			char* fileName[FileNameLength];
			//check for help switch.
			if (strcmp(argv[2], "help")) {
				printf("ReliableUDP: Usage\n");
				printf("	ReliableUDP [input file] [IP] [port num.]\n");
				printf("	 - Switches should be in order, any invalid arguments will be set to defaults.\n");
			}
			//assign fileName.
#pragma warning (disable: 4996) 
			strcpy(*fileName, argv[2]);
			break;
		case 3:
			//IP.
			int a, b, c, d;
#pragma warning(suppress : 4996)
			if (sscanf(argv[3], "%d.%d.%d.%d", &a, &b, &c, &d))
			{
				mode = Client;
			}
			else {
				//set default.
			}
		case 4:
			//port.
			if (atoi(argv[4]) != 0 ) {

			}
			break;
		}

	}

	// before connection is opened, ensure that file exists and can be opened.
	// also ensure that file is an appropriate format (ASCII/binary).
	// (call to file open function, after checking file extension)
	// 
	// After file has been double-checked for compatibility, split the file 
	// into chunks for transmission. 
	// 
	// use a data structure(?) or another method to split file into reasonably 
	// sized chunks (composed of several packets). 'chunk' size should be a 
	// relatively large amount of packets.
	// (test overhead with smaller/larger packets[?])
	// larger chunks should reduce overhead while keeping error correction 
	// time lower.
	// 
	// remember to send one chunk at a time, one packet at a time. 
	// after a chunk has been fully transmitted, check the chunk and 
	// continue if no other action needs to be taken.
	//

	// initialize sockets.

	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	ReliableConnection connection(ProtocolId, TimeOut);


	const int port = mode == Server ? ServerPort : ClientPort;

	if (!connection.Start(port))
	{
		printf("could not start connection on port %d\n", port);
		return 1;
	}


	if (mode == Client)
		connection.Connect(address);
	else
		connection.Listen();

	//
	// After client side has recieved validation for a compatible file, 
	// confirmation of file metadata will be sent in the form of a packet.
	// The header should include all relevant information regarding the 
	// connection and the file contents.
	// 
	// could we maybe send a packet with a bunch of preliminary information(?)
	// some sort of separate packet (?)
	// 
	// The metadata should probably be retrieved before anything substantial occurs.
	//

	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	FlowControl flowControl;

	while (true)
	{
		// update flow control

		if (connection.IsConnected())
			flowControl.Update(DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (mode == Server && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf("reset flow control\n");
			connected = false;
		}

		if (!connected && connection.IsConnected())
		{
			printf("client connected to server\n");
			connected = true;
		}

		if (!connected && connection.ConnectFailed())
		{
			printf("connection failed\n");
			break;
		}

		// send and receive packets

		//
		// Implement a function to 'iterate' through a chunk of a file and check 
		// if any further action is needed.
		// 
		// chunks should, ideally, be evenly divided (x packets per chunk).
		// function will do as follows:
		//  - receive information regarding current chunk and 'progress' 
		// (packets already transmitted in each chunk)
		//  - increment through each chunk, packet by packet.
		//  - when the end of a chunk is reached (packet x), move onto the next chunk.
		//  - **if an error is found in the current chunk the entire chunk should be retransmitted.**
		//

		sendAccumulator += DeltaTime;

		while (sendAccumulator > 1.0f / sendRate)
		{
			int count = 0;
			unsigned char packet[PacketSize];
			const unsigned char newData[50] = "wahah wee";
			memcpy(packet, newData, 50);
			//memset(packet, 0, sizeof(packet));
			connection.SendPacket(packet, sizeof(packet), count);
			sendAccumulator -= 1.0f / sendRate;
			printf("% s", "packet sent !");
		}


		//
		// add functionality here that will guide the program through the transmission.
		// A call to another function should be used, one that checks on the server side
		// against the filesize and all of the packets accumulated thus far.
		// 
		// care should also be taken to ensure that pieces are recieved in the proper order and 
		// without error. once an entire chunk is recieved, checking should be done to reduce 
		// overhead (before doing a full-file check).
		//
		while (true)
		{
			unsigned char packet[256];
			int bytes_read = connection.ReceivePacket(packet, sizeof(packet), packet);
			if (bytes_read == 0)
				printf("% s", "packet recieved !");
				break;
		}

		// show packets that were acked this frame

		//
		// As transmission begins, a copy of the transmitted file should be created on the server-side file system.
		// (after compatibility is ensured)
		// write each piece to this new file as it is completed. This will allow at least one error-check before 
		// information is written. Keep track of progress in file transmission with a generic progress variable 
		// (e.g. chunks completed) that can be incremented and tracked with a sort of 'progress' function.
		//

#ifdef SHOW_ACKS
		unsigned int* acks = NULL;
		int ack_count = 0;
		connection.GetReliabilitySystem().GetAcks(&acks, ack_count);
		if (ack_count > 0)
		{
			printf("acks: %d", acks[0]);
			for (int i = 1; i < ack_count; ++i)
				printf(",%d", acks[i]);
			printf("\n");
		}
#endif

		// update connection

		connection.Update(DeltaTime);

		// show connection stats

		statsAccumulator += DeltaTime;

		while (statsAccumulator >= 0.25f && connection.IsConnected())
		{
			float rtt = connection.GetReliabilitySystem().GetRoundTripTime();

			unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();

			float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();

			printf("rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n",
				rtt * 1000.0f, sent_packets, acked_packets, lost_packets,
				sent_packets > 0.0f ? (float)lost_packets / (float)sent_packets * 100.0f : 0.0f,
				sent_bandwidth, acked_bandwidth);

			statsAccumulator -= 0.25f;
		}

		//net::wait(DeltaTime);
	}
	//
	// before shutdown, check back with our progress function and see if the entire file has been transmited 
	// (did we miss any packets)?
	// 
	// SHA-2 verification is considered as a first option in the protocol. utilising the metadata of 
	// the packets and the fully transmitted file (serverside), a file integirty digest will be 
	// generated and checked in comparison to the transmitted file. Users will be notified of a problem 
	// in file transmission.
	//
	ShutdownSockets();

	return 0;
}
