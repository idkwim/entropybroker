#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "pool.h"
#include "fips140.h"
#include "scc.h"
#include "config.h"
#include "client.h"
#include "utils.h"
#include "log.h"
#include "handle_pool.h"
#include "signals.h"

void help(void)
{
	printf("-c file   config-file to read\n");
        printf("-l file   log to file 'file'\n");
        printf("-s        log to syslog\n");
	printf("-S        statistics-file to log to\n");
        printf("-n        do not fork\n");
}

int main(int argc, char *argv[])
{
	int loop;
	pool **pools;
	int n_pools = 0;
	int c;
	char do_not_fork = 0, log_console = 0, log_syslog = 0;
	char *log_logfile = NULL;
	char *stats_file = NULL;
	fips140 *eb_output_fips140 = new fips140();
	scc *eb_output_scc = new scc();
	char *config_file = (char *)"/etc/entropybroker.conf";
	config_t config;

	memset(&config, 0x00, sizeof(config));

	printf("eb v " VERSION ", (C) 2009 by folkert@vanheusden.com\n");

	eb_output_fips140 -> set_user((char *)"output");
	eb_output_scc     -> set_user((char *)"output");

	while((c = getopt(argc, argv, "c:S:l:sn")) != -1)
	{
		switch(c)
		{
			case 'c':
				config_file = optarg;
				break;

			case 'S':
				stats_file = optarg;
				break;

			case 's':
				log_syslog = 1;
				break;

			case 'l':
				log_logfile = optarg;
				break;

			case 'n':
				do_not_fork = 1;
				log_console = 1;
				break;

			default:
				help();
				return 1;
		}
	}

	set_logging_parameters(log_console, log_logfile, log_syslog);

	load_config(config_file, &config);
	if (stats_file)
		config.stats_file = stats_file;

	eb_output_scc -> set_threshold(config.scc_threshold);

	if (!do_not_fork)
	{
		if (daemon(-1, -1) == -1)
			error_exit("fork failed");
	}

	set_signal_handlers();

	n_pools = config.number_of_pools;

	pools = (pool **)malloc(sizeof(pool *) * n_pools);
	for(loop=0; loop<n_pools; loop++)
		pools[loop] = new pool();

	dolog(LOG_DEBUG, "added %d bits of startup-event-entropy to pool", add_event(pools, n_pools, get_ts()));

	main_loop(pools, n_pools, &config, eb_output_fips140, eb_output_scc);

	return 0;
}
