#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "127.0.0.1:1883"
#define CLIENTID    "myuser"
#define CLIENTPW    "ZZZZ"
#define TOPIC       "mytopic"
#define PAYLOAD     "0"
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

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    int rc;
    char ch;
    int dval=0;

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

    do{
        if (dval==0){
            dval=1;
            pubmsg.payload = "1";
        }
        else {
            dval=0;
            pubmsg.payload = PAYLOAD;
        }

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        while(deliveredtoken != token);

        ch=getchar();
    } while(ch!='Q' && ch != 'q');


    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
