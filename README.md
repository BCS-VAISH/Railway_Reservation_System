# Railway_Reservation_System
Project_BasedOn Operating System Features(Semaphore, Tech:C and Socket Programming)
# OS-Project-Repository
This project was made as part of OS LAB MINI PROJECT 

RAILWAY RESERVATION PROJECT: To establish a client- server architecture in order to maintain a railway reservation system.

## Information about the files present: ##
(All the code files need to be present on both the server and client machines in order to run the login file function)

### reservation_server.h 
This is the header file containing all the global variables, structures and function declarations.

### reservation_server.c 
This is the main server file which will handle the client requests. It contains all the function definitions as well as socket code to communicate with the client(s). 

The init_file() function initializes the seat.txt file. To initialize the seat.txt for a particular date, make changes to the init_file function in the server code.
                       
Uncomment the init_file() function in the main function to reset/initialize the seat.txt file. Else use the sample seat.txt file provided in the Repository

### reservation_client.c 
Client can make requests to the server. It contains all socket code required to send the server requests and recieve server responses to the sent requests.

### login.c 
Contains the main login interface, where the user can either start the server or use the client interface.

The default admin username to start the server: vaishnavan_bcs

default admin password: vaish

### users.txt 
Contains the username and hashed password along with the role of the user.

The default admin username: 
The default admin username to start the server: vaishnavan_bcs 
                 
default admin username: vaish
 
The role of the user may be changed by manually editing the users.txt

### prices.txt 
Contains the base price of each coach

### seat.txt 
Contains the reservation information of each coach(Go through the server code file for more info)


## *** Important Note: Please read the "Steps to Execute" PDF for detailed information ##

  1. Compile the files in the following order (It is important to compile the server and client files before running the login file):  

* gcc reservation_server.c -o server..

* gcc reservation_client.c -o client

* gcc login.c -o login


2. Run the login file by the following command:
  
   * ./login

### First time run:
 Make sure to uncomment the init_file() line. This will initialize the seat.txt file for a fresh run.
 
 ### Subsequent runs of the server:
  
 * Comment the init_file() function in the main function of the server code

## Provided code will work for server and client(s) running on the same machine. 


In order to run it from different machines on the same network, follow the steps below:

To run of different machines on the same network:

(*These are the steps to be followed if the server is running on WSL*):

 * Enable port forwarding and disable firewall

* For port forwarding: 
    ->Run on Windows Powershell as admin: netsh interface portproxy add v4tov4 listenaddress=0.0.0.0 listenport=8080 connectaddress=<server ip> connectport=8080
             (Replace with actual IP address of your server instance, which can be found out by typing "ip addr" in the WSL instance where the server will run)
      
* To disable Firewall:
    * Open Windows Firewall Settings:
  
    * Open Windows Defender Firewall. Open Advanced Settings. Create a New Inbound Rule.
  
    * Select Rule Type: Specify Port: Choose TCP (or TCP/UDP if necessary).
  
    * Select Specific local ports and enter the port number you want to open (e.g., 8080).
  
    * Choose Allow the connection and click Next.
  
    * Profile Selection: Select when this rule applies:
  
  (You can select all profiles if unsure, but make sure to at least select Private for most local network connections)
  
Replace the IP address in the client code with the physical IP address of your server machine (Not the WSL instance)
