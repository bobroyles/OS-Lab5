/*

Lab 5
Operating Systems
Bo Broyles
3/21/23

*/


#include <unistd.h>
#include <iostream>
//#include <ostream>
#include <pthread.h>  
#include <semaphore.h>
#include <stdio.h>
//using std::cout;

//i need a structure for chopsticks
//available represents whether a chopstick has been picked up or not
//i need 5 of them becuase there are 5 on the table
//if picked up then available = 1  (not available anymore)
//if chopstick is on the table then available = 0
typedef struct chopstick
{
    int available; //availability of the chop stick to be picked up, 0 = available, 1 = not available
};

//there needs to be a structure for a person
//each person can have a right and a left chopstick
//a value of 0 means they dont have the chopstick
//a value of 1 means they have the chopstick 
typedef struct person
{
    int rightStick;
    int leftStick;
    int personNum;
};

//semaphores for the chopsticks
//if they are locked it means the associated chopstick has been picked up 
//the semaphores are meant to represent each chopstick 
//they will be used to represent if a stick has been picked up and therefore locked
sem_t Locks[5];
//array of chopsticks
//5 for the 5 chopsticks on the table
chopstick sticks[5];
//philosopher number
//5 philosophers at the table
int PhilNum[5] = {0, 1, 2, 3, 4};

//pickup is when a philosopher picks up a chopstick
//int Num represents the chopstick number that was picked up
void pickup(int Num){ 
    //i thought i needed the semaphores here but i moved them to testingEat
   // sem_wait(&Locks[Num]);
    sticks[Num].available = 1; //setting chopstick availability to 1 because it is now picked up and not available
   // sem_post(&Locks[Num]);
  

}

//putdown is when a philosopher puts down a chopstick 
//int stickNum represents the chopstick number that was put down
void putdown(int stickNum){ 
    //I thought i needed the semaphores here but i moved them to testingEat
   // sem_wait(&Locks[stickNum]);
    sticks[stickNum].available = 0; //chopstick availability set to 0 because it is being put down and available to others
   // sem_post(&Locks[stickNum]);
  
}

//eating is when the philosopher has already picked up both chopsticks and now will eat
//a random amount of time to eat for is generated and then the philosopher is "put to sleep" for that amount of time
void eating(int personNum) { 

    int eatTime = 0; //seconds person is eating for
    eatTime = rand() % 20 + 1; //random eating time between 1 and 10 seconds
    std::cout << "Philosopher " << personNum+1 << " is eating for " << eatTime << " seconds.\n";  //print messaage to terminal
    sleep(eatTime); //put person thread to sleep for amount of time they eat for 
    
}


//testingEat is called when the philosopher is done thinking and ready to eat
//int philosopherNum is the number of the philosopher, used to see what chopsticks he needs
void testingEat(int philosopherNum){

    int left = philosopherNum; //the left chopstick the philosopher needs to pick up 
    int right = (philosopherNum+1) % 5; //the right chopstick the philosopher needs to pick up, it is +1 and % 5 because it must be 1 greater than his own number and if he is philosopher 5 then he must pick up chopstick 0

    sem_wait(&Locks[left]); //locking the left chopstick, and waiting if chopstick is already picked up
    pickup(left); //picking up the left chopstick
    std::cout << "Philosopher " << philosopherNum+1 << " picked up chopstick " << left << "\n"; 

    sem_wait(&Locks[right]); //locking the right chopstick, and waiting if chopstick is already picked up
    pickup(right); //picking up the right chopstick
    std::cout << "Philosopher " << philosopherNum+1 << " picked up chopstick " << right << "\n";

    eating(philosopherNum); //when both chop sticks have been picked up philosopher can begin eating

    putdown(left); //putting down the left chopstick after eating
    std::cout << "Philosopher " << philosopherNum+1 << " put down chopstick " << left << "\n";

    putdown(right); //putting down the right chopstick after eating
    std::cout << "Philosopher " << philosopherNum+1 << " put down chopstick " << right << "\n";

    sem_post(&Locks[left]); //unlocking the left chopstick because it has been put down
    sem_post(&Locks[right]); //unlocking the right chopstick because it has been put down

}

//thinking is what all the philosophers do before eating
//they think for a certain amount of time before getting hungry
//once hungry they try to pick up both chopsticks 
void* thinking( void* i){ 
    
    int num = *reinterpret_cast<int*>(i); //got this from cplusplus.com link: https://cplusplus.com/forum/general/269149/
    
    do {
    
        int thinkTime; //number of seconds philosopher will sleep for
        thinkTime = rand() % 5 + 1; //generate random number of seconds to sleep between 1 and 9
        std::cout << "Philosopher " << num+1 << " is thinking for " << thinkTime << " seconds.\n";
        sleep(thinkTime); //think/sleep for generated amount of time

        testingEat(num); //after thinking the philosopher gets hungry

    } while (true); //always running
}

//array to hold the philosophers sitting at the table
//based on problem the the seating arrangment of the philosophers is based on philosopher index +1 and index -1 (+1 is to the right -1 is to the left)
//person philosophers[5]; 
// ^^^^ didnt end up needing person types


//the main function has the simulation running 
//first the semaphores are initialized, then the threads
//the threads are then joined together and call the thinking function
int main(){
    srand(time(NULL));
    
    //initializing the semaphores that protect the chopsticks 
        for (int i = 0; i<5; i++){
            sem_post(&Locks[i]);
        }
    //creating the philosophers 
    pthread_t th1, th2, th3, th4, th5;
    pthread_attr_t attr1, attr2, attr3, attr4, attr5;

    //thinking is the function that they all start out calling
    pthread_create(&th1, &attr1, thinking, &PhilNum[0]);
    pthread_create(&th2, &attr2, thinking, &PhilNum[1]);
    pthread_create(&th3, &attr3, thinking, &PhilNum[2]);
    pthread_create(&th4, &attr4, thinking, &PhilNum[3]);
    pthread_create(&th5, &attr5, thinking, &PhilNum[4]);

    //joinging all the threads (philosophers) to the main thread 
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);
    pthread_join(th4, NULL);
    pthread_join(th5, NULL);  

    return 0;
}