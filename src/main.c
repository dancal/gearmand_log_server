#include <iostream>
#include <libgearman/gearman.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <libgearman/gearman.h>
#include <fstream>
#include <limits.h>
#include <time.h>
#include <string>

#include <stdarg.h> 
#include <errno.h>
#include <fcntl.h>
    
#include <map>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#define FILE_MODE   ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH )
    
#define ROOT_LOG "/home/static/wp_log_worker/data"
#define MAX_PATH_LEN 1024
#define MAX_WORK_PROCESS    1000000
#define SERVER_LOCATION     "hk"
#define LOG_FUNCTION_NAME	"rtb_log_write"
    
const char *worker_idx;
const char *worker_name;
const int worker_timeout = 10;
long long jobs_processed = 0;

using namespace std;
ofstream hFileHandle;
char beforeTime[64];

void *gworker_fn_demon(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr) {

    //auto jobptr = gearman_job_workload(job);//this takes the data from the client
    //if (jobptr) std::cout << "job: " << (char*) jobptr << std::endl;

    char datestr[64];
    char filename[1024];

    datestr[0]  = 0;
    filename[0] = 0;

    time_t tms = time(NULL);
    struct tm tt;
    struct tm *ttp = localtime_r(&tms,&tt);
    strftime(datestr,sizeof(datestr),"%Y%m%d%H%M",ttp);
    sprintf(filename,"%s/%s_%s_%s_%s.log", ROOT_LOG, SERVER_LOCATION, worker_name, datestr, worker_idx);

	if ( strcmp(datestr,beforeTime) != 0 ) {
		hFileHandle.flush();
		hFileHandle.close();
		hFileHandle.open(filename, ios::out|ios::app);
	}
	memcpy(beforeTime, datestr, sizeof(datestr));

	
    char *inp_char = static_cast<char *>(std::malloc(gearman_job_workload_size(job) + 1));
    memcpy(inp_char, gearman_job_workload(job), gearman_job_workload_size(job));
    inp_char[gearman_job_workload_size(job)] = '\0';

    if (hFileHandle.fail()) {
        cout << "error" << endl;
    } else {
        hFileHandle << inp_char << endl;
    }

    free(inp_char);

    *ret_ptr = GEARMAN_SUCCESS;
    *result_size = 0;

    //cout << jobs_processed << endl;
    return 0;

}

int main( int argc, char** argv ) {

    if ( argc < 4 ) {
        printf("use : cmd {worker_name} {gearmand host} {gearmand_port}\n");
        exit(1);
    }

    worker_name   = argv[1];
    worker_idx    = argv[2];
    char *ghost   = argv[3];
    int gport     = atoi(argv[4]);

    auto status_print = [](gearman_return_t gserver_code){
        cout<<gserver_code<< " --  ";
        if (gearman_success(gserver_code)) cout<<"success";
        if (gearman_failed(gserver_code)) cout<<"failed";
        if (gearman_continue(gserver_code)) cout<<"continue";
        cout<<endl;
    };

    gearman_worker_st* gworker = gearman_worker_create(NULL);

    gearman_return_t gs_code = gearman_worker_add_server(gworker, ghost, gport);
    status_print(gs_code);

    void * job_context = NULL;
    gs_code = gearman_worker_add_function( gworker, worker_name, worker_timeout, gworker_fn_demon, job_context);
    //status_print(gs_code);
    while ( 1 ) {
        gs_code = gearman_worker_work(gworker);

        if ( gs_code == GEARMAN_SUCCESS ) {
            // ok
        } else {
            return 0;
        }

        jobs_processed++;
        if ( jobs_processed > MAX_WORK_PROCESS ) {
            jobs_processed = 0;
            return 0;
        }

    }
	hFileHandle.flush();
	hFileHandle.close();
    gearman_worker_free(gworker);

    cout<<"done"<<endl;
    return 0;
}

