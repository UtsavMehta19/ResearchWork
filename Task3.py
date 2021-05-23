#green dot is at 50,50 with color combination BGR: 30,220,7
#and red dot at 550,550 with color combination: 9,10,250

import random as rand
import cv2 as cv
import sys
import math

img = cv.imread("/home/utsav1902/Pictures/image1.png")
# img = cv.imread("/home/utsav1902/Pictures/image2.png")
#also update the obstacle check function for the second image

if img is None:
    sys.exit("Could not read the image.")

SAFE_DISTANCE_FROM_OBSTACLE = 5
COST_OF_EDGE = 4
PRECISION_VALUE = 4000

allowed_color = (0,0,0)
red_range_low = (0,0,230)
red_range_high = (30,30,255)
green_range_low = (0,220,0)
green_range_high = (30,255,30)
obstacle_color=(255,255,255)

nodesFromStart = []
nodesFromEnd = []

#a class to store nodes evaluated and stores all edges related to it, also stores the path to reach that node
class nodes_evaluated():
    def __init__(self, abcissa = None, ordinate = None ,cost = None, parentNode = None):
        self.abcissa = abcissa
        self.ordinate = ordinate
        self.cost = cost
        self.parentNode = parentNode

class mainCode():

    #a function to start from starting point
    def start(self,startAbcissa, startOrdinate, cost):
        startNode = nodes_evaluated(startAbcissa, startOrdinate, cost)
        startNode.parentNode = None
        self.nodes.append(startNode)

    def __init__(self,nodes=[]):
        self.nodes = nodes

    def obstacleCheck(self,x,y):

        check = True
        for i in range(SAFE_DISTANCE_FROM_OBSTACLE):
            for j in range(SAFE_DISTANCE_FROM_OBSTACLE):
                if((img[x+i-1,y-1+j] == obstacle_color).any()):
                #for second image, comment the above if statement and uncomment the below one
                # if((img[x+i-1,y-1+j] != allowed_color).all() or ((img[x+i-1,y-1+j] > green_range_high).all() and (img[x+i-1,y-1+j] < green_range_low).all()) or ((img[x+i-1,y-1+j] > red_range_high).all() and (img[x+i-1,y-1+j] < red_range_low).all())):
                    check = False
                    break 
            if(check == False):
                break

        return check

    #find the euclidistally nearest node
    def findNearestNode(self,randomAbcissa, randomOrdinate):
        gotcha = None
        condition = True
        minDist = 99999
        for aNode in self.nodes:
            distance = math.sqrt((randomAbcissa - aNode.abcissa)**2 + (randomOrdinate - aNode.ordinate)**2)
            if(distance < minDist): #can also put a condition for angle < 270 or so
                minDist = distance
                gotcha = aNode
        
        #then we find the advanced node i.e., a node in direction of random node, from the nearest node
        if(randomOrdinate - gotcha.ordinate != 0):
            thetha = math.atan((randomAbcissa - gotcha.abcissa)/(randomOrdinate - gotcha.ordinate))
            if(randomAbcissa - gotcha.abcissa >= 0 and randomOrdinate - gotcha.ordinate > 0):
                newNodeAbcissa = int(gotcha.abcissa + COST_OF_EDGE*math.sin(thetha))
                newNodeOrdinate = int(gotcha.ordinate + COST_OF_EDGE*math.cos(thetha))
            elif(randomAbcissa - gotcha.abcissa >= 0 and randomOrdinate - gotcha.ordinate < 0):
                newNodeAbcissa = int(gotcha.abcissa - COST_OF_EDGE*math.sin(thetha))
                newNodeOrdinate = int(gotcha.ordinate - COST_OF_EDGE*math.cos(thetha))
            elif(randomAbcissa - gotcha.abcissa <= 0 and randomOrdinate - gotcha.ordinate > 0):
                newNodeAbcissa = int(gotcha.abcissa + COST_OF_EDGE*math.sin(thetha))
                newNodeOrdinate = int(gotcha.ordinate + COST_OF_EDGE*math.cos(thetha))
            elif(randomAbcissa - gotcha.abcissa <= 0 and randomOrdinate - gotcha.ordinate < 0):
                newNodeAbcissa = int(gotcha.abcissa - COST_OF_EDGE*math.sin(thetha))
                newNodeOrdinate = int(gotcha.ordinate - COST_OF_EDGE*math.cos(thetha))
        else:
            newNodeAbcissa = gotcha.abcissa
            newNodeOrdinate = gotcha.ordinate + COST_OF_EDGE
        
        for aNode in self.nodes:
            if(newNodeAbcissa == aNode.abcissa and newNodeOrdinate == aNode.ordinate):
                condition = False
        
        newNodeCost = gotcha.cost + COST_OF_EDGE
        
        if((self.obstacleCheck(newNodeAbcissa, newNodeOrdinate) == True) and (condition == True)):
            newNode = nodes_evaluated(newNodeAbcissa, newNodeOrdinate, newNodeCost)
            self.nodes.append(newNode)
            newNode.parentNode = gotcha
            return True, newNode
        else:
            return False, newNodeOrdinate
    
    # find a better base node, basically going for RRT*, this function also colours the nodes
    def findBetterNode(self, newNode, count):
        possible = None
        for aNode in self.nodes:
            if(abs((aNode.abcissa - newNode.abcissa) < 7) and (abs(aNode.ordinate - newNode.ordinate) < 7)):
                if(aNode.cost + math.sqrt((aNode.abcissa - newNode.abcissa)**2 + (aNode.ordinate - newNode.ordinate)**2) < newNode.cost):
                    newNode.cost = aNode.cost + int(math.sqrt((aNode.abcissa - newNode.abcissa)**2 + (aNode.ordinate - newNode.ordinate)**2))
                    possible = aNode

        if count%2 == 0:
            img[newNode.abcissa, newNode.ordinate] = [32,156,8]
        else:
            img[newNode.abcissa, newNode.ordinate] = [3,1,255]

        if(possible is not None):
            newNode.parentNode = possible
        
        return newNode

    def backTrackFunction(self, startingNode):
        currentNode = startingNode
        while currentNode.parentNode is not None:
            img[currentNode.abcissa, currentNode.ordinate] = [33,255,240]
            currentNode = currentNode.parentNode

#this function is the starting function and will be used to initiate the trees   
def startFunction():
    letsGoFromStart = mainCode(nodesFromStart)
    letsGoFromEnd = mainCode(nodesFromEnd) 
    letsGoFromStart.start(50,50,0) 
    letsGoFromEnd.start(550,550,0)
    shortestFromStart = [999999,[]]
    shortestFromEnd = [999999,[]]

    count = 0

    random_x = 0
    random_y = 0
    while True:
        count +=1
    
        if(count > PRECISION_VALUE):
            break
        
        if count%2 == 0:
            while True:
                    randomCheck = True
                    random_x = rand.randint(1, 600-SAFE_DISTANCE_FROM_OBSTACLE - 1)
                    random_y= rand.randint(1,600-SAFE_DISTANCE_FROM_OBSTACLE - 1)
                    for aNode in letsGoFromStart.nodes: 
                        if(random_y == aNode.ordinate and random_x == aNode.abcissa):
                            randomCheck = False
                            break
                    if(randomCheck == True):
                        break
        
            returnValue = letsGoFromStart.findNearestNode(random_x, random_y)
            
            if(returnValue[0] is not True):
                count -= 1
                continue

            letsGoFromStart.findBetterNode(returnValue[1], count)

        else: 
            while True:
                    randomCheck = True
                    random_x = rand.randint(1, 600-SAFE_DISTANCE_FROM_OBSTACLE -1)
                    random_y= rand.randint(1,600 - SAFE_DISTANCE_FROM_OBSTACLE -1)
                    for aNode in letsGoFromEnd.nodes: 
                        if(random_y == aNode.ordinate and random_x == aNode.abcissa):
                            randomCheck = False
                            break
                    if(randomCheck == True):
                        break
        
            returnValue = letsGoFromEnd.findNearestNode(random_x, random_y)
            
            if(returnValue[0] is not True):
                count -= 1
                continue

            letsGoFromEnd.findBetterNode(returnValue[1], count)

    check1 = 99999
    check2 = 99999
    node1 = None
    node2 = None
    for aNode in nodesFromStart:
        for anotherNode in nodesFromEnd:
            for i in range(COST_OF_EDGE):
                for j in range(COST_OF_EDGE):
                    if (aNode.abcissa -1 + i == anotherNode.abcissa and aNode.ordinate -1 + j == anotherNode.ordinate):
                        if ((aNode.cost) + (anotherNode.cost)) < check1 + check2:
                            node1 = aNode
                            node2 = anotherNode
                            check1 = anotherNode.cost
                            check2 = aNode.cost

    letsGoFromStart.backTrackFunction(node1)
    letsGoFromEnd.backTrackFunction(node2)

print("Starting")
startFunction()
print("Winner Winner Chicken Dinner")
cv.imshow("Display window", img) 
k = cv.waitKey(0) 