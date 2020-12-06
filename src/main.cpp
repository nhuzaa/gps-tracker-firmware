
#include <constant.h>
#include <datatype.h>

#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// Increase RX buffer if needed
//#define TINY_GSM_RX_BUFFER 512

// Uncomment this if you want to see all AT commands
//#define DUMP_AT_COMMANDS

SoftwareSerial SerialAT(RXPin, TXPin);

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, SERVER, PORT);
String content;
String datatosend;
String lat;
String lon;
String speed;

String status = "OFF";

void gprsConnect();
void gprsDisconnect();
void printOutput();
void getRequest();
void postRequest();
void statusCheck();
void network();
void getGPSCOR();
void sendData();
void getgps();
void offgps();
void sendData(String, const int, boolean);

void setup()
{
  // Set console baud rate
  Serial.begin(SERIAL_BAUDRATE);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(GSM_BAUDRATE);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println(F("Initializing modem..."));
  modem.init();

  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem: "));
  Serial.println(modemInfo);

  pinMode(STATUS_LED, OUTPUT);
  pinMode(GPRS_LED, OUTPUT);
  digitalWrite(GPRS_LED, 1);
  digitalWrite(STATUS_LED, 0);

  // if (!modem.hasSSL()) {
  //   Serial.println(F("SSL is not supported by this modem"));
  //   while(true) { delay(1000); }
  // }
}

void loop()
{
  statusCheck();
  getGPSCOR();
  statusCheck();
  // delay(1000);
  network();
  // delay(1000);
}

void network()
{
  gprsConnect();
  // getRequest();
  postRequest();
  // newpostRequest();
  gprsDisconnect();
}

void statusCheck()
{
  int value = analogRead(STATUS_BUTTON);
  Serial.println("Status button");
  Serial.println(value);
  if (value > 700)
  {
    digitalWrite(STATUS_LED, HIGH);
    status = "ON";
  }
  else
  {
    digitalWrite(STATUS_LED, LOW);
    status = "OFF";
  }
}

void printOutput()
{
  int status = http.responseStatusCode();
  Serial.println(status);
  if (!status)
  {
    delay(10000);
    return;
  }

  while (http.headerAvailable())
  {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    Serial.println(headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0)
  {
    Serial.print(F("Content length is: "));
    Serial.println(length);
  }
  if (http.isResponseChunked())
  {
    Serial.println(F("The content is chunked"));
  }

  String body = http.responseBody();
  Serial.println(F("content:"));
  Serial.println(body);

  Serial.print(F("Body length is: "));
  Serial.println(body.length());
}

void gprsDisconnect()
{
  http.stop();
  Serial.println(F("Server disconnected"));

  modem.gprsDisconnect();
  Serial.println(F("GPRS disconnected"));
  digitalWrite(GPRS_LED, 0);
}

void gprsConnect()
{
  Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork())
  {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");
  Serial.print(F("Connecting to "));
  Serial.print(APN);
  if (!modem.gprsConnect(APN, USERNAME, PASSWORD))
  {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  digitalWrite(GPRS_LED, 1);
  Serial.println(" OK");
}

void getRequest()
{

  Serial.print(F("Performing HTTP GET request... "));
  int err = http.get(RESOURCE);
  if (err != 0)
  {
    Serial.println(F("failed to connect"));
    delay(10000);
    return;
  }
  printOutput();
}

void postRequest()
{

  String contentType = "application/x-www-form-urlencoded";

  Serial.print(F("Performing HTTP post request... "));
  int err = http.post(RESOURCE, contentType, datatosend);
  if (err != 0)
  {
    Serial.println(F("failed to connect"));
    delay(10000);
    return;
  }
  // printOutput();
}

void getGPSCOR()
{

  getgps();

  delay(2000);
  sendData("AT+CGNSINF", 1000, DEBUG);

  while (lat == "" || lon == "" || lat == "0" || lon == "0")
  {
    Serial.print(".");
    sendData("AT+CGNSINF", 1000, DEBUG);
    delay(2000);
  }

  Serial.println("Latitude and Longitude");
  Serial.println(lat);
  Serial.println(lon);
  Serial.println(speed);
  Serial.println(status);

  char *tmp_lat = (char *)malloc(20 * sizeof(char));
  lat.toCharArray(tmp_lat, 20);

  char *tmp_lon = (char *)malloc(20 * sizeof(char));
  lon.toCharArray(tmp_lon, 20);

  char *tmp_speed = (char *)malloc(20 * sizeof(char));
  speed.toCharArray(tmp_speed, 20);

  datatosend = "Latitude=" + lat + "&Longitude=" + lon + "&DeviceId=" + String(DEVICE_ID) + "&Status=" + status + "&Speed=" + speed;
  offgps();
}

void getgps(void)
{
  sendData("AT+CGNSPWR=1", 1000, true);
  sendData("AT+CGPSINF=0", 1000, true);
}

void offgps(void)
{
  sendData("AT+CGNSPWR=0", 1000, true);
}

void sendData(String command, const int timeout, boolean debug)
{
  content = "", lat = "", lon = "", speed = "";
  SerialAT.println(command);
  delay(5);
  if (debug)
  {
    long int time = millis();
    while ((time + timeout) > millis())
    {
      while (SerialAT.available())
      {
        content += char(SerialAT.read());
      }
    }
    Serial.println("content");
    Serial.println(content);
    if (command.equals("AT+CGNSINF"))
    {
      //Serial.print(content);
      int from = -1, to = -1, count = 0;
      for (int i = 0; i < content.length(); i++)
      {
        if (content[i] == ',')
        {
          if (from != -1)
          {
            count++;
            to = i;
            if (count == 3)
              lat = content.substring(from + 1, to);
            if (count == 4)
              lon = content.substring(from + 1, to);
            if (count == 6)
              speed = content.substring(from + 1, to);
            from = to;
          }
          else
          {
            from = i + 2;
          }
        }
      }
    }
  }
}
