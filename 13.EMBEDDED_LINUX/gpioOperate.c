/* ########################################################################################
 * Program Details 
 *
 * Date 	: 18 march 2015
 *
 * Author 	: prolific
 *
 * Purpose 	: sample code for program for cross compilation
 *
 * Compilation Option : ******-gcc gpioOperate.c -o gpioOperate (where **** is cross compiler prefix) 
 *
 * Work to be done : Error handling to be done for return values
 *
 * #########################################################################################
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF 50
#define MAX_GPIO 26
#define TRUE 1
#define FALSE 0

int fd;
char buf[MAX_BUF]; 

int exportGpio(int gpio)
{

	fd = open("/sys/class/gpio/export", O_WRONLY);

	sprintf(buf, "%d", gpio); 

	write(fd, buf, strlen(buf));

	close(fd);
	return 0;
}

int unexportGpio(int gpio)
{

	fd = open("/sys/class/gpio/unexport", O_WRONLY);

	sprintf(buf, "%d", gpio); 

	write(fd, buf, strlen(buf));

	close(fd);
	return 0;

}

int getDirection(int gpio, int *isDirectionIn)
{
	char buff[5];
	memset(buff,'\0',sizeof(buff));
	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);

	fd = open(buf, O_RDONLY);

	read(fd,buff,4); 
	
	printf("String = %s\n",buff);
	if(0 == strncmp(buff,"in",2))
	{
		printf("IN Direction\n");
		*isDirectionIn = 0;
	}
	else
	{

		*isDirectionIn = 1;
	}
	close(fd);
	return 0;
}

int setDirection(int gpio, int isDirectionIn)
{

	sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);

	fd = open(buf, O_WRONLY);

	if(isDirectionIn == TRUE)
	{
		// Set out direction
		write(fd, "out", 3); 
	}else
	{
		// Set in direction
		write(fd, "in", 2); 
	}

	close(fd);
	return 0;
}

int setValue(int gpio, int value)
{

	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);

	if(value == TRUE)
	{
		// Set out direction
		write(fd, "1" , 1); 
	}else
	{
		// Set in direction
		write(fd, "0" , 1); 
	}

	close(fd);
	return 0;
}

//lseek(fp, 0, SEEK_SET);
int getValue(int gpio, int *value)
{

	sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);

	read(fd, value, 1); 

	close(fd);
	return 0;
}

unsigned int printMenu()
{
	unsigned int option;
	printf("1. register gpio\n");
	printf("2. free gpio\n");
	printf("3. Set direction for gpio\n");
	printf("4. Get Direction for gpio\n");
	printf("5. Set Value for Gpio\n");
	printf("6. Get value for Gpio\n");
	printf("\nEnter Choice : \n");
	scanf("%d",&option);
	return option;
}



int main()
{
	unsigned int gpio=0;
	unsigned int option=0;
	int value=FALSE; 
	while(1)
	{
		option=printMenu();

		printf("Enter Gpio Number\n");
		scanf("%d",&gpio);
		
		switch(option)
		{
			case 5 :
				{
					printf("Enter Value to be set \n");
					scanf("%d",&value);
					setValue(gpio,value);
				}
				break;
			case 6 :
				{
					getValue(gpio,&value);
					printf("Value for Gpio %d  = %d \n",gpio,value);
				}
				break;
			case 1:
				{
					printf("Registering gpio = %d \n",gpio);
					exportGpio(gpio);
					break; 
				}
			case 2 :
				{
					printf("Registering gpio = %d \n",gpio); 
					unexportGpio(gpio);
					break;
				}
			case 3 : 
				{
					printf("Enter Direction to be set 1 : out, 0 : in \n");
					scanf("%d",&value);
					setDirection(gpio,value);
					break;
				}
			case 4 : 
				{
					printf("Direction for the port is 1 : out, 0 : in \n");
					getDirection(gpio,&value);
					printf("Direction of the Port is %s \n",(value==1)?"OUT":"IN");
					break;
				}			
		}
		
		value=0;
		gpio=0;
	}
	return 0;
}

