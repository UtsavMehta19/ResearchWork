import socket
import time

HEADER = 64
PORT = 6033
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR) 
#we don't bind, but we connect to those addresses

def enteredTheGame(): 
    print(client.recv(2048).decode(FORMAT)) #starting message
    time.sleep(0.5)
    print(client.recv(2048).decode(FORMAT)) #you are player__

    time.sleep(2)
    round = 0
    while True:
        print("\n")
        print("Round", round+1)
        print(client.recv(2048).decode(FORMAT))
        time.sleep(1)
        max=0
        for j in range(3):
            num = client.recv(2048).decode(FORMAT)
            num=int(num)
            if(num>max):
                max=num
            print(num)
            time.sleep(0.5)

        time.sleep(1)

        #get the largest number as input and send it
        print("Sent the largest number:", max, "\nPlease wait for everyone to select")
        max=str(max)
        message = max.encode(FORMAT) 
        msg_length = len(message) 
        send_length = str(msg_length).encode(FORMAT) 
        send_length += b' ' * (HEADER - len(send_length)) 
        client.send(send_length)
        client.send(message) 
            
        print(client.recv(2048).decode(FORMAT))
        time.sleep(10)
        round+=1
        if(round == 4):
            break
    print("\n\n----------------------------------------------------------------------------------")
    print(client.recv(2048).decode(FORMAT))
    print("----------------------------------------------------------------------------------\n\n")
    time.sleep(5)

    #send if we want to continue
    print(client.recv(2048).decode(FORMAT))
    endingMessage = input()
    message = endingMessage.encode(FORMAT) 
    msg_length = len(message) 
    send_length = str(msg_length).encode(FORMAT) 
    send_length += b' ' * (HEADER - len(send_length)) 
    client.send(send_length)
    client.send(message)
    if(endingMessage == "y"):
        enteredTheGame()

enteredTheGame()