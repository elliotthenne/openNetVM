/*********************************************************************
 *                     openNetVM
 *              https://sdnfv.github.io
 *
 *   BSD LICENSE
 *
 *   Copyright(c)
 *            2015-2021 George Washington University
 *            2015-2021 University of California Riverside
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * The name of the author may not be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * skeleton.c - Template NF for development.    [EDIT]
 ********************************************************************/

#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>

#include <rte_common.h>
#include <rte_ip.h>
#include <rte_mbuf.h>
#include <rte_malloc.h>

#include "onvm_nflib.h"
#include "onvm_pkt_helper.h"

// Define NF Tag Appropriately          [EDIT]
#define NF_TAG "skeleton"

/*
 * State structs                        [EDIT]
 *      Holds any variables which are universal to all files within the NF
 *      Listed below are common variables used within the struct
 */
struct skeleton_state {

        int displayType;
        uint64_t start_time;
        uint32_t delay;
        uint32_t current_time;
        uint32_t packets_processed;
};

/*
 * Print a usage message
 */
static void
usage(const char *progname) {
        printf("Usage:\n");
        printf("%s [EAL args] -- [NF_LIB args]\n", progname);
        
        // Additional Usage Messages    [EDIT]
        printf("%s -F <CONFIG_FILE.json> [EAL args] -- [NF_LIB args] -- [NF args]\n\n", progname);
        printf("Flags:\n");
        printf(" - `-p <delay>`: number of packets between each print, e.g. `-p 1` prints every packets.\n");
}

/*
 * Parse the application arguments.
 */
static int
parse_app_args(int argc, char *argv[], const char *progname, struct skeleton_state *skeleton_data) {
        int c;
        
        /* User can specify how they would like to display data. */ 
        
        skeleton_data->displayType = -1;                                // displayType -1 = No Display
        while ((c = getopt(argc, argv, "p:v:")) != -1) {
                switch (c) {
                        case 'p':
                                skeleton_data->delay = strtoul(optarg, NULL, 10);
                                if (skeleton_data->delay < 1) {
                                        RTE_LOG(INFO, APP, "Option -%c requires an argument.\n", optopt);
                                        return -1;
                                }
                                skeleton_data->displayType = 0;         // displayType 0 = Time-Based Delays
                                return optind;
                        case 'v':
                                skeleton_data->delay = strtoul(optarg, NULL, 10);
                                if (skeleton_data->delay < 1) {
                                        RTE_LOG(INFO, APP, "Option -%c requires an argument.\n", optopt);
                                        return -1;
                                }
                                skeleton_data->displayType = 1;         // displayType 1 = Packet-Based Delays
                                return optind;
                        case '?':
                                usage(progname);
                                if (optopt == 'p')
                                        RTE_LOG(INFO, APP, "Option -%c requires an argument.\n", optopt);
                                else if (optopt == 'v')
                                        RTE_LOG(INFO, APP, "Option -%c requires an argument.\n", optopt);
                                else if (isprint(optopt))
                                        RTE_LOG(INFO, APP, "Unknown option `-%c'.\n", optopt);
                                else
                                        RTE_LOG(INFO, APP, "Unknown option character `\\x%x'.\n", optopt);
                                return -1;
                        default:
                                usage(progname);
                                return -1;
                }
        }
        return optind;
}

/*
 * Function for NFs with Displays       
 *      [EDIT IF NF USES DISPLAY; ELSE DELETE]
 */
static void
do_stats_display(struct skeleton_state *skeleton_data) {
        
        const char clr[] = {27, '[', '2', 'J', '\0'};
        const char topLeft[] = {27, '[', '1', ';', '1', 'H', '\0'};

        /* Clear screen and move to top left */
        printf("%s%s", clr, topLeft);

        printf("PACKETS\n");
        printf("-----\n");
        printf("Number of packet processed : %d\n", skeleton_data->packets_processed);
        printf("Time : %d\n", skeleton_data->current_time);

        printf("\n\n");
}


/*
 * Handles each packet upon arrival     [EDIT]
 */
static int
packet_handler(__attribute__((unused)) struct rte_mbuf *pkt, struct onvm_pkt_meta *meta,
               struct onvm_nf_local_ctx *nf_local_ctx) {
        
        struct skeleton_state *skeleton_data;
        skeleton_data = (struct skeleton_state *)nf_local_ctx->nf->data;
        skeleton_data->packets_processed++;
        
        /* 
         * Timed display current_time (current_time stored in skeleton_state - keep/edit if NF uses display)
         * After the reception of each packet, the current_time is incremented. Once we reach a defined
         * number of packets, display and reset the current_time
         */
        
        if (skeleton_data->displayType == 1) {
                if (skeleton_data->packets_processed % skeleton_data->delay == 0) {
                        do_stats_display(skeleton_data);
                }
        }
        
        /* 
         * meta->action
         *      Defines what to do with the packet. Actions defined in onvm_common.h
         * meta->destination 
         *      Defines the service ID of the NF which will receive the packet.           
         *
         * Possible actions:    
         *      ONVM_NF_ACTION_DROP - Drop packet
         *      NVM_NF_ACTION_NEXT - Go to whatever the next action is configured by the SDN controller in the flow table
         *      ONVM_NF_ACTION_TONF - Send the packet to the NF specified in the argument field (assume it is on the same host)
         *      ONVM_NF_ACTION_OUT - Send the packet out the NIC port set in the argument field
         */
        meta->action = ONVM_NF_ACTION_DROP;

        /* Return 0 on success, -1 on failure */
        return 0;
}

/*
 * Performs action continuously; called every run loop regardless of packet reception     [EDIT]
 */
static int 
action(struct onvm_nf_local_ctx *nf_local_ctx){
        
        struct skeleton_state *skeleton_data;
        skeleton_data = (struct skeleton_state *)nf_local_ctx->nf->data;

        /* 
         * Get the current time, and, if the user has specified a timer-based print delay,
         * check whether it is time to print
         */
        __uint64_t time = (rte_get_tsc_cycles() - skeleton_data->start_time) / rte_get_timer_hz();
        if (skeleton_data->displayType == 0) {
                __uint64_t delay = skeleton_data->delay;
                __uint64_t current_time = skeleton_data->current_time;
                if ((time%delay == 0) && (time != current_time)) {
                        skeleton_data->current_time = time;
                        do_stats_display(skeleton_data);
                        return 0;
                }
        }

        /* Update the current time */
        skeleton_data->current_time = time;
        return 0;
}

/*
 * Initial setup function; called before NF receives any packets/messages                 [EDIT]
 */
static void 
setup(struct onvm_nf_local_ctx *nf_local_ctx){

        /* 
         * Initialize variables within state struct that must be defined
         * before receiving packets/messages 
         */
        
        struct skeleton_state *skeleton_data = nf_local_ctx->nf->data;
        skeleton_data->start_time = rte_get_tsc_cycles();
        skeleton_data->current_time = 0;
        skeleton_data->packets_processed = 0;
        if (skeleton_data->displayType > -1) {
                do_stats_display(skeleton_data);
        }
}

/*
 * Handles each message upon arrival     [EDIT]
 */
static void 
handle_msg(__attribute__((unused))void *msg_data, __attribute__((unused))struct onvm_nf_local_ctx *nf_local_ctx) {

        /* Remove __attribute__((unused)) once implemented */

}

/*
 * Creates function table and local context. Runs NF.
 */
int
main(int argc, char *argv[]) {
        
        /* Handles command line arguments */
        int arg_offset;
        
        /* Local context holds NF and status */
        struct onvm_nf_local_ctx *nf_local_ctx;
        
        /* Declare function table: Holds pointers to the NF methods - setup, msg_handler, user_actions, and pkt_handler */
        struct onvm_nf_function_table *nf_function_table;
        
        /* Handles command line arguments */
        const char *progname = argv[0];                         

        /* Initialize local context and start default signal handler */
        nf_local_ctx = onvm_nflib_init_nf_local_ctx();
        onvm_nflib_start_signal_handler(nf_local_ctx, NULL);

        /* Initialize function table and respective pointers to NF methods: */
        nf_function_table = onvm_nflib_init_nf_function_table();
        nf_function_table->pkt_handler = &packet_handler;
        nf_function_table->setup = &setup;
        nf_function_table->user_actions = &action;
        nf_function_table->msg_handler = &handle_msg;

        /* If a termination signal is received or initiation is interrupted, exit */
        if ((arg_offset = onvm_nflib_init(argc, argv, NF_TAG, nf_local_ctx, nf_function_table)) < 0) {
                onvm_nflib_stop(nf_local_ctx);
                if (arg_offset == ONVM_SIGNAL_TERMINATION) {
                        printf("Exiting due to user termination\n");
                        return 0;
                } else {
                        rte_exit(EXIT_FAILURE, "Failed ONVM init\n");
                }
        }

        /* Command line arguments */
        argc -= arg_offset;
        argv += arg_offset;

        /* 
         * (1) Declare and initialize state struct to hold non-local data within the NF. Use rte_zmalloc
         *              void * rte_zmalloc (const char * type, size_t size, unsigned align)
         *                      type: A string identifying the type of allocated objects. Can be NULL.
         *                      size: Size (in bytes) to be allocated.
         *                      align: If 0, the return is a pointer that is suitably aligned for any kind of variable
         * (2) Assign the nf_local_ctx->nf->data pointer to the struct.
         */
        struct skeleton_state *skeleton_data;
        skeleton_data = (struct skeleton_state *) rte_zmalloc ("skeleton", sizeof(struct skeleton_state), 0);
        nf_local_ctx->nf->data = (void *) skeleton_data;

        /* Invalid command-line argument handling */
        if (parse_app_args(argc, argv, progname, skeleton_data) < 0) {
                onvm_nflib_stop(nf_local_ctx);
                rte_exit(EXIT_FAILURE, "Invalid command-line arguments\n");
        }

        /* Begin running NF */
        onvm_nflib_run(nf_local_ctx);

        /* Once the NF has stopped running, free and stop */
        onvm_nflib_stop(nf_local_ctx);
        printf("If we reach here, program is ending\n");
        return 0;
}
