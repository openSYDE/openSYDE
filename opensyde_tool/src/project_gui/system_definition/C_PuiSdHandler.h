//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology: stores additional UI information (header)

   UI topology: stores additional UI information (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERH
#define C_PUISDHANDLERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerBusLogic.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandler :
   public C_PuiSdHandlerBusLogic
{
public:
   static C_PuiSdHandler * h_GetInstance(void);
   static void h_Destroy(void);

protected:
   //Avoid call (protected for test cases)
   explicit C_PuiSdHandler(QObject * const opc_Parent = NULL);
   virtual ~C_PuiSdHandler(void); //lint !e1768 see comment above

private:
   //Avoid call
   C_PuiSdHandler(const C_PuiSdHandler &);
   C_PuiSdHandler & operator =(const C_PuiSdHandler &);

   static C_PuiSdHandler * mhpc_Singleton;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
