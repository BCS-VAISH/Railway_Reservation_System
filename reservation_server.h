#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

// Named Semaphores
sem_t *save_reservations_semaphore;
sem_t *save_users_semaphore;

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_SEATS 45
#define MAX_WAITING_LIST 20
#define MAX_USERS 20
#define MAX_COACHES 3
#define MAX_ROWS 9
#define MAX_COLS 5
#define MAX_PRICE 5000

// User structure
typedef struct {
    char username[100];
    char password[100];
    char role[10];
} User;

// Passenger structure
typedef struct {
    char username[100];
    char name[100];
    int age;
    int pnr;
    char seat_num[3];
    int coach_num;
    int is_waitlisted;
    float price;
    char booking_time[50];
} Passenger;

User users[MAX_USERS];
int user_count = 0;
int logged_in_user=-1;
char logged_username[100];
Passenger seats[MAX_COACHES][MAX_SEATS];
Passenger waiting_list[MAX_WAITING_LIST];
Passenger priority_queue[MAX_WAITING_LIST];

int waiting_list_count = 0;
int priority_queue_count = 0;
int pnr_counter = 1000;
int available_seats[MAX_COACHES];
int total_demand;
int total_cancellations;
char departure_date[20];
char departure_time[20];
int dep_day,dep_mon,dep_yr;

//Functions to initialize semaphore variables
void init_semaphore();

void cleanup_semaphore();

int hash_password(char *password) ;

// Function to load users from text file
void load_users();

// Function to save users to text file
void save_users();

// Function to load reservations from text file
void load_reservations();

// Function to save reservations to text file
void save_reservations();

// Function to register a user
void register_user(int sock, const char *username,char *password);

// Function to log in a user
int login_user(int sock, const char *username, char *password);

//dynamic price
float get_price(int coach_num, bool is_windowseat, int date);

//display seat matrix
void display_seat_matrix(int sock,int coach_num);

//generate PNR
int generate_pnr();

//Book seat
void reserve_seat(int sock, const char*username,const char* name, const int age, const int coach_num,const char* seat_num);

void check_reservation(int sock, const char* username, int pnr, const char* name);

void cancel_reservation(int sock, const char* username, const int pnr);

//price predictions
void display_upcoming_prices(int sock,int coach_num);

// Function to handle client requests
void handle_client(int sock);

int server_fd;//Global server file descriptor

// Signal handler to catch Ctrl + C (SIGINT)
void handle_sigint(int sig);

//initialize the seat matrix
void init_file();
