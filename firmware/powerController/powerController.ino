// General parameters
#define CHANNELS 8
#define TIMEOUT 1000
#define BAUD 57600

// Unique call codes
#define SERIAL_PING 100
#define SERIAL_STOP_ALL  101
#define SERIAL_GET_ALL  102

// Call indexes (to set fet 4 to on, call SERIAL_FET_ON+4)
#define SERIAL_FET_OFF CHANNELS*0
#define SERIAL_FET_ON  CHANNELS*1
#define SERIAL_GET     CHANNELS*2

// Response codes
#define RESP_SUCCESS 1
#define RESP_FAIL 0

// Globals
int CUR_PIN[] = {7, 6, 5, 4, 3, 2, 1, 0};
int FET_PIN[] =   {2, 3, 4, 5, 6, 7, 8, 9};

char buff[2];

void setFet(int board, bool state)
{
  digitalWrite(FET_PIN[board], !state); //inverts state
}

int getCurrent(int board)
{
  return analogRead(CUR_PIN[board]);
}

void send(int value)
{
  buff[0] = lowByte(value);
  buff[1] = highByte(value);
  Serial.write(buff, 2);
}

void setup() {
  for(int board=0; board<8; board++)
  {
    pinMode(FET_PIN[board], OUTPUT);
    setFet(board, false);
  }

  Serial.begin(BAUD);
  Serial.setTimeout(TIMEOUT);
}

void SetAll(bool v)
{
  for(int board = 0; board < CHANNELS; board++)
    setFet(board, v);
}

bool parse(int input)
{
  switch (input) {
    case SERIAL_PING:
      send(RESP_SUCCESS);
      return true;
      
    case SERIAL_STOP_ALL:
      SetAll(false);
      send(RESP_SUCCESS);
      return true;
      
    case SERIAL_GET_ALL:
      for(int board = 0; board < CHANNELS; board++)
        send(getCurrent(board));
      return true;
  }

  int board;

  if((input >= SERIAL_FET_OFF) && (input < SERIAL_FET_OFF + CHANNELS))
  {
    board = input - SERIAL_FET_OFF;
    setFet(board, false);
    delay(1);
    send(getCurrent(board));
    return true;
  }
  
  if((input >= SERIAL_FET_ON) && (input < SERIAL_FET_ON + CHANNELS))
  {
    board = input - SERIAL_FET_ON;
    setFet(board, true);
    delay(1);
    send(getCurrent(board));
    return true;
  }
  
  if((input >= SERIAL_GET) && (input < SERIAL_GET + CHANNELS))
  {
    board = input - SERIAL_GET;
    send(getCurrent(board));
    return true;
  }
  
  return false;
  
}

void loop() {
  unsigned long start = millis();
  while(millis() - start < TIMEOUT)
  {
    if(!Serial.available()) continue;
      
    if(parse(Serial.read())) 
      return;
    else
      send(RESP_FAIL);
  }
  // If there's no response within the timeout window
  SetAll(false);
}
