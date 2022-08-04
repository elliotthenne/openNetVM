Skeleton
==
This is an example NF that acts as a basic skeleton NF.

Compilation and Execution
--
```
cd examples
make
cd skeleton
./go.sh SERVICE_ID [-p TIME_PRINT_DELAY | -v PACKET_PRINT_DELAY]

OR

./go.sh -F CONFIG_FILE -- -- [-p PRINT_DELAY]

OR

sudo ./build/skeleton -l CORELIST -n 3 --proc-type=secondary -- -r SERVICE_ID -- [-p TIME_PRINT_DELAY | -v PACKET_PRINT_DELAY]
```

App Specific Arguments
--
  - `-p <time_print_delay>`: time between each print, e.g. `-p 1` prints after every second.
  - `-v <packet_print_delay>`: number of packets between each print, e.g. `-v 1` prints after every packet.

Config File Support
--
This NF supports the NF generating arguments from a config file. For
additional reading, see [Examples.md](../../docs/Examples.md)

See `../example_config.json` for all possible options that can be set.
