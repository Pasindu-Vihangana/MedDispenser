
// Liverpool John Moores University;
// Msc in Embedded Systems and IC Design;
// Semester 1 Project - IoT Medical Dispenser;
// Lecturer;
// Dr.Samiru Gayan;
// Students Associated;
// Sanidi Sasenya       - 22pg0046;
// Pasindu Vihangana    - 22pg0060;
// Pasindu Wijewardena  - 22pg0051;
// May .2023

#include <Arduino.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
#include <DNSServer.h>
#include <ESP_Mail_Client.h>
#include <LiquidCrystal_I2C.h>

#include "index.h"

#define ONBOARD_LED 2

#define BUZZER 2

#define TRAY_P0 15
#define TRAY_P1 13
#define TRAY_P2 12
#define TRAY_P3 14

#define ROTATE 16
#define EJECT 0

#define COMPLETE A0

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

    // put function declarations here:
    void
    setPosition(uint8_t column_number);

void bootSystem();

void GPIO_init();

void autoConnectWiFi();

void onboardLED_on();
void onboardLED_off();

void emailNotification();
void smtpCallback(SMTP_Status status);

void init_server_request();

void setTimer();
void requestStack();
void loadnextStack();
void rootServer();

// put variable declarations here:
// ============ WiFi ============
WiFiManager WiFi_Manager;
const char MAC_ADDRESS[] = "C8:2B:96:1F:27:93";
const char AP_PASSWORD[] = "Password";
uint8_t LOCAL_IP[4];

SMTPSession smtp;
void smtpCallback(SMTP_Status status);

const char EMAIL_ADDRESS[] = "22pg0060@sltc.ac.lk";
const char EMAIL_PASSWORD[] = "wzxferhmerbtsjva";

const char RECIPIENT_EMAIL[] = "22pg0060@sltc.ac.lk";

ESP8266WebServer server(80); // Server on port 80

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long count = 0;
unsigned long countdown = 0;

uint8_t loadedStack = 0;

unsigned long timer_eject;
unsigned long timer_reminder;

unsigned long allowed_delay = 5 * 60 * 1000; // (min)
bool med_taken = false;

unsigned long looprate_timer;

unsigned long buzzer_timer;

void setup()
{
  bootSystem();

  GPIO_init();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printstr("ACCESSPOINT ON");
  // delay(500);

  autoConnectWiFi();

  init_server_request();

  looprate_timer = millis();
}

void loop()
{
  server.handleClient();

  if (millis() - looprate_timer > 200)
  {
    looprate_timer = millis();
    if (!med_taken && analogRead(COMPLETE) > 800)
    {

      digitalWrite(EJECT, HIGH);
    }

    if (digitalRead(EJECT) && analogRead(COMPLETE) < 800)
    {
      med_taken = true;
      digitalWrite(EJECT, LOW);
    }

    if (!med_taken && millis() - timer_eject > allowed_delay)
    {
      emailNotification();
      timer_eject = millis();
    }

    if (!med_taken && digitalRead(EJECT) && millis() - buzzer_timer > 2000)
    {
      tone(BUZZER, 200, 100);
    }

    if (med_taken)
    {
      digitalWrite(BUZZER, LOW);
    }
  }
}

void setPosition(uint8_t column_number)
{
  switch (column_number)
  {
  case 0:
    Serial.println("Position 0");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 1:
    Serial.println("Position 1");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 2:
    Serial.println("Position 2");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 3:
    Serial.println("Position 3");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 4:
    Serial.println("Position 4");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, HIGH);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 5:
    Serial.println("Position 5");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, HIGH);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 6:
    Serial.println("Position 6");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, HIGH);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 7:
    Serial.println("Position 7");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, HIGH);
    digitalWrite(TRAY_P3, LOW);
    break;

  case 8:
    Serial.println("Position 8");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, HIGH);
    break;

  case 9:
    Serial.println("Position 9");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, HIGH);
    break;

  case 10:
    Serial.println("Position 10");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, HIGH);
    break;

  case 11:
    Serial.println("Position 11");
    digitalWrite(TRAY_P0, HIGH);
    digitalWrite(TRAY_P1, HIGH);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, HIGH);
    break;

  case 12:
    Serial.println("Position 12");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;

  default:
    Serial.println("default");
    digitalWrite(TRAY_P0, LOW);
    digitalWrite(TRAY_P1, LOW);
    digitalWrite(TRAY_P2, LOW);
    digitalWrite(TRAY_P3, LOW);
    break;
  }
}

void bootSystem()
{
  lcd.begin();
  lcd.backlight();
  delay(300);
  lcd.noBacklight();
  delay(300);
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printstr("Booting");
  delay(300);
  lcd.write('.');
  delay(300);
  lcd.write('.');
  delay(300);
  lcd.write('.');
  delay(300);

  Serial.begin(115200);
  Serial.println("Serial Com Started");
}

void GPIO_init()
{
  // pinMode(ONBOARD_LED, OUTPUT);

  pinMode(TRAY_P0, OUTPUT);
  pinMode(TRAY_P1, OUTPUT);
  pinMode(TRAY_P2, OUTPUT);
  pinMode(TRAY_P3, OUTPUT);

  pinMode(ROTATE, OUTPUT);
  pinMode(EJECT, OUTPUT);

  digitalWrite(TRAY_P0, HIGH);
  digitalWrite(TRAY_P1, HIGH);
  digitalWrite(TRAY_P2, HIGH);
  digitalWrite(TRAY_P3, HIGH);

  digitalWrite(ROTATE, HIGH);
  delay(1000);
  digitalWrite(ROTATE, LOW);

  digitalWrite(EJECT, HIGH);
  delay(1000);
  digitalWrite(EJECT, LOW);

  digitalWrite(TRAY_P0, LOW);
  digitalWrite(TRAY_P1, LOW);
  digitalWrite(TRAY_P2, LOW);
  digitalWrite(TRAY_P3, LOW);

  // pinMode(COMPLETE, INPUT);
}

void onboardLED_on()
{
  digitalWrite(ONBOARD_LED, LOW);
  delay(500);
}

void onboardLED_off()
{
  digitalWrite(ONBOARD_LED, HIGH);
  delay(500);
}

void runRotation()
{
  timer_eject = millis();
  digitalWrite(ROTATE, HIGH);
  delay(1000);
  digitalWrite(ROTATE, LOW);
}

void autoConnectWiFi()
{
  Serial.println("Creating AccessPoint...");
  Serial.println("AccessPoint Created");
  Serial.print("SSID: ");
  Serial.println(MAC_ADDRESS);
  WiFi_Manager.autoConnect(MAC_ADDRESS, AP_PASSWORD);
  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printstr("Connected");
  lcd.setCursor(0, 0);
  lcd.printstr("server started");
  lcd.setCursor(0, 1);
  for (int i = 0; i < 4; i++)
  {
    LOCAL_IP[i] = WiFi.localIP()[i];
    String t_address = String(LOCAL_IP[i]);
    char ip_address[4];
    t_address.toCharArray(ip_address, 4);
    lcd.printstr(ip_address);
    // lcd.setCursor(i*3+3, 1);
    if (i == 3)
    {
      break;
    }
    lcd.printstr(".");
  }
  delay(300);
}

void emailNotification()
{
  smtp.debug(1);
  smtp.callback(smtpCallback);
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = EMAIL_ADDRESS;
  session.login.password = EMAIL_PASSWORD;
  session.login.user_domain = "";
  SMTP_Message message;

  message.sender.name = "MedDispenser";
  message.sender.email = EMAIL_ADDRESS;
  message.subject = "Time to Take Medicine";
  message.addRecipient("Myself", RECIPIENT_EMAIL);

  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>This is a Reminder to take your Medicine!</h1><p>- Sent from MedDispenser</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      // ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      // ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}

void init_server_request()
{
  server.on("/", rootServer);
  server.on("/setTimer", setTimer);
  server.on("/setStack", requestStack);
  server.on("/nextStack", loadnextStack);

  server.begin();
  Serial.println("server online...");
}

void setTimer()
{
  String payload = server.arg("sec");
  Serial.println(payload);
  countdown = payload.toInt();
  delay(20);
  Serial.print("countdown = ");
  Serial.println(countdown);
  server.send(200, "text/plane", "");
  timer_reminder = millis();
}

void requestStack()
{
  String payload = server.arg("stack");
  Serial.println(payload);
  int stack = payload.toInt();
  delay(20);
  Serial.print("loading Stack ");
  Serial.println(stack);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printstr("loading Stack ");
  char t_string[3];
  String(stack).toCharArray(t_string, 3);
  lcd.printstr(t_string);
  server.send(200, "text/plane", "");
  setPosition(stack);
  loadedStack = stack;
  med_taken = false;
  runRotation();
}

void loadnextStack()
{
  String payload = server.arg("stack");
  Serial.println(payload);
  int skip2next = payload.toInt();
  delay(20);
  Serial.print(skip2next);
  Serial.println("Stack(s) skipped");
  server.send(200, "text/plane", "");
  loadedStack += skip2next;
  if (loadedStack > 12)
  {
    loadedStack = loadedStack % 12;
  }
  setPosition(loadedStack);
  runRotation();
}

void rootServer()
{
  Serial.println("server page loading...");
  String s = index_html;            // Read HTML contents
  server.send(200, "text/html", s); // Send web page
}
