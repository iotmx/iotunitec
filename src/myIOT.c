#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#include "grovepi.h"

#define ADDRESS     "io.adafruit.com:1883"
#define CLIENTID    "grprado"
#define TOPIC       "grprado/feeds/GvPiLED"
#define TOPIC2      "grprado/feeds/GvPiPushButton"
#define PAYLOAD     "ON"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{

    if (!strcmp(topicName,TOPIC))
    if (!strcmp(message->payload,"ON")) 
       //printf("It is ON\n");
       digitalWrite(4,1);
    else
       digitalWrite(4,0);


    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    if (init()==-1) exit(1);
    pinMode(4,1);
    pinMode(5,0);

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc;
    int ch;
    int dval, dval0;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = "grprado";
    conn_opts.password = "70b87211bc4940eba48380897f070eca";

//    sprintf(pubmsg.payload, "%d", 0);
    pubmsg.payload = "0";
    pubmsg.payloadlen = 1;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);	
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    MQTTClient_publishMessage(client, TOPIC2, &pubmsg, &token);
    printf("Waiting for publication\n");
    while(deliveredtoken != token);

    /*
    do 
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');  */

    while(1){
        dval = digitalRead(5);
        pi_sleep(50);
        if (dval!=dval0) {
            if (dval0==0)
                pubmsg.payload = "1";
            else
                pubmsg.payload = "0";
            //pubmsg.payloadlen = 1;

            MQTTClient_publishMessage(client, TOPIC2, &pubmsg, &token);
            printf("Waiting for publication\n");
            while(deliveredtoken != token);
        }    
        dval0=dval;
    }

        

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
