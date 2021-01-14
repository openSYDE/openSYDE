//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for opensyde communication driver (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERTHREAD_H
#define C_SYVCOMDRIVERTHREAD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QThread>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverThread :
   public QThread
{
   Q_OBJECT

public:
   typedef void (* PR_ThreadFunction)(void * const opv_Instance);

   C_SyvComDriverThread(PR_ThreadFunction const opr_ThreadFunc, void * const opv_Instance,
                        QObject * const opc_Parent = NULL);

protected:
   virtual void run(void);

private:
   //Avoid call
   C_SyvComDriverThread(const C_SyvComDriverThread &);
   C_SyvComDriverThread & operator =(const C_SyvComDriverThread &);

   const PR_ThreadFunction mpr_ThreadFunc;
   void * const mpv_FuncInstance;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
