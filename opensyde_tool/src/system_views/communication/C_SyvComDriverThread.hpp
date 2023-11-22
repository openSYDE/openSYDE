//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for opensyde communication driver (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERTHREAD_HPP
#define C_SYVCOMDRIVERTHREAD_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QThread>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverThread :
   public QThread
{
   Q_OBJECT

public:
   typedef void (* PR_ThreadFunction)(void * const opv_Instance);

   C_SyvComDriverThread(const PR_ThreadFunction opr_ThreadFunc, void * const opv_Instance,
                        QObject * const opc_Parent = NULL);

protected:
   void run(void) override;

private:
   //Avoid call
   C_SyvComDriverThread(const C_SyvComDriverThread &);
   C_SyvComDriverThread & operator =(const C_SyvComDriverThread &) &;

   const PR_ThreadFunction mpr_ThreadFunc;
   void * const mpv_FuncInstance;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
