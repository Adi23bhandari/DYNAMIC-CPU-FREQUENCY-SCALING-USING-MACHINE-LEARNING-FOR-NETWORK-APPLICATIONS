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