#include<bits/stdc++.h>
#include<unistd.h>
#include <chrono>
#include<sys/time.h>
#include<thread>
#define max_cores 8
//#define Hz 1000000000 
#define Hz 10000
using namespace std;
using namespace std::chrono;
const int run_program=5;	//seconds
const int rate=100;
mutex m;
mutex cout_mtx;
queue<int>Queue_1,Queue_2,Queue_3;
static vector<int> E(3);
vector<int> energy;
int random (int a,int b)
{
	return rand()%(b-a);
}
struct QoS{
	int QCI_value;
	int priority_level;
	int packet_delay_budget;	//in milliseconds
	double pkt_error_rate;
};
struct clock{
	int hh;
	int mm;
	int ss;
	int ms;
};
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
	{70,55,200,1e-6},
	{70,55,200,1e-6},
	{79,65,50,1e-2},
	{80,68,10,1e-6},
	{82,19,10,1e-4},
	{83,22,10,1e-4},
	{84,24,30,1e-5},
	{85,21,5,1e-5}
};
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
double get_time(struct clock s){
	double value=3600*(s.hh)+60*(s.mm)+s.ss+s.ms;
	return value*1e3;		// to get in milliseconds
}
class Packet{
public:
	int id;
	int pkt_size;
	int QCI_idx;
	int QCI_idx_pos;
	int priority_level;
	bool pkt_sucess;
	struct clock start_time;
	struct clock end_time;
	double time_req;
	static int obj_count;

	static int get_count()
	{
		return obj_count;
	}

	Packet(){
		obj_count++;
		pkt_size=1400;
		pkt_sucess=false;
		id=obj_count;
	}
		void print()
	{
		cout_mtx.lock();
		cout<<"\nid   pkt_size   QCI_idx\n";
		cout<<id<<" "<<pkt_size<<" "<<QCI_idx<<" "<<start_time.hh<<":"<<start_time.mm<<":"<<start_time.ss<<":"<<start_time.ms<<" "<<end_time.hh<<":"<<end_time.mm<<":"<<end_time.ss<<":"<<end_time.ms<<" "<<time_req<<" "<<pkt_sucess<<"\n";
		cout_mtx.unlock();
	}
	// void print()
	// {
	// 	cout<<"\nid   pkt_size   QCI_idx\n";
	// 	cout<<id<<" "<<pkt_size<<" "<<QCI_idx<<" "<<start_time.hh<<":"<<start_time.mm<<":"<<start_time.ss<<":"<<start_time.ms<<" "<<time_req<<"\n";
	// }
		void set_end_time();
};

static vector<Packet> pkts;
void Packet::set_end_time(){
	end_time=set_time();
}
int Packet::obj_count=0;
class Traffic_Generator{
	public:
		int pkt_rate;
		int delay;
		// static vector<Packet> pkts;
		void random_pkt(){
			double per=(1.0/pkt_rate)*1e6-100,rest,extra;
			// cout<<"Current Time :: ";
   //  		current_time_point(chrono::system_clock::now());
			auto start = high_resolution_clock::now();
    		int cnt=0;
    		while(cnt<pkt_rate*run_program)
    		{
    			if(cnt%pkt_rate==0)
    				cout<<cnt<<endl;
				auto start = high_resolution_clock::now();

				m.lock();
				set_pkt_info();
				m.unlock();

				cnt++;
		 		auto stop = high_resolution_clock::now();
		 		auto duration = duration_cast<nanoseconds>(stop - start);
		 		rest=per-(duration.count()/1e3);
		 		// extra=per-(rest+(duration.count()/1e3));
		 		// rest+=extra;
		 		if(rest>=0)
		 			usleep(rest);
    		}
		 	auto stop = high_resolution_clock::now();
		 	auto duration = duration_cast<nanoseconds>(stop - start);
		 	
		 	cout_mtx.lock();
		 	cout<<double(duration.count()/1e9)<<"\n";
			cout_mtx.unlock();
			//delay(1)
		}
		void specific_pkt(){

		}
		void set_pkt_rate(int x){
			pkt_rate=x;
		}
		void set_pkt_info(){
			int x=random(0,QCI.size());
			Packet p;
			p.QCI_idx=QCI[x].QCI_value;
			p.QCI_idx_pos=x;
			p.time_req=QCI[x].packet_delay_budget;
			p.priority_level=QCI[x].priority_level;
    		//p.print();
			// fill_time(p);
    		// time_t mytime = time(NULL);
    		// char * time_str = ctime(&mytime);
    		// time_str[strlen(time_str)-1] = '\0';
    		p.start_time=set_time();
    		
    		pkts.push_back(p);
    		send_data_to_queue_1(p.id);
    		//(pkts.back()).print();
    		//cout<<p.start_time<<"\n";
    			// printf("Current Time : %s\n", time_str);
			//p.print();
		}
		void send_data_to_queue_1(int id)
		{
			
    		Queue_1.push(id);

		}
		void set_end_time(int id){
    		pkts[id].end_time=set_time();
		}
		bool pkt_reached(int id){
			double end=get_time(pkts[id].end_time);
			double start=get_time(pkts[id].start_time);
			return end-start<=pkts[id].time_req;
		}

};
class Cores
{
public:
	// static vector<int> E(3);
	queue<int> curr_queue;
	vector<int> Core_E;
	int freq;
	bool change_freq; // 0 no chng 1 low , 2 med, 3 high
	bool measure_power;
	int type;	//0 low , 1 med , 2 high
	double curr_time;
	double prev_time;

	const vector<pair<int,int>> type_range{{0,(1*Hz+1)},{1*Hz,(2*Hz+1)},{2*Hz,(3*Hz+1)}};
	Cores(){
		freq=3*Hz;
		change_freq=false;
		measure_power=false;
		type=2;
		for(int i=0;i<3;i++)
			Core_E.push_back(0);
	}
	void change_rate_of_freq(int x)
	{
		freq=x*Hz;
		for(int i=0;i<type_range.size();i++)
		{
			if(freq>=type_range[i].first&&freq<type_range[i].second)
			{
				type=i;
				break;
			}
		}
		//cout<<type<<"??";
		change_freq=false;
	}
	int get_packet()
	{
		int id=curr_queue.front();
		curr_queue.pop();
		return id;
	}
	void get_power()
	{
		int sum=0;
		for(int i=0;i<3;i++)
		{
			E[i]+=Core_E[i];
			Core_E[i]=0;
		}
		display_energy();
	}
	void send_data_to_queue_3(int x)
	{
		Queue_3.push(x);
	}		int cnt=0,x=0;
	void process()
	{

		while(!curr_queue.empty())
		{
			//cout<<change_freq<<" -->";
			if(change_freq)
			{

				x++;
				//cout<<":::"<<x<<" ";
				if(x%3==1)
					change_rate_of_freq(1);
				else if(x%3==2)
					change_rate_of_freq(2);
				else 
					change_rate_of_freq(3);
				//change_freq=false;
			}
			int id=get_packet();
			pkts[id].set_end_time();
			send_data_to_queue_3(id);
			//cout<<id<<":";
			Core_E[type]+=1;

			for(int i=0;i<3;i++)
				cout<<Core_E[i]<<" ";

			cnt++;
			//cout<<"cnt:"<<cnt<<"\n";
			if(cnt%3==0)
				measure_power=true;	
			if(cnt%10==0)
				change_freq=true;		
			if(measure_power)
			{
				get_power();
				measure_power=false;
			}


			// else 
			// 	change_freq=3;

			//usleep(rest);
//**set constant rate for sleep
		}
	}
	void display_energy(){
		cout_mtx.lock();
		for(int i=0;i<3;i++)
			cout<<E[i]<<"..";
		cout<<"\n";
		cout_mtx.unlock();
	}
	
};
vector<thread> t;
vector<Cores> core;
class VUPE_Block{
public:

	bool measure_power;
	bool change_config;
	VUPE_Block(){
		measure_power=false;
		change_config=false;
	}
	int get_packet()
	{
		int id=Queue_1.front();
		Queue_1.pop();
		return id;
	}

	bool find(int x,int id){
		for(int i=0;i<core.size();i++){
			if(core[i].type==x){
				core[i].curr_queue.push(id);
				return true;
			}
		}
		return false;
	}
	void assign_core(int id){
		double req=pkts[id].time_req;
		bool cond=false;
		if(req<100){	//high
			cond=find(2,id);
			if(!cond){
				cond=find(1,id);
				if(!cond)
					cond=find(0,id);
			}
		} 
		else if(req<250){	//med
			cond=find(1,id);
			if(!cond){
				cond=find(2,id);
				if(!cond)
					cond=find(0,id);
			}
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
	// void send_info_to_db(int id){

	// }
	void process_packet(int id){
		if(change_config){
			//change_core_config()
			change_config=false;
		}
		assign_core(id);
		//send_info_to_db(id);
	}
	void energy_module()
	{
		int sum=0;
		for(int i=0;i<core.size();i++)
			core[i].get_power();
		for(int i=0;i<3;i++)
			sum+=(i+1)*E[i];
		energy.push_back(sum);
	}
	void send_data_to_queue_2(int id)
	{
		Queue_2.push(id);
	}
	void run()
	{
		double per=(1.0/rate)*1e6-100,rest;
		auto start = high_resolution_clock::now();
		//create_cores();
		while(!Queue_1.empty())
		{
			auto start = high_resolution_clock::now();
			m.lock();
			int id=get_packet();
			pkts[id].set_end_time();
			process_packet(id);
			send_data_to_queue_2(id);
			if(measure_power){
				energy_module();
				measure_power=false;
			}
			m.unlock();

			auto stop = high_resolution_clock::now();
		 	auto duration = duration_cast<nanoseconds>(stop - start);
			rest=per-(duration.count()/1e3);
	 		if(rest>=0)
	 			usleep(rest);	
		}
		auto stop = high_resolution_clock::now();
	 	auto duration = duration_cast<nanoseconds>(stop - start);
	 	cout_mtx.lock();
	 	cout<<double(duration.count()/1e9)<<"\n";
	 	cout_mtx.unlock();
	}
};
class Core_And_Traffic_Statistics{
public:
	vector<int> throughput;
	vector<double> pkt_loss;

	void plot_graph(int idx){
    	ofstream data("data/plot.txt", ios::app); // open filename.txt in append mode
    	data<<idx<< endl; // append "some stuff" to the end of the file
    	data.close();			
	}
	void plot(){
		int ans,x;
		bool condition=true;
		cout<<"\n1. Energy vs Time";
		cout<<"\n2. Throughput vs Time";
		cout<<"\n3. Energy vs Throughput";
		cout<<"\n4. Throughput vs Packet Loss";
		while(condition){
			cout<<"\nEnter the plot no.";
			cin>>x;
			if(x>0&&x<=4){
				plot_graph(x);
			}
			else
				cout<<"\nInvalid choice";
			cout<<"\nDo you want to continue plot Y/N";
			cin>>ans;
			if(ans=='N')
				condition=false;
		}

	}

};
class VUPB_Block{
public:
	int freq;
	bool predict_data;
	VUPB_Block(){
		freq=3*Hz;
		predict_data=false;
	}
	void learn_module(){

	}
	// void set_data_to_dataset(int id){
 //    	ofstream data("data/dataset.txt", ios::app); // open filename.txt in append mode
 //    	data<<id<< endl; // append "some stuff" to the end of the file
 //    	data.close();

	// }
	void set_data_to_dataset(int id)
	{
    	// file pointer
    	cout_mtx.lock();
    	fstream datafile;
    	// opens an existing csv file or creates a new file.
    	datafile.open("dataset.csv", ios::out | ios::app);
  		//ID Pkt_size QCI_value priority_level start_time(Day MM DD HH:MM:SS YYYY) time_req(ms)
 		datafile<<pkts[id].id<<", "<<pkts[id].pkt_size<<", "<<pkts[id].QCI_idx<<", "<<pkts[id].priority_level<<", "<<pkts[id].start_time.hh<<":"<<pkts[id].start_time.mm<<":"<<pkts[id].start_time.ss<<":"<<pkts[id].start_time.ms<<", "<<pkts[id].end_time.hh<<":"<<pkts[id].end_time.mm<<":"<<pkts[id].end_time.ss<<":"<<pkts[id].end_time.ms<<", "<<pkts[id].time_req<<"\n";
    	datafile.close();
    	cout_mtx.unlock();
	}
	void send_data_to_db(int x){

	}
	void change_config(int freq){

	}
	void send_freq_to_db(int x){

	}
	int  predict_freq(){
		return 0;
	}
	void run(){
		while(!Queue_2.empty()){
			learn_module();
			int id=Queue_2.front();
			cout<<id;
			Queue_2.pop();
			set_data_to_dataset(id);
			if(predict_data){
				freq=predict_freq();
				change_config(freq);
			}
			else{
				send_freq_to_db(freq);

			}

		}
	}
};
void thread_1(int id){
	static Traffic_Generator traffic_controller;
	traffic_controller.pkt_rate=100;

	traffic_controller.random_pkt();
	//sleep(5);
}

void make(int id){
	Cores c;
	core.push_back(c);
	auto start = high_resolution_clock::now();
	while(1){

		while(!core[id].curr_queue.empty()){

				core[id].process();
		}
				auto stop = high_resolution_clock::now();
	 	auto duration = duration_cast<nanoseconds>(stop - start);
	 	if(duration.count()/1e9>=run_program+5)
	 		break;
	}
}
void thread_2(int id){
	auto start = high_resolution_clock::now();
	static VUPE_Block engine;
	while(1)
	{
		if(!Queue_1.empty())
			engine.run();

		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(stop - start);
	 	if(duration.count()/1e9>=run_program+5){
	 		engine.energy_module();
	 		break;
	 	}
	}
}
void thread_3(int id){
	static VUPB_Block VUPB;
	auto start=high_resolution_clock::now();
	while(1){
		VUPB.run();
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(stop - start);
	 	if(duration.count()/1e9>=run_program+5){
	 		break;
	 	}
	}

}
void thread_4(int id){
	static Core_And_Traffic_Statistics Stats;
	auto start = high_resolution_clock::now();


	while(1)
	{
		this_thread::sleep_for(chrono::milliseconds(5000) );// 5 seconds;
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<nanoseconds>(stop - start);
	 	if(duration.count()/1e9>=run_program+5){
	 		break;
	 	}
	}
}
//**working properly

//To run a thread evey k minutes

// #include <chrono> 
// #include <thread> 
// // .... 
// while (some_condition) { 
//   // Do whatever you're doing 
//   std::this_thread::sleep_for(std::chrono::minutes(5)); 
// }
int main()
{
	thread t4(thread_4,4);

	sleep(1);
	for(int i=0;i<max_cores;i++){
		t.push_back(thread(make,i));
	}
	sleep(1);	
	thread t2(thread_2,2);
	sleep(1);
	thread t3(thread_3,3);
	sleep(1);
	thread t1(thread_1,1);	

	t1.join();

	for(int i=0;i<max_cores;i++){
		t[i].join();
	}	
	t2.join();
	t4.join();
	t3.join();
	// for(int i=0;i<10;i++)
	// 	pkts[i].print();
	// Traffic_Generator traffic_controller;
	// traffic_controller.pkt_rate=rate;
	// traffic_controller.random_pkt();
	// cout<<"\n";
	// cout<<"\n";
	// pkts[2].end_time=set_time();
	// pkts[2].print();
}

