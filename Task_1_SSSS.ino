#include "ssss.h"
#include "Entropy.h"

void setup() {
  // put your setup code here, to run once:
  
  Ssss i_ssss;
  SsssMode i_mode;
  FamilyId i_family;
  uint8_t temp_family;
  uint8_t i_shares;
  uint8_t private_key[30] = {224,181,214,12,53,89,154,24,137,252,189,224,203,126,98,76,163,46,126,149,108,18,109,239,225,130,7,52,212};
  uint8_t received_key[32];
  uint8_t secret1[32];
  uint8_t secret2[32];
  uint8_t secret3[32];
  uint8_t secret4[32];
  uint8_t secret5[32];
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo and Due
  }
  Entropy.initialize();
  
  ///////////////////////////Logic to load previous state of device///////////////////////////
  i_ssss.LoadState();
  i_mode = (SsssMode)i_ssss.getMode(); //Get the last mode of device
  i_shares = i_ssss.getShares();
  if(i_ssss.hasFamily())
  {
    temp_family = i_ssss.getFamily();
    i_family.family = ((temp_family&0xFF)>>3); //check the structure defination
    i_family.req = (temp_family&0x07);
  }

  ///////////////////////////   Implementation of 2 to 5 scheme   ////////////////////////////////
  
  if(NoMode == i_mode)
  {
    i_ssss.setFamily((1<<3)|(5));  //1 is familyID and 5 shares will be created
    i_ssss.setThreshold(2); //2 shares will needed to recover the share
    i_ssss.setPayload(30);  
  }

  do
  {
    switch(i_mode)
    {
      case NoMode:
        i_ssss.beginShuffle();
        break;
      case Shuffle:
        while(!(i_ssss.addEntropy(Entropy.randomByte())));
        i_ssss.addSecret(private_key);
        i_ssss.beginDeal();
        break;
      case Deal:
        i_ssss.dealNextShare((int8_t*)secret1);
        i_ssss.dealNextShare((int8_t*)secret2);
        i_ssss.dealNextShare((int8_t*)secret3);
        i_ssss.dealNextShare((int8_t*)secret4);
        i_ssss.dealNextShare((int8_t*)secret5);
        //Print all the shares created.
        //Print getShares to verify the no. of share created
        for(int cnt=0;cnt<32;cnt++)
        {
          Serial.print(secret1[cnt]);
        }
        for(int cnt=0;cnt<32;cnt++)
        {
          Serial.print(secret2[cnt]);
        }
        for(int cnt=0;cnt<32;cnt++)
        {
          Serial.print(secret3[cnt]);
        }
        for(int cnt=0;cnt<32;cnt++)
        {
          Serial.print(secret4[cnt]);
        }
        for(int cnt=0;cnt<32;cnt++)
        {
          Serial.print(secret5[cnt]);
        }
        
        i_ssss.setSharePayloadLength(32); // crosscheck this number
        i_ssss.beginCollect();
        break;
      case Collect:
        i_ssss.addShare(secret1);
        i_ssss.addShare(secret2);
        if(i_ssss.secretAvailable())
          i_ssss.beginReveal();
        //else we need to add more secret's
        //add check while write API alogrithm if(hasShare(secret1)
        break;
      case Reveal:
        i_ssss.getSecret(private_key);
        for(int cnt=0;cnt<30;cnt++)
        {
          Serial.print(private_key[cnt]);
        }
        i_ssss.clear();
        break;
    }
    i_mode = (SsssMode)i_ssss.getMode();
  }
  while(NoMode);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print("Password: ");
  //Serial.println(getPassword(pw));
}
