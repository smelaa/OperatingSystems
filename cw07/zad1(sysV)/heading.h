#define N_HAIRDRS 4
#define N_CHAIRS 2 //N_HAIRDRS >= N_CHAIRS 
#define N_QUEUE 2
#define N_CLIENTS 10 //N_CLIENTS >= N_HAIRDRS

#define SHM_PATH getenv("HOME")
#define SHM_ID 1

#define HAIRDRS_SEM_PATH getenv("HOME")
#define HAIRDRS_SEM_ID 2

#define CHAIRS_SEM_PATH getenv("HOME")
#define CHAIRS_SEM_ID 3

#define QUEUE_SEM_PATH getenv("HOME")
#define QUEUE_SEM_ID 4

#define CLIENTS_SEM_PATH getenv("HOME")
#define CLIENTS_SEM_ID 5

#define READY_SEM_PATH getenv("HOME")
#define READY_SEM_ID 6

#define HAIRDR_EXEC "./hairdresser"