#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "TM4C123GH6PM.h"
#include "macros.h"

#define OFF  1
#define UP   2
#define DOWN 3



//Declaration of Semaphores, Mutex, Queue, & Task Handler
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xLockSemaphore;
SemaphoreHandle_t xMutex;
QueueHandle_t xQueue;
TaskHandle_t  DriverHandle;

//Declaration of Functions
void INTERRUPTSInit(void);
void timer0Init(void);
void timer0_Delay(int time);
void motorInit(void);
void limitInit(void);
void buttonsInit(void);

/*
Description:
- This task waits for semaphore from ISR
- Then, it checks whether the lock button is pressed or not
- Depending on the value, the priority of the driver task is changed
*/
void lock(void* pvParameters) {
    xSemaphoreTake(xLockSemaphore, 0);
    while (1) {
			//Take semaphore
        xSemaphoreTake(xLockSemaphore, portMAX_DELAY);
			
			//Check on Button State
				if (GET_BIT(GPIO_PORTA_DATA_R,3)==0){
					
					
					GPIO_PORTF_DATA_R |= (1<<1); 				// Turn RED LED ON for indication
					vTaskPrioritySet(DriverHandle,2);  	// Change Driver Task Priority to 2
				}
				else if(GET_BIT(GPIO_PORTA_DATA_R,3)==1)
				{
					GPIO_PORTF_DATA_R &= ~(1<<1); 		// Turn RED LED OFF for indication
					vTaskPrioritySet(DriverHandle,1);	// CHange Driver Task Priority to 1
				}
		}
	}

/*
Description:
- This task waits for semaphore from ISR
- Then, it turns the motor DOWN for 1sec
*/
void jamTask(void* pvParameters) {
    xSemaphoreTake(xBinarySemaphore, 0);
    while (1) {
			
			//Take semaphore
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);

        //motor reverse
        GPIO_PORTF_DATA_R |= (1 << 3);
        GPIO_PORTF_DATA_R &= ~(1 << 2);
       
        timer0_Delay(1000); //Timer Delay Used to Unblock Task

				// Stop motor
			  GPIO_PORTF_DATA_R &= ~(1 << 3);
        GPIO_PORTF_DATA_R &= ~(1 << 2);
   }  
}


/*
Description:
- This task waits for queue from lower priority tasks
- When recieving Value, State of motor is determined
*/
void recieveQueue(void* pvParameters) {
		int Val;
		portBASE_TYPE xStatus;
		while(1)
		{
			xStatus=xQueueReceive(xQueue,&Val,portMAX_DELAY);		//Recieve From Queue
			if(Val==OFF)			//stop motor
			{
					GPIO_PORTF_DATA_R &= ~(1 << 3);
					GPIO_PORTF_DATA_R &= ~(1 << 2);
			}
			else if(Val==UP)			//open window
			{
					GPIO_PORTF_DATA_R |= (1 << 3);
					GPIO_PORTF_DATA_R &= ~(1 << 2);
			}
			else if(Val==DOWN)			//close window
			{
					GPIO_PORTF_DATA_R &= ~(1 << 3);
					GPIO_PORTF_DATA_R |= (1 << 2);
			}		
		}
}


/*
Description:
- This task takes mutex upon entry
- Polls on UP & DOWN PB
- If pressed less than 1sec, AUTOMATIC mode is used
- If pressed more than 1sec, MANUAL mode is used
*/
void driver(void* pvParameters){
		int Val;
	  portBASE_TYPE xStatus;
		while(1)
		{
			
			//Take Mutex
			xSemaphoreTake(xMutex,portMAX_DELAY );
			
			if (GET_BIT(GPIO_PORTD_DATA_R,0)==0){ 
				Val=UP;
				xStatus = xQueueSendToBack(xQueue,&Val,0); //Send in Queue
				vTaskDelay(400); 
				if (GET_BIT(GPIO_PORTD_DATA_R,0)==0) //still pressing then it is manual
					{
						while(GET_BIT(GPIO_PORTD_DATA_R,0)==0);
					}
     
				else if (GET_BIT(GPIO_PORTD_DATA_R,0)==1) // else it will be automatic
				{   							//limit																	ON																OFF
							while(!(GET_BIT(GPIO_PORTA_DATA_R,7)==1 | GET_BIT(GPIO_PORTD_DATA_R,0)==0 | GET_BIT(GPIO_PORTD_DATA_R,1)==0)); 
				}
				Val=OFF;
				xStatus = xQueueSendToBack(xQueue,&Val,0);
			}
			if (GET_BIT(GPIO_PORTD_DATA_R,1)==0){
					Val=DOWN;
					xStatus = xQueueSendToBack(xQueue,&Val,0);
					vTaskDelay(400); 
					if (GET_BIT(GPIO_PORTD_DATA_R,1)==0) //still pressing then it is manual
					{

					while(GET_BIT(GPIO_PORTD_DATA_R,1)==0);
					}
     
					else if (GET_BIT(GPIO_PORTD_DATA_R,1)==1) // else it will be automatic
					{   
								while(!(GET_BIT(GPIO_PORTD_DATA_R,6)==1 | GET_BIT(GPIO_PORTD_DATA_R,1)==0 | GET_BIT(GPIO_PORTD_DATA_R,0)==0)); 
					}
					Val=OFF;
					xStatus = xQueueSendToBack(xQueue,&Val,0);
			
				}
			

				xSemaphoreGive(xMutex);
				timer0_Delay(200);
		}
	}

	
	/*
Description:
- This task takes mutex upon entry
- Polls on UP & DOWN PB
- If pressed less than 1sec, AUTOMATIC mode is used
- If pressed more than 1sec, MANUAL mode is used
*/
void passenger(void* pvParameters){
		int Val;
		portBASE_TYPE xStatus;
		while(1)
		{
			//Take Mutex
			xSemaphoreTake(xMutex,portMAX_DELAY );
			if (GET_BIT(GPIO_PORTD_DATA_R,2)==0){
					Val=UP;
					xStatus = xQueueSendToBack(xQueue,&Val,0); //send in queue
					vTaskDelay(400); 
					if (GET_BIT(GPIO_PORTD_DATA_R,2)==0) //still pressing then it is manual
						{
							while(GET_BIT(GPIO_PORTD_DATA_R,2)==0) {
								if (uxTaskPriorityGet(DriverHandle) == 2) {
									break;
								}
							}
						}
			 
					else if (GET_BIT(GPIO_PORTD_DATA_R,2)==1) // else it will be automatic
						{   
							while(!(GET_BIT(GPIO_PORTA_DATA_R,7)==1 | GET_BIT(GPIO_PORTD_DATA_R,2)==0 | GET_BIT(GPIO_PORTD_DATA_R,3)==0)) {
								if (uxTaskPriorityGet(DriverHandle) == 2) {
									break;
								}
							}
						}
					Val=OFF;
					xStatus = xQueueSendToBack(xQueue,&Val,0);
			}
			
			if (GET_BIT(GPIO_PORTD_DATA_R,3)==0){
					Val=DOWN;
					xStatus = xQueueSendToBack(xQueue,&Val,0);
					vTaskDelay(400); 
					if (GET_BIT(GPIO_PORTD_DATA_R,3)==0) //still pressing then it is manual
					{
						while(GET_BIT(GPIO_PORTD_DATA_R,3)==0) {
							if (uxTaskPriorityGet(DriverHandle) == 2) {
									break;
								}
						}
					}
				 
					else if (GET_BIT(GPIO_PORTD_DATA_R,3)==1) // else it will be automatic
					{   
						while(!(GET_BIT(GPIO_PORTD_DATA_R,6)==1 | GET_BIT(GPIO_PORTD_DATA_R,3)==0 | GET_BIT(GPIO_PORTD_DATA_R,2)==0)) {
							if (uxTaskPriorityGet(DriverHandle) == 2) {
									break;
								}
						} 
					}
					Val=OFF;
					xStatus = xQueueSendToBack(xQueue,&Val,0);
					
			}
			xSemaphoreGive(xMutex);
			vTaskDelay(100); 
		}
}


int main( void )
{
	//Initialization of Semaphores, Queues, Mutex, GPIO, & Tasks
	  xQueue = xQueueCreate(2,sizeof(int));
	  xMutex = xSemaphoreCreateMutex(); 
    INTERRUPTSInit();
		buttonsInit();
		limitInit();
		motorInit();
	  timer0Init();
		__asm("CPSIE i\n");
		
		vSemaphoreCreateBinary(xBinarySemaphore);
		vSemaphoreCreateBinary(xLockSemaphore);
	if( xBinarySemaphore != NULL )
		{
			xTaskCreate( jamTask, "jamTask", 200, NULL, 5, NULL );
			xTaskCreate( recieveQueue, "recieveQueue", 200, NULL, 3, NULL );
			xTaskCreate( passenger, "passenger", 270, NULL, 1, NULL );
			xTaskCreate( driver, "driver", 270, NULL, 1, &DriverHandle );
			xTaskCreate( lock, "lock", 270, NULL, 4, NULL );
			vTaskStartScheduler();
		}
}


void GPIOA_Handler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	
	//Check if Jam Interrupt was triggered
		if(GPIO_PORTA_RIS_R == (1<<2)){
			xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
		}
		//Check if Lock Interrupt was triggered
		else if (GPIO_PORTA_RIS_R == (1<<3)){
			xSemaphoreGiveFromISR( xLockSemaphore, &xHigherPriorityTaskWoken );		
		}
		    //Clear Interrupt Flag
    GPIO_PORTA_ICR_R |= (1<<2) | (1<<3);
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


/*
Description:
- Configure Timer0 

*/
void timer0Init(void)
{
	SYSCTL_RCGCTIMER_R |= 0x01;
	TIMER0_CTL_R=0x00;
	TIMER0_CFG_R=0x00;
	TIMER0_TAMR_R=0x02;
	TIMER0_CTL_R=0x03;
}


/*
Description:
- Use timer0 for delays

*/
void timer0_Delay(int time)
{
	TIMER0_CTL_R=0x00;
	TIMER0_TAILR_R=16000*time-1;
	TIMER0_ICR_R=0x01;
	TIMER0_CTL_R |=0x03;
	while((TIMER0_RIS_R & 0x01)==0);
}


/*
Description:
- Configure Lock & Jam button Pins at pins A3 & A2 respectively
- These pins will be used as interrupts

*/
void INTERRUPTSInit(void)
{
	  //Enable PORTA
    SYSCTL_RCGCGPIO_R |= 0x01;
	
    __asm__("NOP; NOP; NOP; NOP;");					//while((SYSCTL_PRGPIO_R&0x00000001) == 0){};
	
	
	// Enable Pins A2 & A3 as INPUTS
    GPIO_PORTA_DIR_R &= ~((1 << 2) | (1 << 3));
    GPIO_PORTA_CR_R |= (1 << 2) | (1 << 3);
    GPIO_PORTA_PUR_R |= (1 << 2) | (1 << 3);	
    GPIO_PORTA_DEN_R |= (1 << 2) | (1 << 3);
	

	// Configure Pins as interrupts
    GPIO_PORTA_IM_R &=0;
    GPIO_PORTA_IS_R &= ~((1<<2) | (1<<3)); //Define as edge-triggered interrupts
    GPIO_PORTA_IEV_R &= ~((1<<2)); //Jam interrupt triggered by FALLING EDGE
		GPIO_PORTA_IBE_R |= (1<<3); // Lock Interrupt triggered by BOTH EDGES
    GPIO_PORTA_ICR_R |= (1<<2) | (1<<3);
    GPIO_PORTA_IM_R |= (1<<2) | (1<<3);	
	
	
	// Enable Interrrupts on PORTA
	  NVIC_PRI0_R |= (1<<7) | (1<<6) | (1<<5);
    NVIC_EN0_R |= (1<<0);
    
	
}


/*
Description:
- Configure Up & Down Pushbutton Pins at pins D0,D1,D2,D3

*/
void buttonsInit(void)
{
	//Enable PORTD
    SYSCTL_RCGCGPIO_R |= 0x08;
	
    __asm__("NOP; NOP; NOP; NOP;");					//while((SYSCTL_PRGPIO_R&0x00000008) == 0){};
	
	// Configure Pins D0 -> D3 as INPUT
    GPIO_PORTD_DIR_R &= ~((1 << 0)|(1<<1)|(1<<2)|(1<<3));
    GPIO_PORTD_CR_R |= (1 << 0)|(1<<1)|(1<<2)|(1<<3);
    GPIO_PORTD_PUR_R |= (1 << 0)|(1<<1)|(1<<2)|(1<<3);	
    GPIO_PORTD_DEN_R |= (1 << 0)|(1<<1)|(1<<2)|(1<<3);
}


/*
Description:
- Configure Limit Switch Pins at pins D6 & A7

*/
void limitInit(void)
{
	
	//Enable PORTD
    SYSCTL_RCGCGPIO_R |= 0x08;
	
    __asm__("NOP; NOP; NOP; NOP;");					//while((SYSCTL_PRGPIO_R&0x00000008) == 0){};
	
	
	//Configure Pin D6 as INPUT
    GPIO_PORTD_DIR_R &= ~(1 << 6);
	  GPIO_PORTD_DIR_R &= ~(1 << 4);
    GPIO_PORTD_CR_R |= (1 << 6)|(1<<4);
    GPIO_PORTD_PUR_R |= (1 << 6)|(1<<4);
    GPIO_PORTD_DEN_R |= (1 << 6)|(1<<4);
	
	//Configure Pin A7 as INPUT
		GPIO_PORTA_DIR_R &= ~(1 << 7);
    GPIO_PORTA_CR_R |= (1<<7);
    GPIO_PORTA_PUR_R |= (1<<7);
    GPIO_PORTA_DEN_R |= (1<<7);
}



/*
Description:
- Configure Motor Pins IN1 & IN2 at pins F2 & F3

*/
void motorInit(void)
{
	
	//Enable PORTF
    SYSCTL_RCGCGPIO_R |= 0x20;
	
    __asm__("NOP; NOP; NOP; NOP;");					//while((SYSCTL_PRGPIO_R&0x00000020) == 0){};
	
	//Enable Pins 2 & 3 for MOTOR + LEDS, and pin 1 for LEDS
	//LEDS are used for indication
	//Enable PINS as OUTPUT then ENABLING
    GPIO_PORTF_DIR_R |= ((1 << 1)|(1 << 2)|(1<<3));
    GPIO_PORTF_CR_R |= (1 << 1)|(1 << 2)|(1<<3);
    GPIO_PORTF_DEN_R |= (1 << 1)|(1 << 2)|(1<<3);
}