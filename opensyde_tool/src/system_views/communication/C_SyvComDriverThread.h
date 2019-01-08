//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Thread for opensyde communication driver (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERTHREAD_H
#define C_SYVCOMDRIVERTHREAD_H

/* -- Includes ------------------------------------------------------------- */
#include <QThread>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvComDriverThread :
   public QThread
{
   Q_OBJECT

public:
   typedef void (* PR_ThreadFunction)(void * const opv_Instance);

   C_SyvComDriverThread(PR_ThreadFunction const opr_ThreadFunc, void * const opv_Instance,
                        QObject * const opc_Parent = NULL);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void run(void);
   //lint -restore

private:
   //Avoid call
   C_SyvComDriverThread(const C_SyvComDriverThread &);
   C_SyvComDriverThread & operator =(const C_SyvComDriverThread &);

   const PR_ThreadFunction mpr_ThreadFunc;
   void * const mpv_FuncInstance;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
