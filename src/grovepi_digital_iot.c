#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "grovepi.h"

#define ADDRESS     "cloud.iot-playground.com:1883"
#define CLIENTID    "iotmx"
#define CLIENTPW    "m3x1c0"
#define TOPIC       "/myDigitalInput"
#define TOPIC2      "/myDigitalOutput"
#define PAYLOAD     "0"
#define PAYLOAD2    "0"
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

    if (!strcmp(topicName,TOPIC2))
      if (!strcmp(message->payload, PAYLOAD2))
        
        digitalWrite(4,0);
      else
        digitalWrite(4,1);

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

    //Exit on failure to start communications with the GrovePi
    if(init()==-1) exit(1);


    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc;
    char ch[1];
    int dval=0;

    pinMode(2,0);
    pinMode(4,1);

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = CLIENTID;
    conn_opts.password = CLIENTPW;

    pubmsg.payload = PAYLOAD;
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
           "Press Q<Enter> to quit\n\n", TOPIC2, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC2, QOS);

    while(1){
        dval=digitalRead(2);
        printf("Digital read %d\n", dval);
        sprintf(ch, "%d", dval);
        pubmsg.payload = ch;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        while(deliveredtoken != token);

        // Sleep
        pi_sleep(200);
    }


    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
