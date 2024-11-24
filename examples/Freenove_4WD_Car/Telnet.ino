/**********************************************************************
  Telnet.ino

  Telnet support routines
**********************************************************************/

//*********************************************************************
// Finish creating the network client
// Inputs:
//   client: Address of a NetworkClient object
//   contextData: Buffer to receive the address of an object allocated by
//                this routine
// Outputs:
//   Returns true if the routine was successful and false upon failure.
bool contextCreate(NetworkClient * client, void ** contextData)
{
    // Return an optional object address to be used as a parameter for
    // r4aTelnetClientProcessInput
    return r4aTelnetContextCreate(client, menuTable, menuTableEntries, contextData);
}

//*********************************************************************
// Display the telnet clients
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void listClients(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    telnet.listClients(display);
}

//*********************************************************************
// Display the server information
// Inputs:
//   menuEntry: Address of the object describing the menu entry
//   command: Zero terminated command string
//   display: Device used for output
void serverInfo(const R4A_MENU_ENTRY * menuEntry, const char * command, Print * display)
{
    telnet.serverInfo(display);
}
