import socket          
import threading
import time
import random

HEADER = 64           
PORT = 5050
SERVER = socket.gethostbyname(socket.gethostname())   
ADDR = (SERVER, PORT) 
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "n"
PLAYERS = 3 

conn = [0]
cards = [0]
recievedCards = [0]
winnerOfOneRound = [0]

for i in range(PLAYERS - 1):
    conn.append(0)
    winnerOfOneRound.append(0)
    recievedCards.append(0)

for i in range(PLAYERS*3 - 1):
    cards.append(0)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

server.bind(ADDR) 

#this function generates random numbers within the range and converts them to card numbers
def generateRandomCards():
    random.seed()
    i=0
    while i<3*PLAYERS:
        cards[i] = random.randint(1,52)
        j=0
        while j<i:
            if(cards[i] == cards[j]):
                i = i - 1
                break
            j+=1
        i+=1

    for k in range(3*PLAYERS):
        if((cards[k]%13) == 0):
            cards[k] = 13
        else:
            cards[k] %= 13

#this function is called once in every round and declares it's results as well as generates next set of random cards
def updateAndDeclare(round):
    time.sleep(12)
    multiWinner = 0
    highestAmongRecieved = 0
    for i in range(PLAYERS):
        if(highestAmongRecieved == recievedCards[i]):
            multiWinner += 1
        if(highestAmongRecieved < recievedCards[i]):
            highestAmongRecieved = recievedCards[i]
            multiWinner = 0
    if(multiWinner == 0):
        for i in range(PLAYERS):
            if(recievedCards[i] == highestAmongRecieved):
                winnerOfOneRound[i] += 1
                print("Winner of round-", round+1," is player-", i+1)
                print("\n")
    elif(multiWinner > 0):
        print("Round", round+1," was a tie between the players--", end=' ')
        for i in range(PLAYERS):
            if(recievedCards[i] == highestAmongRecieved):
                print(i+1, end = '    ')
        print("\n")
    generateRandomCards()

#this function distributes the random values to the clients
def sendTheCards(clientID):
    for i in range(PLAYERS):
        if clientID == conn[i]:
            clientID.send("Pick the largest among:".encode(FORMAT))
            time.sleep(1)
            yourCards = [0,0,0]
            for j in range(3):
                yourCards[j] = (cards[3*i + j])
            b=str(yourCards)
            clientID.send(b.encode(FORMAT))

#this function recieves 3 values from the clients, one in each round
def recieveTheHighest(clientID, addr,round):
    for i in range(PLAYERS):
        if clientID == conn[i]:
            msg_length = clientID.recv(HEADER).decode(FORMAT)
            if msg_length: 
                msg_length = int(msg_length) 
                msg = clientID.recv(msg_length).decode(FORMAT) 
                print(f"[{addr}] {msg}")
                recievedCards[i] = int(msg)
                if(round < 3):
                    clientID.send("Msg received, next round starts soon...".encode(FORMAT))
                else:
                    clientID.send("Game is over, results will be declared soon...".encode(FORMAT))

#this function sends and recieves all messages from the clients
def handle_clients(clientID, addr):

    while threading.active_count() -1 < PLAYERS:
        time.sleep(2)

    time.sleep(2)
    round=0
    while True:

        time.sleep(0.1)
        sendTheCards(clientID)
                            
        time.sleep(15)

        recieveTheHighest(clientID,addr,round)

        if(clientID == conn[0]):
            updateAndDeclare(round)
            time.sleep(8)
        else:
            time.sleep(20)  

        round+=1
        if(round == 4):
            break
    
    maxPoints=0
    
    winningMessage = "The winner(s) of the 4 rounds is(are) player(s)---"
    for m in range(PLAYERS):
        if(winnerOfOneRound[m] > maxPoints):
            maxPoints = winnerOfOneRound[m]
    for n in range(PLAYERS):
        if(maxPoints == winnerOfOneRound[n]):
            winningMessage = winningMessage + (' ' + str(n+1))
    if(clientID == conn[0]):
        print(winningMessage)
    clientID.send(winningMessage.encode(FORMAT))

    time.sleep(5)

    endingMessage = "Do you wish to continue? [y/n]"

    clientID.send(endingMessage.encode(FORMAT))
    msg_length = clientID.recv(HEADER).decode(FORMAT)
    if msg_length: 
        msg_length = int(msg_length) 
        msg = clientID.recv(msg_length).decode(FORMAT)
        if msg == DISCONNECT_MESSAGE:
            clientID.close()
        else:
            time.sleep(30)
            handle_clients(clientID, addr)
    
#this function starts the game only when we get "PLAYER" number of clients clients
def start():

    server.listen() 
    print(f"Server is listening on the IP Address: {SERVER}")
    
    while True: 
        i = threading.active_count() - 1
        while i < PLAYERS:
            conn[i], addr = server.accept()
            thread = threading.Thread(target = handle_clients, args=(conn[i],addr)) 
            thread.start()
            startingMessage = "Starting the game...[Please wait for other players to join]"
            conn[i].send(startingMessage.encode(FORMAT))
            time.sleep(0.5)
            playerNumber = "You are player "
            playerNumber = playerNumber + str((i+1))
            conn[i].send(playerNumber.encode(FORMAT))
            print(f"[NEW CONNECTION] {addr} connected.")
            i = threading.active_count() - 1
    
print("Server is starting...")
generateRandomCards()
start()