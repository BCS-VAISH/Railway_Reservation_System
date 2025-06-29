#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void clear_screen() {
    printf("\033[H\033[J");
}

void display_menu() {
    clear_screen();
   printf("\n==========================================\n");
        printf("   Welcome to Railway Reservation System\n");
        printf("==========================================\n");
        printf("[1] Register\n");
        printf("[2] Login\n");
        printf("[3] Display Seat Availability Matrix\n");
        printf("[4] Reserve Seat\n");
        printf("[5] Check Reservation Status\n");
        printf("[6] Check prices for upcoming days\n");
        printf("[7] Cancel reservation\n");
        printf("[8] Exit\n");
    printf("\n\nChoose an option: ");
}

int main() {
    int sock = 0;
    int newsock=0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error.\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {  //Works on same machine
        printf("Invalid address.\n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed.\n");
        return -1;
    }

    printf("Connected to the server.\n");
     char username[50] = {0};
    char password[50] = {0};
    char seat_num[3]={0};
    int option;
    char buffer[2048] = {0};
    char name[50]={0};
    int age,coach_num,pnr;

    do{
        display_menu();
        scanf("%d", &option);
        getchar(); // To consume the newline character

        switch (option) {
            case 1: // Register
                printf("To register:\nEnter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline

                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0; // Remove newline

                snprintf(buffer, sizeof(buffer), "REGISTER %s %s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                break;

            case 2: // Login
                printf("To login:\nEnter username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0; // Remove newline
                printf("Enter password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0; // Remove newline
                snprintf(buffer, sizeof(buffer), "LOGIN %s %s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                break;

            case 3: // Display Seats
                printf("To view seat matrix:\nEnter coach number(1-3): ");
                scanf("%d",&coach_num);
                getchar();
                coach_num--;
                snprintf(buffer, sizeof(buffer), "DISPLAY_SEATS %d",coach_num);
                send(sock, buffer, strlen(buffer), 0);
                break;

            case 4: // Book Seat
               printf("To book a seat:\nEnter Passenger Name: ");
               scanf("%s", name);
               printf("Enter Age: ");
               scanf("%d", &age);
               printf("Choose a coach(1-3): ");
               scanf("%d", &coach_num);
               getchar(); // To consume the newline character
               coach_num--;
               printf("Choose a seat: ");
               scanf("%s", seat_num);
               getchar();
               snprintf(buffer, sizeof(buffer), "BOOK %s %s %d %d %s", username,name,age,coach_num,seat_num);
               send(sock, buffer, strlen(buffer), 0);
               break;

            case 5: // View Reservation Status
            printf("To view reservation status:\nEnter PNR: ");
            scanf("%d", &pnr);
            printf("Enter Passenger Name corresponding to entered PNR: ");
            scanf("%s", name);
            getchar();
                snprintf(buffer, sizeof(buffer), "RES_STATUS %s %d %s", username,pnr,name);
                send(sock, buffer, strlen(buffer), 0);
                break;
            case 6:
            printf("To view approximate pricing for upcoming days:\nSelect coach number(1-3): ");
                scanf("%d",&coach_num);
                getchar();
                coach_num--;
                snprintf(buffer, sizeof(buffer), "DISPLAY_PRICE %d",coach_num);
                send(sock, buffer, strlen(buffer), 0);
                break;

            case 7: // Cancel Reservation
                printf("To cancel reservation:\nEnter PNR: ");
                scanf("%d", &pnr);
                getchar(); // To consume the newline character
                snprintf(buffer, sizeof(buffer), "CANCEL %s %d", username, pnr);
                send(sock, buffer, strlen(buffer), 0);
                break;    

            case 8: // Exit
             printf("\n\nThank you for using Railway Reservation System!!\n\n");
                snprintf(buffer, sizeof(buffer), "EXIT");
                send(sock, buffer, strlen(buffer), 0);
                close(sock);
                return 0;


            default:
                printf("Invalid option, please try again.\n");
                continue;
        }
        clear_screen();
         int valread = read(sock, buffer, sizeof(buffer) - 1);
        buffer[valread] = '\0';
        printf("\nServer response:\n %s\n", buffer);
        
       

       if(option!=8){
       printf("\nPress Enter to continue...");
       getchar();
    }
    }while(option!=8);

    close(sock);
    return 0;
}
