_This project has been created as part of the 42 curiculum by obehlil._

### Description
**Codexion** is a POSIX-threads simulation of coders competing for scarce USB dongles to compile quantum code. The program models concurrency, resource arbitration (FIFO and EDF), dongle cooldowns, and precise burnout detection to exercise synchronization, liveness, and scheduling policies. 



> "There are USB dongles on the table. There are as many dongles as coders."  
> "Compiling quantum code requires two dongles plugged in simultaneously, one in each hand: a coder takes their left and right dongles to compile."

# Instructions

## Build
```sh
make
```

- **Compiler flags:** `-Wall -Wextra -Werror -pthread`  
- **Make targets:** `NAME`, `all`, `clean`, `fclean`, `re`, `bonus` (if implemented).

## Run
```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```
- **scheduler** must be exactly `fifo` or `edf`.  
- All times are in **milliseconds**.

## Example
```sh
./codexion 5 800 200 200 200 3 50 edf
```

# Resources
**Primary references used while implementing this project:**
- POSIX threads and synchronization primitives (pthread_mutex, pthread_cond).  
- Real-time timing with `gettimeofday()` and `usleep()`.  
- Scheduling theory references for FIFO and EDF.

# Blocking cases handled
**Summary of concurrency issues addressed:**  
- **Deadlock prevention:** global design avoids circular wait by controlled acquisition order and scheduler-aware granting.  
- **Starvation prevention:** EDF implementation includes liveness guarantees so no coder is indefinitely postponed under feasible parameters.  
- **Cooldown handling:** dongles are unavailable for `dongle_cooldown` ms after release; cooldown timers are enforced per-dongle.  
- **Precise burnout detection:** a dedicated monitor thread checks deadlines and prints a `burned out` log within 10 ms of the actual burnout.  
- **Log serialization:** all output is protected by a logging mutex so messages never interleave.

# Thread synchronization mechanisms
**Primitives used and how they coordinate:**
- **`pthread_mutex_t` per dongle:** protects dongle state (free/held/cooldown) and the request queue.  
- **`pthread_cond_t` per dongle:** used to wake waiting coders when a dongle becomes available.  
- **Priority queue (heap):** implemented in C to order waiting requests by arrival (FIFO) or by deadline (EDF).  
- **Monitor thread:** reads shared timestamps under mutex protection to detect burnout and to signal termination.  
- **Logging mutex:** serializes all `printf` calls to prevent interleaved lines.

### Examples of race prevention
- Dongle state changes (take/release) are always performed while holding the dongle mutex; request enqueue/dequeue is atomic with respect to that mutex.  
- The monitor reads each coder’s `last_compile_start` under a dedicated mutex to avoid stale reads.

# Files and structure
- `coders/` — source files and headers.  
- `Makefile` — build rules.  
- `README.md` — this file.  



# Notes for evaluators and maintainers
- The program follows the Norm and forbids global variables; shared state is encapsulated in structs passed to threads.  
- All heap allocations are freed on termination.  
