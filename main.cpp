#include<bits/stdc++.h>
#include<unistd.h>
#include <chrono>
#include<sys/time.h>
#include<thread>
#include <boost/algorithm/string.hpp>
#define PY_SSIZE_T_CLEAN
#include<Python.h>
// #include </usr/include/python3.8/Python.h>
//#include "numpy/arrayobject.h"
// #include "matplotlibcpp.h"
#include<cmath>


#define max_cores 16
#define GHz (long long int )1000000000 
using namespace std;
using namespace std::chrono;
// namespace plt = matplotlibcpp;

const long long int run_program=180;	//runtime for each rate index  in seconds
const long long int iterations=3;	// each iteration symbolising 3 minutes of data so,(3*480)/60=24 hour
const long long int total_runtime=run_program*iterations;		// total time period of execution of program 
high_resolution_clock::time_point simulation_timer_start;		//	used for making calls at completion of every run_program like power,throughput,pkt_loss 
int rate_idx=0;		//tells about the index of rate 			

/*
	Used to maintain variable traffic flow in the network 
	Consist of 480 entries each symbolising no.of active UEs in every 3 minutes
	Reference taken from data provided by mentor
*/
const vector<double> rate{
 2.60, 2.60, 2.36, 2.36, 2.36, 2.36, 2.36, 2.36, 2.36, 2.36, 2.36, 2.36, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37
, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.37, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38
, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.38, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39
, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.39, 2.40, 2.40, 2.40, 2.40, 2.40, 0.98, 0.98
, 0.98, 0.98, 0.98, 2.40, 2.40, 2.40, 2.40, 2.40, 2.40, 2.40, 2.40, 2.40, 2.40, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41
, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.41, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42
, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.42, 2.43, 3.55, 3.84, 10.46, 11.29, 23.94, 32.32, 33.16, 37.42, 38.35
, 42.62, 48.06, 48.06, 53.50, 55.05, 58.70, 61.53, 61.54, 62.72, 63.90, 65.56, 73.59, 73.59, 80.21, 81.74, 83.05, 88.25, 88.25, 96.29, 97.38
, 97.95, 96.29, 96.29, 91.09, 87.98, 87.08, 85.66, 85.66, 83.06, 80.52, 80.22, 81.64, 81.64, 83.06, 83.06, 85.66, 85.66, 87.08, 87.08, 85.66
, 83.06, 83.06, 81.65, 81.65, 78.81, 77.63, 76.21, 76.21, 74.79, 73.61, 73.61, 73.61, 76.22, 76.22, 76.22, 77.63, 77.64, 78.82, 80.24, 80.24
, 76.22, 76.22, 70.78, 68.18, 68.18, 66.77, 66.77, 64.17, 61.57, 61.57, 61.71, 62.75, 68.19, 68.19, 72.21, 72.82, 74.81, 76.23, 76.23, 72.21
, 72.82, 74.81, 76.23, 76.23, 72.21, 70.28, 66.77, 65.59, 65.59, 66.78, 65.54, 64.18, 61.58, 61.58, 64.18, 66.56, 68.20, 70.80, 70.80, 73.64
, 75.49, 76.24, 78.84, 78.84, 80.26, 81.44, 81.68, 83.10, 83.36, 85.70, 88.17, 88.30, 87.12, 86.80, 85.70, 83.10, 83.10, 77.66, 77.66, 77.66
, 77.67, 77.67, 78.85, 79.50, 80.27, 80.27, 80.27, 78.85, 78.17, 77.67, 76.25, 76.25, 74.83, 74.01, 73.65, 73.65, 73.65, 73.65, 72.50, 72.24
, 70.82, 70.82, 70.82, 69.71, 69.64, 70.82, 70.82, 70.82, 69.64, 69.64, 68.22, 68.22, 69.64, 69.64, 69.64, 70.83, 70.83, 71.66, 73.67, 78.87
, 78.87, 87.14, 88.80, 91.16, 92.34, 92.34, 93.76, 91.63, 89.75, 83.13, 83.13, 81.71, 83.40, 84.31, 89.75, 89.75, 92.35, 89.26, 88.33, 83.13
, 83.18, 84.32, 84.13, 83.14, 81.72, 81.72, 78.86, 77.43, 73.68, 62.81, 62.81, 61.63, 62.10, 62.81, 60.21, 60.21, 57.61, 54.07, 50.76, 40.12
, 40.12, 32.08, 27.73, 25.23, 22.63, 22.63, 21.21, 20.32, 20.03, 18.61, 18.61, 18.61, 17.37, 17.19, 17.19, 17.02, 16.01, 16.01, 16.01, 16.01
, 16.01, 13.18, 10.58, 10.58, 10.58, 10.58, 10.58, 9.16, 9.16, 7.98, 7.98, 9.16, 10.58, 10.58, 7.98, 9.16, 10.58, 10.58, 7.98, 7.98
, 5.15, 5.15, 5.15, 3.97, 3.97, 3.97, 2.55, 2.55, 2.55, 2.55, 2.55, 2.55, 2.55, 2.55, 2.55, 2.55, 3.26, 3.26, 2.56, 2.56
, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 2.56, 3.98, 3.98, 3.98, 3.98, 3.98, 3.98, 3.98, 3.98, 3.98
, 3.99, 3.99, 3.99, 3.99, 3.99, 3.99, 3.99, 3.99, 3.99, 2.84, 2.57, 2.57, 2.57, 2.57, 3.89, 3.99, 3.99, 3.99, 3.99, 3.99
, 3.99, 5.18, 5.18, 5.18, 5.18, 5.18, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.00, 4.80, 5.19, 5.19
, 5.19, 5.19, 4.25, 4.01, 4.01, 4.01, 4.01, 4.01, 4.01, 2.59, 2.59, 2.59, 2.59, 2.59, 2.59, 2.59, 2.59, 2.83, 2.83, 2.83

};

// mutex used to avoid deadlock and starvation and  to  maintain synchronisation of data values 

mutex cout_mtx;
mutex q1_mtx,q2_mtx,q3_mtx;		// Used for Queue_1,Queue_2,Queue_3 respectively
mutex set_time_mtx;				// To assign start and end time to  given packet

mutex total_processed_pkt_mtx;	// Used to check the no.of packets processed in cores i.e to ensure that each packet gets assigned to one of the cores.

queue<long long int>Queue_1,Queue_2,Queue_3;	// To transfer packet in system via use of packet id 
static vector<long long int> E(3);				
vector<long long int> energy;
vector<long long int > predicted_freq_list;

/* To get a value in range form [a,b)  i.e a<=x<b  */
long long int random (long long int a,long long int b)
{
	return rand()%(b-a);
}

/*	Characterstics of the packet in network depending of QCI values	*/
struct QoS{
	long long int QCI_value;
	long long int priority_level;
	long long int packet_delay_budget;	//in milliseconds
	double pkt_error_rate;
};

/*	Mapping the characterstics with the help of QCI values	*/
const vector<QoS> QCI{
	{1,20,100,1e-2},
	{2,40,150,1e-3},
	{3,30,50,1e-3},
	{4,50,300,1e-6},
	{65,7,75,1e-2},
	{66,20,100,1e-2},
	{67,15,100,1e-3},
	// {75}
	{71,56,150,1e-6},
	{72,56,300,1e-4},
	{73,56,300,1e-8},
	{74,56,500,1e-8},
	{76,56,500,1e-4},
	{5,10,100,1e-6},
	{6,60,300,1e-6},
	{7,70,100,1e-3},
	{8,80,300,1e-6},
	{9,90,300,1e-6},
	{69,5,60,1e-6},
	//{70,55,200,1e-6},
	{70,55,200,1e-6},
	{79,65,50,1e-2},
	{80,68,10,1e-6},
	{82,19,10,1e-4},
	{83,22,10,1e-4},
	{84,24,30,1e-5},
	{85,21,5,1e-5}
};

/*	Used to measure the time in HH:MM:SS:MS */
struct clock{
	long long int hh;
	long long int mm;
	long long int ss;
	long long int ms;
};

/*	QCI values according 5G	*/
vector<int> qci{1,2,3,4,65,66,67,71,72,73,74,76,5,6,7,8,9,69,70,79,80,82,83,84,85};
int N=qci.size();

/*	Frequency of chances of occurrences with respect to other qci values reference taken from research paper	*/
vector<int> count_qci{200,60,80,20,   1,1,1,1,1,1,1,1     ,16,90,16,16,16,  1,1,1,1,1,1,1,1};

/* Used to calculate the cummulative sum of frequency array i.e count_qci	*/
vector<int>  prefix(N);

map<int,int> map_qci;
map<int,int> rev_map_qci;
map<int,int> loss;
int sr_no=0;

/*	
	Returns the index of ceiling of index r in prefix array using binary search
*/
int findCeil(vector<int>  &arr, int r, int l, int h)
{
    int mid;
    while (l < h)
    {
        mid = l + ((h - l) >> 1); // Same as mid = (l+h)/2
        (r > arr[mid]) ? (l = mid + 1) : (h = mid);
    }
    return (arr[l] >= r) ? l : -1;
}

/* 
	Returns a random number from array (arr) according distribution array
 	defined by frequency array (freq) where n is size of arrays.
 */
int random_generator(vector<int> &arr, vector<int>&freq,vector<int> &prefix,int n)
{

    // Generate a random number with value from 1 to this sum
    int r = (rand() % prefix[n - 1]) + 1;
    // Find index of ceiling of r in prefix array
    int indexc = findCeil(prefix, r, 0, n - 1);
    return arr[indexc];
}


/* Returns the current time in HH:MM:SS:MS format via. struct clock variable start */
struct clock set_time(){
	struct clock start;
	timeval tp;
	gettimeofday(&tp, 0);
	time_t curtime = tp.tv_sec;
	tm *t = localtime(&curtime);

	start.hh=t->tm_hour;
	start.mm=t->tm_min;
	start.ss=t->tm_sec;
	start.ms=tp.tv_usec/1000;

	return start;
}

/*	Used to print the time in HH:MM:SS:MS format */
void print_time(struct clock s){
	cout<<s.hh<<":"<<s.mm<<":"<<s.ss<<":"<<s.ms<<endl;
}

/* 
	Returns the time in terms of milliseconds which will be
	used to calculate the difference in start and end time
*/
double get_time(struct clock s){
	double value=3600*(s.hh)+60*(s.mm)+s.ss;
	return value*1e3+s.ms;		// to get in milliseconds
}

/* 
	Returns true if time stated is valid
	i.e used to check if set_time() working properly
	as well as to check if each packet is getting time 
	assigned or not
	If not then check for errors  
*/
bool valid_time(struct clock s){
	return ((s.hh>=0&&s.hh<=23)&&(s.mm>=0&&s.mm<=59)&&(s.ss>=0&&s.ss<=59));
}

/*	
	Used to store the data regarding packets in form of csv file
	Each file will have the data of 3 minutes 
	Data is consists of frequency of QCI values followed by throughput
	Each row represent the data for each second  
*/
void send_info_to_dataset(int id){
    sr_no++;
    long long int sum=0;
    // file pointer
   	fstream datafile;
   	// opens an existing csv file or creates a new file.
   	string file="dataset/dataset_";
   	file+="24hr_";
   	file+=to_string(id);
   	file+=".csv";
   	datafile.open(file, ios::out | ios::app);
  	//ID Pkt_size QCI_value priority_level start_time(Day MM DD HH:MM:SS YYYY) time_req(ms)
 	for(auto it=map_qci.begin();it!=map_qci.end();it++){
 		sum+=it->second;
 		datafile<<it->second<<", ";

 	}
   	datafile<<(long long int )(sum)<<"\n";
   	datafile.close();	
}

/*
	Used for reading the data from one csv and writing  data to another csv
	Reading csv file will have data of 3 minutes 
	Writing csv file will summarise the data of 3 minutes of the reading file
	and will add one row regarding the summary i.e each row in writing csv will hold 
	the data of summary of 3 minutes 
*/
class CSV
{
    string file_one;		// Read csv file name
    string file_two;		// Write csv file name
    string delimeter;		// Used to identify the csv data input seperator
    int column_count;		// No. of columns the writing file should have
    vector<int> count;		// Used to summarise the data of one file to another 

    public:
        CSV(string file_one,string file_two,int column_count,string delm = ",") :
        	file_one(file_one),file_two(file_two),delimeter(delm),column_count(column_count),count(column_count)
            { }

        /*
            Function to fetch data from a CSV File
            Parses through csv file line by line and returns the data
            in vector of vector of strings.
        */
        vector<vector<string> > get_data_from_csv(){
            ifstream file(file_one);
            vector<vector<string> > dataList;
            string line = "";
            // Iterate through each line and split the content using delimeter
            while (getline(file, line)){
                vector<string> vec;
                boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
                dataList.push_back(vec);
            }
            file.close();
            return dataList;
        }

        /* 
        	Function which transfers data from one csv file
         	to another csv in form of summary
        */
        void transfer_data(){
        	cout<<file_one<<"\n";
            vector<vector<string> > dataList = get_data_from_csv();           
            int cnt=0;
            // vector<int> count(column_count);          // No.of columns in csv file
            for(vector<string> vec : dataList){
                cnt=0;
                for(string data : vec){
                    count[cnt]+=stoi(data);
                    cnt++;
                }
            }
            string line;
            for(int i=0;i<26;i++){
                line+=std::to_string(count[i]);
                line+=", ";
            }
            write_csv(line);
        }

        /* 
        	Function which writes data of first csv (file_one) file 
        	to the second/another csv file (file_two)
        */
        void write_csv(string line)
        {

            // file pointer
            fstream datafile;
            // opens an existing csv file or creates a new file.
            datafile.open(file_two, ios::out | ios::app);
            datafile<<line<<"\n";
            datafile.close();
        }

};

/*
	Class containing the basic features/characterstics of a packet
*/
class Packet{
public:
	long long int id;				// Used to identify the packet unqiuely
	long long int pkt_size;			
	long long int QCI_idx;			// Tells about the QCI value assigned to the packet
	long long int QCI_idx_pos;		// Used to reverse map the QCI value index in vector<int> qci
	long long int priority_level;	// Tells about the priority of the packet
	bool GBR;						// Tells whether packet has GBR or Non GBR
	bool pkt_sucess;				// Tells whether packet has reached being processed by any core within time frame
	struct clock start_time;		// Tells when packet was created
	struct clock end_time;			// Tells when packet was processed by any of the core
	double time_req;				// Tells us the time window (milliseconds) in which it should be processed 
	
	/*	
		Used to keep count of no.of objects created of 
		class packet and to provide the packet id which later will
		be passing through Queue1 , Queue2 , Queue3
	*/
	static long long int obj_count;	

	/* 
		Returns the count of object of class packet created till current time
	*/
	static long long int get_count()
	{
		return obj_count;
	}

	Packet(){
		obj_count++;
		pkt_size=1400;
		pkt_sucess=false;
		id=obj_count;
		GBR=false;
	}

	/*
		Used to check the packet information and whether there 
		is any issue with any of the input field
	*/
	void print()
	{
		cout_mtx.lock();
		cout<<"\nid   pkt_size   QCI_idx\n";
		cout<<id<<" "<<pkt_size<<" "<<QCI_idx<<" "<<start_time.hh<<":"<<start_time.mm<<":"<<start_time.ss<<":"<<start_time.ms<<" "<<end_time.hh<<":"<<end_time.mm<<":"<<end_time.ss<<":"<<end_time.ms<<" "<<time_req<<" "<<pkt_sucess<<"\n";
		cout_mtx.unlock();
	}

	/*
		Sets the end time for the packet i.e when 
		packet has been processed by any core
	*/
	void set_end_time(){
		set_time_mtx.lock();
		end_time=set_time();
		set_time_mtx.unlock();
	}

	/*
		Return true if the packet was processed 
		before the time required for the packet
	*/
	bool pkt_reached(){
		double end=get_time(end_time);
		double start=get_time(start_time);
		if(end-start<=time_req+10)
			pkt_sucess=true;
		return pkt_sucess;		
	}

};

/* 
	Used as a hash table throughout the program  
	It contains the info regarding packets
	For lookup index of any packet = packet.id-1;
*/
static vector<Packet> pkts;		

long long int Packet::obj_count=0;


/*
	Class responsible for 
		i)making variable traffic rate according to dataset info via vector<double > rate
		ii)sending packet data to Queue1 once packet has been created 
*/
class Traffic_Generator{
	public:
		long long int pkt_rate;		// Tells about how many packet are created per second
		long long int delay;		
		
		/*
			Tells in which of the csv file  packets 
			data should be stored store the data
		*/ 

		long long int id;

		Traffic_Generator(){
			simulation_timer_start=high_resolution_clock::now();		//Used as reference for measuring power
		}

		/*
			Used to set the packet rate 
		*/
		void set_pkt_rate(long long int x){
			pkt_rate=x;
		}

		/*
			Sets the info regarding the object of class packet
		*/
		void set_pkt_info(){
			long long int x=random_generator(qci,count_qci,prefix,N);
			long long int idx=rev_map_qci[x];
			map_qci[x]++;
			Packet p;
			p.QCI_idx=QCI[idx].QCI_value;
			p.QCI_idx_pos=x;
			p.time_req=QCI[idx].packet_delay_budget;
			p.priority_level=QCI[idx].priority_level;
			if(idx<12||idx>22)
				p.GBR=true;
			
			set_time_mtx.lock();
    		p.start_time=set_time();
    		set_time_mtx.unlock();
    		
    		pkts.push_back(p);
    	
		}
		/*
			Used to generate random packet and set packet info via random number generator
			in arbitrary probability distribution fashion i.e using random_generator() function

		*/
		void random_pkt(){

			/* 
				Tells about which rate_idx of the 
				vector<double> rate is been used now
			*/	
			int total_cnt=0;
			
			/*
				Tells about how much time the particular rate_idx should run
				In current scenario time window is 180 seconds
			*/
			int runs;

			/*
				Tells about how much time in microseconds each packet should 
				take to create and put info of packet to maintain the 
				packet rate i.e packet per second (pps) 
			*/
			double per;
			
			/*
				In case some time in microsecond is left i.e(packet time < per time ) even 
				after fill info of packet then it should sleep to maintain 
				the packet rate
			*/
			double rest;

			/*
				Used to remove the extra time in microseconds taken 
				by the rest of the instruction in while block to help 
				maintain the accurate packet rate
			*/
			double extra=120;
			while(total_cnt<iterations)
			{

				cout<<"\n"<<total_cnt<<"\n";

				set_pkt_rate(rate[rate_idx]*50);
				id=rate_idx;
            	if(pkt_rate<2500)
            		extra=200;
            	else
            		extra=100;
				per=((1.0/pkt_rate)*1e6)-extra;
    			runs=0;
				auto start = high_resolution_clock::now();
				auto timer_start=start;

    			while(runs<=run_program)
    			{
    				auto start = high_resolution_clock::now();
    				auto timer_stop=start;
    				auto timer_duration=duration_cast<milliseconds>(timer_stop - timer_start);
    				if(timer_duration.count()>=1000||runs==0){	

    					timer_start=timer_stop;

    					// Here random is used to consider the deviation the frequency i.e vector<int> count_qci
            			
            			prefix[0] = count_qci[0]+random(0,10);		
            			for (int i = 1; i < N; ++i){
                			prefix[i] = prefix[i - 1] + count_qci[i];
                			if(i<21)
                				prefix[i]+=random(0,10);
                			else
                				prefix[i]+=random(0,3);
                		}

            			if(runs)		
            				send_info_to_dataset(id);		
            			runs++;
            			for(int i=0;i<N;i++)
            				map_qci[qci[i]]=0;
            		
    				}

					set_pkt_info();
    				send_data_to_queue_1(pkts.back().id);

		 			auto stop = high_resolution_clock::now();
		 			auto duration = duration_cast<nanoseconds>(stop - start);
		 			rest=per-(duration.count()/1e3);

		 			if(rest>=0){
		 				usleep(rest);
		 			}
    			}
		 		auto stop = high_resolution_clock::now();
		 		auto duration = duration_cast<nanoseconds>(stop - start);

				total_cnt++;
				rate_idx++;
			}

		}
		/*
			Sends the packet id to Queue_1
			Since Queue_1 data is shared between VUPE_Block and Traffic Generator
			mutex are required to avoid situation of packet id loss as both may 
			try to update the Queue_1 at some point of time  
		*/
		void send_data_to_queue_1(long long int id)
		{

			q1_mtx.lock();		
	    	Queue_1.push(id);
    		q1_mtx.unlock();
		}


};
static Traffic_Generator traffic_controller;

/*
	Class responsible processing packets when assigned by the VUPE_Block
*/
class Cores
{
public:
	static long long int core_obj_count;		// To count the no.of object created of class Core till now
	static long long int total_processed_pkt;	// To check if all packets are reaching/assigned to cores i.e any one of the core
	queue<long long int> curr_queue;			// Stores the packet id assigned to the core object
	vector<long long int> Core_E;				// Used to measure the power consumption of the core object
	long long int freq;							// Tells about the frequency on which the core object is running
	bool change_freq; 							// Tells whether frequency change is required or not
	bool measure_power;							// Tells whether power consumption should be measured or not
	long long int type;							// 0->low freq , 1-> med freq , 2-> high freq
	int id;										// Used to identify core object uniquely
	int pkts_processed;							// Tells how many packets processed in this core object
	
	/*
		Tells about how much time in microseconds each packet should 
		take to while processing to maintain present frequency of the core  
	*/
	double per;									
	
	/*
		Used to map the type correctly as it would be useful in measuring the power i.e whether the power
		consumption for the current core is done by low,med,high frequency
	*/
	const vector<pair<long long int,long long int>> type_range{{0,(1*GHz+1)},{1*GHz,(2*GHz+1)},{2*GHz,(3*GHz+1)}};
	
	Cores(){

		change_freq=false;
		measure_power=false;
		type=2;
		freq=3*GHz;
		per=((1.0/freq)*1e6);
		id=core_obj_count;
		core_obj_count++;
		pkts_processed=0;
		for(long long int i=0;i<3;i++)
			Core_E.push_back(0);
	}

	Cores(int type_value){

		change_freq=false;
		measure_power=false;
		type=type_value;
		freq=(type+1)*GHz;
		per=((1.0/freq)*1e6);
		id=core_obj_count;
		core_obj_count++;
		pkts_processed=0;
		for(long long int i=0;i<3;i++)
			Core_E.push_back(0);
	}

	/*
		Used to change the frequency as well as type and per when required 
		i.e when VUPE_Block asks to change_core_config()
	*/
	void change_rate_of_freq(long long int x)
	{
		freq=x*GHz;
		for(long long int i=0;i<type_range.size();i++)
		{
			if(freq>=type_range[i].first&&freq<type_range[i].second)
			{
				type=i;
				break;
			}
		}
		per=((1.0/freq)*1e6);
		change_freq=false;
	}

	/*
		Returns the packet id from the curr_queue of the core object
	*/
	long long int get_packet()
	{
		long long int pkt_id=curr_queue.front();
		curr_queue.pop();
		return pkt_id;
	}

	/*
		Display power consumption in low , med , high frequency till that point of time
	*/
	void display_energy(){
		cout_mtx.lock();
		cout<<"Power consumption till going through core "<<id<<" ";
		for(long long int i=0;i<3;i++)
			cout<<E[i]<<"..";
		cout<<"\n";
		cout_mtx.unlock();
	}

	/*
		Used to measure the power consumption of core object depending upon type
	*/
	void get_power()
	{
		measure_power=true;
		long long int sum=0;
		for(long long int i=0;i<3;i++)
		{
			
			E[i]+=Core_E[i];
			Core_E[i]=0;
		}

		measure_power=false;
		display_energy();
	}

	/*
		Sends packet id to Queue_3 where the throughput and packet loss will be calculated
	*/
	void send_data_to_queue_3(long long int x)
	{
		q3_mtx.lock();
		Queue_3.push(x);
		q3_mtx.unlock();
	}		
	
	/*
		Process the packet if present in current queue 
		Critical Section are shared memory section and this will 
		occur every 3 minutes and will delay the process until respective 
		work is not done i.e for change_freq -> until change_rate_of_frequency is done and
		measure_power -> until power is not measured 
	*/
	void process()
	{
		/*
			In case some time in microsecond is left i.e even 
			after processing  packet then it should sleep to (maintain/align with) 
			the frequency of the core.
		*/
		double rest;

		while(!curr_queue.empty())
		{
    		auto start = high_resolution_clock::now();
    		
			while(change_freq){} // Critical Section 
			
			pkts_processed++;
			total_processed_pkt_mtx.lock();
			total_processed_pkt++;
			total_processed_pkt_mtx.unlock();


			long long int pkt_id=get_packet();
			
			int idx=pkt_id-1;			// Check reference below class packet

			pkts[idx].set_end_time();


			send_data_to_queue_3(pkt_id);
			
			while(measure_power){}		// Critical  section

			Core_E[type]+=1;
			
		

    		auto stop = high_resolution_clock::now();
    		auto duration=duration_cast<milliseconds>(stop - start);
		 	rest=per-(duration.count()/1e3);
		 	if(rest>=0)
		 		usleep(rest);

		}
	}

	
};

/*
	Static data members and functions initialisation
*/
long long int Cores::core_obj_count=0;
long long int Cores::total_processed_pkt=0;

/*
	Used as hash table and to store core object data and useful 
	in calculation of power measure and change core frequency
*/
vector<Cores> core;


/*
	Class responsible for 
	i) accessing packet id from Queue_1
	ii) assigning packets to core
	iii) measuring power consumption
	iv) changing core configuration 
	v) sending packet id to Queue_2
*/
class VUPE_Block{
public:
	// high_resolution clock used for periodically making calls to measure power i.e every 3 minutes
	high_resolution_clock::time_point power_timer_start;
	high_resolution_clock::time_point power_timer_stop;

	bool measure_power;				//Tells whether power consumption should be measured at current point of time
	bool change_config;				//Tells whether core frequency should be changed or not
	vector<int> core_config;		// Tells the initial configuration of core i.e which core is running on which freq;
	vector<pair<int,int> > core_type_range;	// Tells the range of cores that have been running on low,med and high freq
	
	VUPE_Block(){
		measure_power=false;
		power_timer_start=simulation_timer_start;		//sync the power measure run with Traffic Generator
		change_config=false;							
		for(int i=0;i<max_cores;i++){
			if(i<max_cores/4)
				core_config.push_back(0);
			else if(i<(max_cores/2))
				core_config.push_back(1);
			else
				core_config.push_back(2);
		}
		// Indicates that from core_0 to core_((max_core/4)-1) runs on low  frequency initially
		core_type_range.push_back({0,(max_cores)/4});			
		
		// Indicates that from core_(max_core/4) to core_((max_cores/2)-1) runs on med frequency initially
		core_type_range.push_back({(max_cores)/4,max_cores/2,});
		
		// Indicates that from core_(max_core/2) to core_(max_cores-1) runs on high  frequency initially
		core_type_range.push_back({max_cores/2,max_cores});		
	}

	/*
		Takes the packet_id from Queue_1 
	*/
	long long int get_packet()
	{
		q1_mtx.lock();
		long long int id=Queue_1.front();
		Queue_1.pop();
		q1_mtx.unlock();
		return id;
	}

	/*
		Checks whether the suitable core available for the packet
		If yes then assign the packet to core

	*/
	bool find(long long int x,long long int id){
		
		if(core_type_range[x].first!=-1){
			int len=core_type_range[x].second-core_type_range[x].first;
			int core_id=core_type_range[x].first+random(0,len);
			core[core_id].curr_queue.push(id);
			return true;
		}
		return false;
	}

	/*
		Assigns the packet based on priority such that in minimum power 
		the packet can get processed without packet loss
	*/
	void assign_core(long long int id){
		int idx=id-1;
		bool cond=false;
		if(pkts[idx].GBR){			// If GBR true then should be sent to any of the high frequency core
			cond=find(2,id);
		}
		else{
			if(pkts[idx].time_req<200){
				cond=find(1,id);
				if(!cond)
					cond=find(2,id);				
			}
			else{
				cond=find(0,id);
				if(!cond){
					cond=find(1,id);
					if(!cond)
						cond=find(2,id);
				}
			}
		}
		if(!cond){
			cout<<"Error in core_config,not able to assign pkt";
			exit(0);
		}
	}

	// void send_info_to_db(long long int id){

	// }


	/*
		Changes the core configures and make call to the cores object and assign the 
		new freq that has to be set due to predicted frequency
	*/
	void change_core_config(long long int low_freq_core_cnt,long long int med_freq_core_cnt,long long int high_freq_core_cnt){
		change_config=true;
		

		//Changing/Updating  the core_type_range 
		if(low_freq_core_cnt)
			core_type_range[0]={0,low_freq_core_cnt};
		else
			core_type_range[0]={-1,-1};
		if(med_freq_core_cnt)
			core_type_range[1]={low_freq_core_cnt,low_freq_core_cnt+med_freq_core_cnt};
		else
			core_type_range[1]={-1,-1};
		core_type_range[2]={low_freq_core_cnt+med_freq_core_cnt,max_cores};

		//Making changes in core frequency of the core objects
		for(int i=0;i<max_cores;i++){
			core[i].change_freq=true;
			if(i<low_freq_core_cnt){
				core[i].change_rate_of_freq(1);
			}
			else if(i<low_freq_core_cnt+med_freq_core_cnt){
				core[i].change_rate_of_freq(2);
			}
			else{
				core[i].change_rate_of_freq(3);
			}
		}
		change_config=false;	// to come out of the critical section in function VUPE_Block::run()
	}

	/*
		Make call to assign_core to assign packet to any one of the cores
	*/
	void process_packet(long long int id){
		assign_core(id);
		//send_info_to_db(id);
	}

	/*
		Calculates the total energy consumption till the current point of time
	*/
	void energy_module()
	{
		long long int sum=0;
		for(long long int i=0;i<max_cores;i++)
			core[i].get_power();
		for(long long int i=0;i<3;i++)
			sum+=(i+1)*E[i];			// since freqs are 1GHz,2GHz,3GHz 
		energy.push_back(sum);
	}

	/*
		Sends packet id to Queue_2
	*/
	void send_data_to_queue_2(long long int id)
	{
		q2_mtx.lock();
		Queue_2.push(id);
		q2_mtx.unlock();
	}

	/*
		Runs the process of VUPE_Block when Queue_1 is not empty
		Critical Section are shared memory section and this will 
		occur every 3 minutes and will delay the process of function 
		VUPE_Block::run() until respective work is not done i.e
		for change_freq -> until change_rate_of_frequency is done 
	*/
	void run()
	{

		while(!Queue_1.empty())
		{
			while(change_config)	{}		//Critical Section	
			
			long long int id=get_packet();

			process_packet(id);
			send_data_to_queue_2(id);

			auto power_timer_stop=high_resolution_clock::now();
			auto power_duration=duration_cast<milliseconds>(power_timer_stop - power_timer_start);
			
			// In current scenario after 3 minutes the call will be made to measure power
			if(power_duration.count()>=1000*run_program){		
				measure_power=true;
				power_timer_start=power_timer_stop;
			}
			if(measure_power){
				energy_module();
				measure_power=false;
			}	
		}

	}
};
static VUPE_Block engine;


/*
	Class responsible for 
		i) Measuring Throughput and Packet loss
		ii) Plotting Graph at the end of program
*/
class Core_And_Traffic_Statistics{
public:
	static long long int total_throughput;			
	static long long int total_pkt_loss;	
	static vector<long long int> throughput;		// Each position specificing throughput in every 3 minutes times
	static vector<double> pkt_loss;					// Each position specificing packet loss in every 3 minutes times
	long long int pkt_count;						// To count the packets processed to ensure that no packet should be left out
	bool timer_period_completed;					// To check that every call is made after 3 minutes

	// high_resolution clock used for periodically making calls to measure power i.e every 3 minutes	
	high_resolution_clock::time_point timer_start;
	high_resolution_clock::time_point timer_stop;

	Core_And_Traffic_Statistics(){
		timer_start=simulation_timer_start;
		timer_period_completed=false;
		pkt_count=0;
	}

	/*
		Provides the data regarding the throughput and packet loss with time 
	*/
	static void get_data_analysis(){

		cout<<"Throughput\n";
		for(int i=0;i<throughput.size();i++){
			cout<<throughput[i]<<" ";
		}
		cout<<"\t Total Throughput:"<<total_throughput;
		cout<<"\n";
		cout<<"Packet Loss\n";
		for(int i=0;i<pkt_loss.size();i++){
			cout<<pkt_loss[i]<<" ";
		}
		cout<<"\t Total Loss:"<<total_pkt_loss;
		cout<<"\n";
	}
	
	/*
		Takes the packet_id from Queue_1 
	*/
	int get_packet(){

		int pkt_id;
		q3_mtx.lock();
		pkt_id=Queue_3.front();
		Queue_3.pop();
		q3_mtx.unlock();
		return pkt_id;
	}

	/*
		Runs the process of Core_And_Traffic_Statistics which measures
		throughput and packet loss in every 3 minutes
	*/
	void run(){
		while(!Queue_3.empty()){
			int pkt_id=get_packet();

			pkt_count++;

			timer_stop=high_resolution_clock::now();
			auto duration=duration_cast<milliseconds>(timer_stop - timer_start);

			if(duration.count()>=1000*run_program){
				timer_period_completed=true;
				timer_start=timer_stop;

			}
			if(timer_period_completed){
				measure_throughput_and_pkt_loss();
				timer_period_completed=false;
			}
		}
		
	}

	/*
		Calculate the throughput and pkt_loss in that 3 minutes of period
	*/
	void measure_throughput_and_pkt_loss(){

		long long int cnt=0;
		long long int curr_id=pkt_count;	// Tells about the no.of packets reached in Queue_3 from start of program
		throughput.push_back(curr_id-total_throughput);	// Total Throughput is value of throughput till previous 3 minutes 
		
		/*
			Traversing over the packet id to check whether the packet reached before required time
			If not then which QCI value packet has been lost 
		*/
		for(int i=total_throughput;i<curr_id;i++){		

			if(pkts[i].pkt_reached()){
				cnt++;
			}
			else{				
				loss[pkts[i].QCI_idx]++;
			}

		}

		pkt_loss.push_back((curr_id-total_throughput)-cnt);

		/* update total_pkt_loss and total_throughput till current time call */
		total_pkt_loss+=pkt_loss.back();
		total_throughput+=throughput.back();
	}

};

/*
	Static data members and functions initialisation
*/
vector<long long int> Core_And_Traffic_Statistics::throughput;
vector<double> Core_And_Traffic_Statistics::pkt_loss;
long long int Core_And_Traffic_Statistics:: total_throughput=0;
long long int Core_And_Traffic_Statistics:: total_pkt_loss=0;

static Core_And_Traffic_Statistics Stats;

class VUPB_Block{
public:
	long long int  freq;		// To store the value of predicted frequency depending on dataset 
	bool predict_data;	// Used to make call to predict data in every 3 minutes
	long long int rate_idx_pos;	//To synchronise the prediction with respect to traffic_generator time via comparing rate_idx_pos and rate_idx
	long long int prediction_list_idx=0;

	VUPB_Block(){
		freq=3*GHz/2;
		predict_data=false;
		rate_idx_pos=0;
		prediction_list_idx=0;
	}

	/*
		Transfers the file_one  data into file_two in form of summary
		using class CSV functions.
	*/
	void set_data_to_dataset(long long int id)
	{
		string file_one="dataset/dataset_24hr_";
		string file_two="dataset/dataset_24hrs.csv";
        file_one+=std::to_string(id);
        file_one+=".csv";
        CSV csv(file_one,file_two,26);		// object created
        csv.transfer_data();				// transfering the data
	}

	// void send_data_to_db(long long int x){

	// }

	/*
		Depending upon the predicted frequency it changes the configuration of the cores
		depending upon the certain condition on predicted frequency
	*/
	void change_config(long long int  pred_freq){
		//cout<<"Welcome"<<endl;
		/* Used to identify the configuration of core after prediction	*/
		int high_freq_core_cnt,med_freq_core_cnt,low_freq_core_cnt;
		
		/* 
			Core config as follow low% , med% , high% in percentage 
			then using ceil function to get exact count 
		*/
		if(pred_freq<1*GHz+1){

			if(pred_freq<(GHz/2)+1){					// 50%,10%,40%
				low_freq_core_cnt=ceil(0.5*max_cores);			
				med_freq_core_cnt=ceil(0.1*max_cores);
			}
			else{										// 40%,20%,40%
				low_freq_core_cnt=ceil(0.4*max_cores);
				med_freq_core_cnt=ceil(0.2*max_cores);
			}
		}
		else if(pred_freq<2*GHz+1){
			if(pred_freq<1.3*GHz+1){					// 30%,20%,50%
				low_freq_core_cnt=ceil(0.3*max_cores);	
				med_freq_core_cnt=ceil(0.2*max_cores);
			}
			else if(pred_freq<1.7*GHz+1){
				low_freq_core_cnt=ceil(0.25*max_cores);	// 25%,25%,50%
				med_freq_core_cnt=ceil(0.25*max_cores);
			}
			else{										// 20%,30%,50%
				low_freq_core_cnt=ceil(0.2*max_cores);
				med_freq_core_cnt=ceil(0.3*max_cores);
			}
		}
		else if(pred_freq<3*GHz+1){
			if(pred_freq<2.5*GHz+1){					// 20%,20%,60%
				low_freq_core_cnt=ceil(0.2*max_cores);
				med_freq_core_cnt=ceil(0.2*max_cores);	
			}
			else{											// 10%,30%,60%
				low_freq_core_cnt=ceil(0.1*max_cores);
				med_freq_core_cnt=ceil(0.3*max_cores);				
			}

		}
		high_freq_core_cnt=max_cores-low_freq_core_cnt-med_freq_core_cnt;
		/*
			Make call to VUPE_block object to change the core configuration
		*/
		//cout<<low_freq_core_cnt<<","<<med_freq_core_cnt<<","<<high_freq_core_cnt<<endl;
		engine.change_core_config(low_freq_core_cnt,med_freq_core_cnt,high_freq_core_cnt);


	}

	// void send_freq_to_db(long long int x){

	// }
	
	/*
		Predicts the frequency based on data entries in dataset 
		i.e file_two dataset/dataset_24hrs.csv
	*/
	long long int  predict_freq(){
		long long int predicted_freq=predicted_freq_list[prediction_list_idx];
		//cout<<prediction_list_idx<<".";
		prediction_list_idx++;
		return predicted_freq;
	}

	/*
		Used to run process in VUPB_Block 
	*/
	void learn_module(){

		while(!Queue_2.empty()){

			//Shared resource 
			q2_mtx.lock();
			long long int pkt_id=Queue_2.front();
			Queue_2.pop();
			q2_mtx.unlock();

			/*
				Once dataset for the particular rate_idx for 3 minutes 
				is completed then send data from file_one to file_two
				also,make the prediction of frequency
			*/
			if(rate_idx_pos<rate_idx){
				set_data_to_dataset(rate_idx_pos);
				rate_idx_pos++;	
				predict_data=true;		

			}

			/*
				Prediction of frequency along with change of core configuration
			*/
			if(predict_data){
				long long int pred_freq=predict_freq();				// not completed yet
				freq=pred_freq;
				//cout<<pred_freq<<" ";
				change_config(pred_freq);
				predict_data=false;
			}
			else{
				// send_freq_to_db(freq);

			}

		}

	}

};

static VUPB_Block VUPB;


/*
	Thread_1 responsible for generating the traffic 
	via creating packets
*/
void traffic_thread(long long int id){
	traffic_controller.random_pkt();
}

/*
	Core Threads responsible for starting all the cores 
	with each core running on different thread
*/
void core_thread(long long int id){

	auto start = high_resolution_clock::now();
	while(1){
		while(!core[id].curr_queue.empty()){

				core[id].process();
		}
		auto stop = high_resolution_clock::now();
	 	auto duration = duration_cast<milliseconds>(stop - start);
	 	if(duration.count()/1e3>=total_runtime+8)
	 		break;
	}
}

/*
	Thread_2 responsible for running the VUPE object 
*/
void VUPE_thread(long long int id){
	auto start = high_resolution_clock::now();

	while(1)
	{
		if(!Queue_1.empty())
			engine.run();

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
	 	
	 	/*
	 		If in case some energy hasn't been calculated due to some reasons
	 		So make call to energy module to avoid miscalculations
	 	*/
	 	if(duration.count()/1e3>=total_runtime+8){	
	 		engine.energy_module();
	 		break;
	 	}
	}
}

/*
	Thread_3 responsible for running VUPB object
*/
void VUPB_thread(long long int id){

	auto start=high_resolution_clock::now();

	while(1){
		VUPB.learn_module();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
	 	if(duration.count()/1e3>=total_runtime+8){
	 		break;
	 	}
	}
	if(VUPB.rate_idx_pos<rate_idx&&rate_idx==iterations){
		VUPB.set_data_to_dataset(VUPB.rate_idx_pos);
		VUPB.rate_idx_pos++;				
	}


}
/*
	Thread_4 responsible for running Core_And_Traffic_Statistics object
*/
void stats_thread(long long int id){

	auto start = high_resolution_clock::now();

	while(1)
	{
		Stats.run();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
	 	if(duration.count()/1e3>=total_runtime+8){		// additional time for if some data in cores still processed;
	 		
	 		auto duration_remaining=duration_cast<milliseconds>(Stats.timer_stop - Stats.timer_start);
			
				// additional time for if some data in cores still processed;
	 			if(duration_remaining.count()){
	 				Stats.measure_throughput_and_pkt_loss();
	 		}
	 		break;
	 	}
	}
}

/*
	Creates Core objects with initial configuration
*/
void create_cores()
{
	for(int i=0;i<max_cores;i++){
		if(i<max_cores/4){
			Cores c(0);
			core.push_back(c);
		}
		else if(i<((max_cores/2))){
			Cores c(1);
			core.push_back(c);
		}
		else{
			Cores c(2);
			core.push_back(c);
		}

	}

}

vector<thread> t;		// For execution of core threads 

/*

#include <chrono> 
#include <thread> 
// .... 
while (some_condition) { 
  // Do whatever you're doing 
  std::this_thread::sleep_for(std::chrono::minutes(k)); 
}

*/

string file_one="dataset/predict.csv";

void read_file(int id){
    int cnt=0;
	string delimeter=",";
	ifstream file(file_one);
    string line = "";
    // Iterate through each line and split the content using delimeter
    while (getline(file, line)||cnt<iterations){
        auto start = high_resolution_clock::now();
        cnt++;
        vector<string> vec;
        boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
        cout<<vec.size()<<"::";
        if(vec.size())
        {
        	predicted_freq_list.push_back(stoll(vec[0]));
        	cout<<"<"<<stoll(vec[0])<<">";
        }
        else
        	continue;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        sleep(run_program-(duration.count()/1e3));
    }
    file.close();
}
void run_python_code(int argc,char* argv[]){

        wchar_t *program = Py_DecodeLocale(argv[0], NULL);
        if (program == NULL) {
            fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
            exit(1);
        }
        Py_SetProgramName(program);  /* optional but recommended */
        Py_Initialize();
        PyRun_SimpleString("exec(open('write_v3.py').read())") ;
        if (Py_FinalizeEx() < 0) {
            exit(120);
        }
        PyMem_RawFree(program);   
        
}

int main(int argc, char *argv[])
{
	run_python_code(argc,argv);

	sleep(run_program);
	for(int i=0;i<N;i++){
		rev_map_qci[qci[i]]=i;
	}
	
	create_cores();
	sleep(1);
	for(long long int i=0;i<max_cores;i++){
		t.push_back(thread(core_thread,i));
	}

	sleep(1);

	thread t1(traffic_thread,1);

	usleep(10);	//	to avoid creation of other threads before traffic thread;
	
	thread t2(VUPE_thread,2);
	usleep(10);	
	thread t3(VUPB_thread,3);
	usleep(10);
	thread t4(stats_thread,4);
	usleep(10);
	thread read_python_code(read_file,5);
	sleep(1);

	for(long long int i=0;i<max_cores;i++){
		t[i].join();
	}
	//run_python_code.join();	
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	read_python_code.join();

	cout<<"Total Packet created: "<<Packet::obj_count<<"\n";
	cout<<"Total Packet processed: "<<Cores::total_processed_pkt<<"\n";

	Core_And_Traffic_Statistics::get_data_analysis();
	cout<<"\n";
	cout<<"No.of packet processed in each core \n";
	for(int i=0;i<max_cores;i++)
		cout<<core[i].pkts_processed<<" ";
	cout<<endl;

	cout<<"Packet loss in every 3 minutes \n";
	for(auto it=loss.begin();it!=loss.end();it++){
		cout<<it->first<<","<<it->second<<"\t";
	}
	cout<<endl;
	cout<<"Total Energy consumption with time\n";
	for(int i=0;i<energy.size();i++)
		cout<<energy[i]<<" ";
	cout<<endl;
}



/* 
	Run on terminal
	g++ -I/usr/include/python3.8/  main.cpp -L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu  -lpython3.8 -pthread -o output && ./output


*/

