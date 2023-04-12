#include "life.h"
#include <pthread.h>


/** Defines a particular task to handle */
typedef struct {
    size_t from;
    size_t to;
	size_t steps;
	LifeBoard *state;
    LifeBoard *next_state;
    pthread_barrier_t *barr;
} task_description;

void *thread_task(void *args) {
    task_description *task_info = (task_description*) args;
	size_t from = task_info->from;
	size_t to = task_info->from;
	size_t width = task_info->state->width;
	for (int step = 0; step < task_info->steps; step += 1) {

        /* We use the range [1, width - 1) here instead of
            * [0, width) because we fix the edges to be all 0s.
            */
        for (int y = from; y < to; y += 1) {
            for (int x = 1; x < width - 1; x += 1) {
                
                /* For each cell, examine a 3x3 "window" of cells around it,
                    * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                        if (LB_get(task_info->state, x + x_offset, y + y_offset))
                            live_in_window += 1;
                
                /* Cells with 3 live neighbors remain or become live.
                    Live cells with 2 live neighbors remain live. */
                LB_set(task_info->next_state, x, y,
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && LB_get(task_info->state, x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        pthread_barrier_wait(task_info->barr);
		//new state swapped into state here
        pthread_barrier_wait(task_info->barr);
    }
	return NULL;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
	size_t rows = state->height;
	size_t rows_per_thread = rows/threads;
	//size_t leftover_rows = rows%threads;

    // store per-thread information (don't re-use, memory is shared)
	pthread_barrier_t barrier;
    pthread_t id[threads];
    task_description tasks[threads];

	pthread_barrier_init(&barrier, NULL, threads+1);
	LifeBoard *new_state = LB_new(state->width, state->height);

    //size_t step = 0;
    // spawn the threads
	for(int i=0; i<threads; i+=1) {
		tasks[i].state = state;
        tasks[i].next_state = new_state;
		tasks[i].steps = steps;
        tasks[i].barr = &barrier;
	}
	for(int i=0; i<threads; i+=1) {
		if (i == threads - 1) {
            tasks[i].to = rows - 1;
        }
        else {
            tasks[i].to = (i+1)*rows_per_thread;
        }
		if (i == 0) {
            tasks[i].from = 1;
        }
        else { 
            tasks[i].from = i*rows_per_thread;
        }
		pthread_create(&id[i], NULL, thread_task, &tasks[i]);
	}
    // for(int i=0; i<threads; i+=1) {		
    //     tasks[i].from = step;
	// 	step+=rows_per_thread;
	// 	if (threads-i<=leftover_rows){
	// 		step+=1;
	// 	}
    //     tasks[i].to = step;
    //     pthread_create(id+i, NULL, sum_array, tasks+i);
    // }

    // // wait for and combine the results
    // double result = 0;
    // for(int i=0; i<threads; i+=1) {
    //     void *ans;
    //     pthread_join(id[i], &ans);
    //     result += *(double *)ans;
    //     free(ans); // was malloced in just-joined thread
    // }

	//execute the provided number of steps
	for (int i = 0; i < steps; i ++) {
        pthread_barrier_wait(&barrier);
        LB_swap(state, new_state);
        pthread_barrier_wait(&barrier);
   	}

	for (int i = 0; i < threads; i++) {
		pthread_join(id[i], NULL);
	}
	
	//CLEANUP
	pthread_barrier_destroy(&barrier);
	LB_del(new_state);
}
