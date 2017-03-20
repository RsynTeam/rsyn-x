/*
 * Timer.h
 *
 *  Created on: 2011/3/11
 *      Author: kason
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>



class Timer {
public:
	Timer();
	virtual ~Timer();
	void reset();
	void start();
	void stop();
	//void conti();
	void show();
	float getTime();
private:
	double start_time, sum_time;
	int state;
	struct timeval tv;
	/*
	struct timeval tv;
	struct timezone tz;
	double start_time,now_time,sum_time;
	*/
};

//static FILE* tksim_fp;

#endif /* TIME_H_ */
