//*********************************************
// Group:            AE-DIY-Group?
// Module name:      Bluetooth
// Version:          V0.9Beta
// Editor:           Rededge
// Log:              Undertest
//*********************************************
#include <Arduino.h>

char val;
int k = 1;
int ret;

int Bluetooth()
{
    val = BTSerial.read();
    switch (val)
    {
    default:
        BTSerial.println('Undefined code or BT not connected');
        ret = 0;
        break;
    case 's':
        BTSerial.println('Stop clock');
        ret = 1;
        break;
    case 'm':
        if (k == 1)
        {
            BTSerial.println('Mute device');
            k *= -1;
            ret = 2;
        }
        if (k == -1)
        {
            BTSerial.println('Unmute device');
            k *= -1;
            ret = 3;
        }
        break;
    }
    return ret;
}