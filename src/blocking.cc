/* $Id: blocking.cc,v 1.4 1999/12/14 10:36:15 code Exp $ */

#include "blocking.h"
#include <iostream>
#include <assert.h>

//
// Class Monitor
//

Monitor::Monitor() {
	pthread_mutex_init(&class_lock, NULL);
};

Monitor::~Monitor() {
	pthread_mutex_unlock(&class_lock);
	pthread_mutex_destroy(&class_lock);
};
	
void Monitor::LockMe() {
	pthread_mutex_lock(&class_lock);
};

void Monitor::UnlockMe() {
	pthread_mutex_unlock(&class_lock);
};

pthread_mutex_t* Monitor::GetClassLock() {
	return &class_lock;
};

//
// Class Room
//

Room::Room() {
	readers_in = 0;
	readers_waiting = 0;
	writers_in = 0;
	writers_waiting = 0;
	pthread_mutex_init(&global_lock,NULL);
	pthread_cond_init(&readers, NULL);
	pthread_cond_init(&writers, NULL);
};

Room::~Room() {
	pthread_mutex_unlock(&global_lock);
	pthread_mutex_destroy(&global_lock);
	pthread_cond_destroy(&readers);
	pthread_cond_destroy(&writers);
};

bool Room::IsReader() {
	bool b;
	pthread_mutex_lock(&global_lock);
	b = readers_in > 0;
	pthread_mutex_unlock(&global_lock);
	return b;
}

bool Room::IsWriter() {
	bool b;
	pthread_mutex_lock(&global_lock);
	b = writers_in > 0;
	pthread_mutex_unlock(&global_lock);
	return b;
}

/*
void Room::EnterReader() {
	cout << "E Reader" << endl;
	pthread_mutex_lock(&global_lock);
	cout << "E Reader(endlock)" << endl;
	if( writers_in + writers_waiting > 0) {
		readers_waiting++;
		pthread_cond_wait(&readers, &global_lock);
	} else {
		readers_in++;
		pthread_mutex_unlock(&global_lock);
	}
};

void Room::EnterWriter() {
	cout << "E Writer" << endl;
	pthread_mutex_lock(&global_lock);
	cout << "E Writer(endlock)" << endl;
	if( writers_in + readers_in > 0) {
		writers_waiting++;
		pthread_cond_wait(&writers, &global_lock);
	} else {
		writers_in++;
		pthread_mutex_unlock(&global_lock);
	};
};


void Room::LeaveReader() {
	cout << "L Reader" << endl;
	pthread_mutex_lock(&global_lock);
	cout << "L Reader(endlock)" << endl;
	readers_in--;
	if( readers_in == 0 && writers_waiting > 0) {
		writers_waiting--;
		writers_in++;
		pthread_cond_signal(&writers);
		pthread_mutex_unlock(&global_lock);
	} else
		pthread_mutex_unlock(&global_lock);
};

void Room::LeaveWriter() {
	cout << "L Writer" << readers_in << " " << writers_in << endl;
	pthread_mutex_lock(&global_lock);
	cout << "L Writer(endlock)" << endl;
	if( readers_waiting > 0 ) {
		readers_in = readers_waiting;
		readers_waiting = 0;
		writers_in--;
		pthread_cond_broadcast(&readers);
	} else
		if( writers_waiting > 0 ) {
			writers_waiting--;
			pthread_cond_signal(&writers);
		} else {
			writers_in--;
		}
	pthread_mutex_unlock(&global_lock);
};
*/

void Room::EnterReader() {
	pthread_mutex_lock(&global_lock);
	readers_waiting++;
	while ( writers_waiting > 0 || writers_in > 0) {
		pthread_cond_wait(&readers, &global_lock);
	};
	readers_waiting--;
	readers_in++;
	pthread_mutex_unlock(&global_lock);
	assert(!(writers_in > 1));
};

void Room::EnterWriter() {
	pthread_mutex_lock(&global_lock);
	writers_waiting++;
	while ( writers_in > 0 || readers_in > 0) {
		pthread_cond_wait(&readers, &global_lock);
	};
	writers_waiting--;
	writers_in++;
	pthread_mutex_unlock(&global_lock);
	assert(!(writers_in > 1));
};


void Room::LeaveReader() {
	pthread_mutex_lock(&global_lock);
	readers_in--;

	pthread_cond_signal(&readers);

	pthread_mutex_unlock(&global_lock);
	assert(!(writers_in > 1));
};

void Room::LeaveWriter() {
	pthread_mutex_lock(&global_lock);
	writers_in--;

	pthread_cond_signal(&readers);

	pthread_mutex_unlock(&global_lock);
	assert(!(writers_in > 1));
};
