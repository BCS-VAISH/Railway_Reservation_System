#include"reservation_server.h"

// File paths for text-file storage
char *user_file = "users.txt";
char *reservation_file = "seat.txt";
char *prices_file = "prices.txt";

//Functions to initialize semaphore variables
void init_semaphore() {
    save_reservations_semaphore = sem_open("/reservation_sem", O_CREAT, 0644, 1);
    if (save_reservations_semaphore == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }
    save_users_semaphore = sem_open("/user_sem", O_CREAT, 0644, 1);
    if (save_users_semaphore == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }
}

void cleanup_semaphore() {
    sem_close(save_reservations_semaphore);
    sem_unlink("/reservation_sem");
    sem_close(save_users_semaphore);
    sem_unlink("/user_sem");
}

int hash_password(char *password) {
    int hash = 0;
    while (*password) {
        hash += *password++;
    }
    return hash;
}

// Function to load users from text file
void load_users() {
    FILE *file = fopen(user_file, "r");
    if (file == NULL) return;

    fscanf(file, "%d", &user_count);
    for (int i = 0; i < user_count; i++) {
        fscanf(file, "%s %s %s", users[i].username, users[i].password, users[i].role);
    }
    fclose(file);
}

// Function to save users to text file
void save_users() {
    sem_wait(save_users_semaphore);
    FILE *file = fopen(user_file, "w");
    fprintf(file, "%d\n", user_count);
    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s %s %s\n", users[i].username, users[i].password, users[i].role);
    }
    fclose(file);
    sem_post(save_users_semaphore);
}

// Function to load reservations from text file
void load_reservations() {
    FILE *file = fopen(reservation_file, "r");
    if (file == NULL) return;

    fscanf(file, "%d %d %d %d %d %s %s", &waiting_list_count, &priority_queue_count, &pnr_counter, &total_demand, &total_cancellations,departure_date, departure_time);
    for (int i = 0; i < MAX_COACHES; i++) {
        fscanf(file, "%d", &available_seats[i]);
        printf("LOADing coach %d with %d available seats\n", i, available_seats[i]);
        for (int j = 0; j < MAX_SEATS; j++) {
            fscanf(file, "%d %s %s %d %s %d %d %f %[^\n]",
                &seats[i][j].pnr, seats[i][j].username, seats[i][j].name, &seats[i][j].age,
                seats[i][j].seat_num, &seats[i][j].coach_num, &seats[i][j].is_waitlisted, &seats[i][j].price, seats[i][j].booking_time);
             //    printf("%d %s %s %d %s %d %d %f %s\n\n",
             //   seats[i][j].pnr, seats[i][j].username, seats[i][j].name, seats[i][j].age,
             //   seats[i][j].seat_num, seats[i][j].coach_num, seats[i][j].is_waitlisted, seats[i][j].price, seats[i][j].booking_time);
        }      
        }
    
    for (int i = 0; i < waiting_list_count; i++) {
        fscanf(file, "%d %s %s %d %s %d %d %f %[^\n]",
            &waiting_list[i].pnr, waiting_list[i].username, waiting_list[i].name, &waiting_list[i].age,
            waiting_list[i].seat_num, &waiting_list[i].coach_num, &waiting_list[i].is_waitlisted,&waiting_list[i].price, waiting_list[i].booking_time);
    }

    for (int i = 0; i < priority_queue_count; i++) {
        fscanf(file, "%d %s %s %d %s %d %d %f %[^\n]",
            &priority_queue[i].pnr, priority_queue[i].username, priority_queue[i].name, &priority_queue[i].age,
            priority_queue[i].seat_num, &priority_queue[i].coach_num, &priority_queue[i].is_waitlisted, &priority_queue[i].price, priority_queue[i].booking_time);
    }

    fclose(file);
    sscanf(departure_date, "%d-%d-%d",&dep_yr,&dep_mon,&dep_day);
}

// Function to save reservations to text file
void save_reservations() {
     sem_wait(save_reservations_semaphore);
    FILE *file = fopen(reservation_file, "w");
    fprintf(file, "%d %d %d %d %d %s %s\n", waiting_list_count, priority_queue_count, pnr_counter, total_demand,total_cancellations,departure_date, departure_time);
    
    for (int i = 0; i < MAX_COACHES; i++) {
        fprintf(file, "%d\n", available_seats[i]);
        for (int j = 0; j < MAX_SEATS; j++) {
            fprintf(file, "%d %s %s %d %s %d %d %f %s\n",
                seats[i][j].pnr, seats[i][j].username, seats[i][j].name, seats[i][j].age,
                seats[i][j].seat_num, seats[i][j].coach_num, seats[i][j].is_waitlisted, seats[i][j].price,seats[i][j].booking_time);          
        }
        
    }

    for (int i = 0; i < waiting_list_count; i++) {
        fprintf(file, "%d %s %s %d %s %d %d %f %s\n",
            waiting_list[i].pnr, waiting_list[i].username, waiting_list[i].name, waiting_list[i].age,
            waiting_list[i].seat_num, waiting_list[i].coach_num, waiting_list[i].is_waitlisted, waiting_list[i].price,waiting_list[i].booking_time);
    }

    for (int i = 0; i < priority_queue_count; i++) {
        fprintf(file, "%d %s %s %d %s %d %d %f %s\n",
            priority_queue[i].pnr, priority_queue[i].username, priority_queue[i].name, priority_queue[i].age,
            priority_queue[i].seat_num, priority_queue[i].coach_num, priority_queue[i].is_waitlisted,priority_queue[i].price,priority_queue[i].booking_time);
    }
    fclose(file);
    sem_post(save_reservations_semaphore);
}

// Function to register a user
void register_user(int sock, const char *username,char *password) {
     for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            send(sock, "Username already exists.\n", 25, 0);
            return;
        }
    }

    if (user_count >= MAX_USERS) {
        printf("User registration full. Cannot add more users.\n");
        send(sock, "User registration full.\n", 25, 0);
        return;
    }
    User new_user;
    strcpy(new_user.username,username);
    sprintf(new_user.password, "%d", hash_password(password));
    strcpy(new_user.role,"user");
    users[user_count++] = new_user;
    save_users(); 
    send(sock, "Registration successful.\n", 25, 0);
}

// Function to log in a user
int login_user(int sock, const char *username, char *password) {
    char hashed_password[100];
    sprintf(hashed_password, "%d", hash_password(password));

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, hashed_password) == 0 && strcmp(users[i].role,"user")==0) {
            logged_in_user = i;
            strcpy(logged_username,username);
            return 1;
        }
    }

    return 0;
}

//Function to calculate prices dynamically
float get_price(int coach_num, bool is_windowseat, int date){
FILE *file = fopen(prices_file, "r");
    if (!file) {
        printf("Failed to open pricing file.\n");
        return 0.0;
    }

    float price = 0.0, c_price=0.0;
    int c_num;
     for (int i = 0; i < MAX_COACHES; i++) {
        fscanf(file, "%d %f",&c_num,&c_price); 
        if (c_num==coach_num) {
         price=c_price;
        }
     }

     if (price == 0.0) {
        printf("Invalid coach number or price not found.\n");
        return 0.0;
    }

    float window_factor;
      if(!is_windowseat)
       window_factor=1.0;
      else window_factor=1.25;

      if (available_seats[coach_num] > 0) {
       float demand_factor = 1 + 0.25 * (total_demand - total_cancellations) / MAX_SEATS;
        float cancellation_factor = 1 - 0.1 * (total_cancellations / (float)MAX_SEATS);
        float time_factor= 1+ 0.75*(1-((dep_day-date)/(float)dep_day));
        //printf("Time factor:%f\n",time_factor);
        float increment = demand_factor * cancellation_factor*window_factor*time_factor;
        float newprice=price*increment;
        price=(newprice<=MAX_PRICE)?newprice : MAX_PRICE;
      }

    fclose(file);
    return price;
}

//Function to display seat matrix coach-wise
void display_seat_matrix(int sock,int coach_num) {
    char buffer[2048]={0};
    int index=0;
    if (coach_num < 0 || coach_num >= MAX_COACHES) {
        send(sock,"Invalid coach number\n",50,0);
        return;
    }
 time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int current_day = tm_info->tm_mday;   
index+=sprintf(buffer+index, "\n\t\t============================================================");
if(coach_num==0)
index+=sprintf(buffer+index, "\n\t\t|    Engine   |$$$Coach 1$$$|   Coach 2   |    Coach 3     |");
else if(coach_num==1)
index+=sprintf(buffer+index, "\n\t\t|    Engine   |   Coach 1   |$$$Coach 2$$$|    Coach 3     |");
else
index+=sprintf(buffer+index, "\n\t\t|    Engine   |   Coach 1   |   Coach 2   |$$$ Coach 3  $$$|");
index+=sprintf(buffer+index, "\n\t\t============================================================\n\n\t\t\t"); 
index+=sprintf(buffer+index, "*Note: Prices may vary at time of reservation*");
index+=sprintf(buffer+index, "\n\t\t============================================================\n\n\t\t\t");  
index+=sprintf(buffer+index, "\n\t\t\tSeat Availability Matrix for Coach %d:", coach_num+1);
index+=sprintf(buffer+index, "\n\t\t============================================================\n\n\t\t\t");
index+=sprintf(buffer+index, "\n\t\t\t1\t2\t\t3\t4\t5\t\n\n\t\t");

for (int i = 0; i < MAX_ROWS; i++) {
    index += sprintf(buffer + index, "%c\t", 'A' + i);
        for (int j = 0; j < MAX_COLS; j++) {
             index += sprintf(buffer + index, "[ %c%d]", 'A' + i, j+1);  
            if (seats[coach_num][j + i * MAX_COLS].pnr != 0) {
                buffer[index - 2] = ' '; 
                buffer[index - 3] = 'X'; 
            }
            if(j==1){
             buffer[index++] = '\t'; 
             buffer[index++] = '\t';
            } 
             else  buffer[index++] = '\t'; 
        }
        index+=sprintf(buffer+index, "\n\t\t\t");

        for (int j = 0; j < MAX_COLS; j++) {
            float price;
            if(j==0 || j==MAX_COLS-1)
            price=get_price(coach_num,1,current_day);
            else price=get_price(coach_num,0,current_day);

            if(seats[coach_num][j + i * MAX_COLS].pnr == 0)
            index += sprintf(buffer + index, "%.2f ", price);
            else 
            index += sprintf(buffer + index, "       ");
            if (j == 1) {
                index+=sprintf(buffer+index, "       ");
            } else {
                index+=sprintf(buffer+index, "  ");
            } 
        }

         if(i % 2 == 1)
         index+=sprintf(buffer+index, "\n\n\n\n\t\t");
        else index+=sprintf(buffer+index, "\n\n\t\t");
    }
    buffer[index] = '\0'; 
    send(sock, buffer, strlen(buffer), 0);
}

//Function generate PNR
int generate_pnr() {
    return pnr_counter++;
}

//Function to reserve seat
void reserve_seat(int sock, const char*username,const char* name, const int age, const int coach_num,const char* seat_num) {
    if (logged_in_user == -1) {
        send(sock,"Please log in before reserving a seat.\n",50,0);
        return;
    }
    char buffer[1024]={0};
    Passenger p;
    strcpy(p.name,name);
    p.age=age;

    if (coach_num < 0 || coach_num >= MAX_COACHES) {
        send(sock,"Invalid coach number.\n",50,0);
        return;
    }
printf("Seat Coll %c\n",seat_num[1]);
    bool is_windowseat=false;
    if(seat_num[1]=='1' || seat_num[1]==(MAX_COLS)+'0'){
      is_windowseat=true;
      //printf("Seat Coll %c\n",seat_num[1]);
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int current_day = tm_info->tm_mday; 
    p.coach_num = coach_num;
    p.is_waitlisted = 0;
    strcpy(p.username, users[logged_in_user].username);
    p.price=get_price(coach_num, is_windowseat,current_day);
  
    if(available_seats[coach_num]){
   for (int i = 0; i < MAX_ROWS; i++) {
     for (int j = 0; j < MAX_COLS; j++) {
         if (strcmp(seats[coach_num][j + i * MAX_COLS].seat_num ,seat_num)==0) {
            if(seats[coach_num][j + i * MAX_COLS].pnr==0){
           strcpy(p.seat_num,seat_num); 
           p.pnr = generate_pnr();
          // char buffer1[50];
           time_t booking_time=time(NULL);
            struct tm *time_info = localtime(&booking_time);
            strftime(p.booking_time, sizeof(p.booking_time), "%Y-%m-%d %H:%M:%S", time_info);
            printf("Buffer Timeinfo%s\n",p.booking_time);
           seats[coach_num][j + i * MAX_COLS] = p;
           available_seats[coach_num]--;
           snprintf(buffer,sizeof(buffer),"Seat allocated to %s (PNR: %d, Seat No: %s, Coach: %d)\nPrice:%.2f", p.name, p.pnr, p.seat_num, coach_num + 1,p.price);
           send(sock, buffer, strlen(buffer), 0);
           save_reservations();
           return;
            }
            else if(seats[coach_num][j + i * MAX_COLS].pnr!=0){
              snprintf(buffer,sizeof(buffer),"Selected seat not available. Please choose another seat!!\n");
              send(sock, buffer, strlen(buffer), 0); 
              save_reservations();
           return;  
            }
    } 
        }
   }
    }
        p.pnr = generate_pnr();
        snprintf(buffer,sizeof(buffer),"No seats available in selected coach...Adding to waiting list!!\nPNR: %d\n",p.pnr);
        send(sock, buffer, strlen(buffer), 0);
        p.is_waitlisted = 1;
        strcpy(p.seat_num,"00");
        if(p.age>=60){
         priority_queue[priority_queue_count++]=p;
        }
        else{
        waiting_list[waiting_list_count++] = p;
        }

    save_reservations();  
}

//Function to check the reservation
void check_reservation(int sock, const char* username, int pnr, const char* name) {
     if (logged_in_user == -1) {
        send(sock,"Please log in to check reservation status\n",50,0);
        return;
    }
printf("Entered details: %d\n",pnr);
    char buffer[4096]={0};
    int index=0;
      for (int coach_num = 0; coach_num < MAX_COACHES; coach_num++) {
        for (int i = 0; i < MAX_SEATS; i++) {
            if (seats[coach_num][i].pnr == pnr && strcmp(seats[coach_num][i].name, name) == 0 /*&& strcmp(seats[coach_num][i].username, logged_username) == 0*/) { 
             index+=sprintf(buffer+index,"\t=======================================================\n");
             index+=sprintf(buffer+index,"\t\t\tTICKET CONFIRMATION\t\t\t\n");
             index+=sprintf(buffer+index,"\t=======================================================\n");
             index+=sprintf(buffer+index,"\t\tReservation found for username %s\n",seats[coach_num][i].username);
             index+=sprintf(buffer+index,"\t\tPassenger Name: %s\n",seats[coach_num][i].name);
             index+=sprintf(buffer+index,"\t\tAge: %d\n",seats[coach_num][i].age);
             index+=sprintf(buffer+index,"\t\tCoach Number: %d\n",coach_num+1);
             index+=sprintf(buffer+index,"\t\tSeat: %s\n",seats[coach_num][i].seat_num);
             index+=sprintf(buffer+index,"\t\tPrice: %.2f\n",seats[coach_num][i].price);
             index+=sprintf(buffer+index,"\t\tDeparture Time : %s %s\n",departure_date, departure_time);
             index+=sprintf(buffer+index,"\t\tBooking Time : %s\n", seats[coach_num][i].booking_time);
             index+=sprintf(buffer+index,"\t=======================================================\n");
                send(sock,buffer, strlen(buffer), 0);
                return;
            }
        }
      
    }

    for (int i = 0; i < waiting_list_count; i++) {
        if (waiting_list[i].pnr == pnr) {
            snprintf(buffer,sizeof(buffer),"You are on the waiting list. \nName: %s, Age: %d\n", waiting_list[i].name, waiting_list[i].age);
              send(sock,buffer, strlen(buffer), 0);
            return;
        }
    }

     snprintf(buffer,sizeof(buffer),"No reservation found for PNR: %d and passenger name: %s !!\nCheck entered details again!!\n\n", pnr,name);
     send(sock,buffer, strlen(buffer), 0);
}

//Function to cancel reservation
void cancel_reservation(int sock, const char* username, const int pnr) {
    if (logged_in_user == -1 || username=="") {
        send(sock,"Please log in to cancel reservation\n",50,0);
        return;
    }
    int found = 0;
    char buffer[1024] = {0};

    // Search for the reservation in the seats array
    for (int coach_num = 0; coach_num < MAX_COACHES; coach_num++) {
        for (int i = 0; i < MAX_SEATS; i++) {
            if (seats[coach_num][i].pnr == pnr && strcmp(seats[coach_num][i].username, username) == 0) {
                // Reservation found; cancel it
                snprintf(buffer, sizeof(buffer), "Reservation for %s (PNR: %d, Seat: %s, Coach: %d) cancelled.\n", seats[coach_num][i].name, pnr, seats[coach_num][i].seat_num, coach_num + 1);
                send(sock, buffer, strlen(buffer), 0);
                // Free up the seat
                strcpy(seats[coach_num][i].name,"0");
                strcpy(seats[coach_num][i].username,"0");
                strcpy(seats[coach_num][i].seat_num,seats[coach_num][i].seat_num);
                seats[coach_num][i].age = 0;
                seats[coach_num][i].coach_num = 0;
                seats[coach_num][i].pnr =0;
                seats[coach_num][i].price = 0.00;
                strcpy(seats[coach_num][i].booking_time,"0");
                available_seats[coach_num]++;

                // Check priority queue for possible reallocation
                if (priority_queue_count > 0 && priority_queue[0].coach_num == coach_num) {
                    // Reallocate seat to priority queue passenger
                  //  priority_queue[0].seat_num = seats[coach_num][i].seat_num;
                    priority_queue[0].is_waitlisted = 0;
                    strcpy(priority_queue[0].seat_num,seats[coach_num][i].seat_num);
                    seats[coach_num][i] = priority_queue[0];
                    available_seats[coach_num]--;

                    // Shift priority queue
                    for (int j = 1; j < priority_queue_count; j++) {
                        priority_queue[j - 1] = priority_queue[j];
                    }
                    priority_queue_count--;
                }
                // If no one in priority queue, check waiting list
                else if (waiting_list_count > 0 && waiting_list[0].coach_num == coach_num) {
                    // Reallocate seat to waiting list passenger
                   // waiting_list[0].seat_num = seats[coach_num][i].seat_num;
                    waiting_list[0].is_waitlisted = 0;
                    strcpy(waiting_list[0].seat_num,seats[coach_num][i].seat_num);
                    seats[coach_num][i] = waiting_list[0];
                    available_seats[coach_num]--;

                    // Shift waiting list
                    for (int j = 1; j < waiting_list_count; j++) {
                        waiting_list[j - 1] = waiting_list[j];
                    }
                    waiting_list_count--;

                }
                total_cancellations++;
                found = 1;
                save_reservations();  // Save updated reservation data
                break;
            }
        }
        if (found) break;
    }

    // If reservation was not found in the seats array, check waiting list
    if (!found) {
        if(waiting_list_count>0){
        for (int i = 0; i < waiting_list_count; i++) {
            if (waiting_list[i].pnr == pnr && strcmp(waiting_list[i].username, username) == 0) {
                snprintf(buffer, sizeof(buffer), "Reservation for %s (PNR: %d) cancelled from waiting list.\n", waiting_list[i].name, pnr);
                send(sock, buffer, strlen(buffer), 0);

                // Remove passenger from waiting list
                for (int j = i + 1; j < waiting_list_count; j++) {
                    waiting_list[j - 1] = waiting_list[j];
                }
                waiting_list_count--;
                total_cancellations++;
                found = 1;
                save_reservations();  // Save updated reservation data
                break;
            }
        }
    }
    }

    // If no reservation was found
    if (!found) {
        snprintf(buffer, sizeof(buffer), "This is PNR not booked under given username.\n");
        send(sock, buffer, strlen(buffer), 0);
    }
}

//Function to desplace dynamic prices
void display_upcoming_prices(int sock,int coach_num) {
    char buffer[2048]={0};
    int index=0;
    if (coach_num < 0 || coach_num >= MAX_COACHES) {
        send(sock,"Invalid coach number\n",50,0);
        return;
    }

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    int current_day = tm_info->tm_mday;
    int current_month = tm_info->tm_mon + 1;  // tm_mon starts at 0 for January
    int current_year = tm_info->tm_year + 1900;
    
index+=sprintf(buffer+index, "\n\t\t============================================================");
index+=sprintf(buffer+index, "\n\t\t\tPrices for upcoming days for Coach %d",coach_num+1);
index+=sprintf(buffer+index, "\n\t\t============================================================\n");
    for (int i = 0; i <5; i++) {
        int future_day = current_day + i;
        float approx_price = get_price(coach_num, 0,future_day);
        index+=sprintf(buffer+index,"\n\t\tDate: %d-%d-%d\t\tApproximate Price: %.2f\n", current_year, current_month, future_day, approx_price);
    }
    buffer[index] = '\0'; 
    send(sock, buffer, strlen(buffer), 0);
}

// Function to handle client requests
void handle_client(int sock) {
     char buffer[1024]={0};
    int valread;

    // Read the client's request
    while ((valread = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[valread] = '\0';
        printf("Request from client: %s\n", buffer);

        // Command parsing
        char command[20], username[50], password[50];
        sscanf(buffer, "%s", command);

        if (strcmp(command, "REGISTER") == 0) {
            sscanf(buffer + strlen(command) + 1, "%s %s", username, password);
            register_user(sock, username, password);
        } 
        else if (strcmp(command, "LOGIN") == 0) {
            sscanf(buffer + strlen(command) + 1, "%s %s", username, password);
            if (login_user(sock, username, password)) {
                send(sock, "Login successful.\n", 18, 0);
            } else {
                send(sock, "Login failed.Invalid Credentials!!\n", 30, 0);
            }
        }
        
        else if (strcmp(command, "DISPLAY_SEATS") == 0) {
            int coach_num;
            sscanf(buffer, "DISPLAY_SEATS %d",&coach_num);
            display_seat_matrix(sock,coach_num);
        }
        
        else if (strcmp(command, "BOOK") == 0) {
            char name[50],seat_num[3];
            int age,coach_num;
            sscanf(buffer, "BOOK %s %s %d %d %s",username,name,&age,&coach_num,seat_num);
            reserve_seat(sock, username, name, age,coach_num,seat_num);
            total_demand++;
        }
        
        else if (strcmp(command, "CANCEL") == 0) {
            int pnr;
            sscanf(buffer, "CANCEL %s %d",username,&pnr);
            cancel_reservation(sock, username,pnr);
        }
        
        else if (strcmp(command, "RES_STATUS") == 0) {
            int pnr;
            char name[50];
            sscanf(buffer, "RES_STATUS %s %d %s",username,&pnr,name);
            check_reservation(sock, username,pnr,name);
        }
        else if (strcmp(command, "EXIT") == 0) {
            save_reservations();
        }
       else if (strcmp(command, "DISPLAY_PRICE") == 0) {
            int coach_num;
            sscanf(buffer, "DISPLAY_PRICE %d",&coach_num);
            display_upcoming_prices(sock,coach_num);
        }
         
        else {
            send(sock, "Unknown command\n", 16, 0);
        }
        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
}

int server_fd;//Global server file descriptor

// Signal handler to catch Ctrl + C (SIGINT)
void handle_sigint(int sig) {
    printf("\nCaught signal %d. Closing the server socket...\n", sig);
    // Close the server socket
    if (server_fd >= 0) {
        save_reservations();
        //cleanup_semaphore();
        close(server_fd);
        printf("Server socket closed.\n");
    }
    save_reservations();
    close(server_fd);

    exit(0); 
}

void init_file(){
    FILE *file = fopen("seat.txt", "w");
    fprintf(file, "0 0 1000 0  0 2024-12-17 15:30:00\n");
    
    for (int i = 0; i < MAX_COACHES; i++) {
        fprintf(file, "%d\n",MAX_SEATS);
        for (int j = 0; j < MAX_ROWS; j++) {
           for (int k = 0; k < MAX_COLS; k++) {  
            fprintf(file, "0 0 0 0 %c%d 0 0 0.00 0\n",'A'+j,k+1);          
        }
        }
        
    }
    fclose(file);
}

int main() {
 //init_file(); //Uncomment to initialize the reservation file
 signal(SIGINT, handle_sigint);
 signal(SIGTERM, handle_sigint);
 init_semaphore();

    int client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    load_users();
    load_reservations();
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //Binds to any 
    address.sin_port = htons(PORT);
    //address.sin_addr.s_addr =inet_addr("172.17.198.41"); //Put IP Address to assign to the server

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d at IP: %s\n\n", PORT,inet_ntoa(address.sin_addr));

    // Accept incoming connections and create new thread to handle each client
    while ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        pthread_t client_thread;
        printf("New client connected.\n");
        pthread_create(&client_thread, NULL, (void *)handle_client, (void *)(intptr_t)client_socket);
        pthread_detach(client_thread);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    cleanup_semaphore();
    close(client_socket);
    printf("Server closed.\n");
    return 0;
}
