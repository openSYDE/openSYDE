//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Buffer for max performance model additions (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMMETTREEGUIBUFFER_H
#define C_CAMMETTREEGUIBUFFER_H

/* -- Includes ------------------------------------------------------------- */
#include <list>
#include <mutex>
#include <QTimer>
#include <QObject>
#include "C_CamMetTreeLoggerData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamMetTreeGUIBuffer :
   public QObject
{
   Q_OBJECT

public:
   C_CamMetTreeGUIBuffer(QObject * const opc_Parent = NULL);

   void HandleData(const C_CamMetTreeLoggerData & orc_NewData);
   void ClearBuffer(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigInternalTrigger(void);
   void SigUpdateUi(const std::list<C_CamMetTreeLoggerData> & orc_Buffer);

private:
   bool mq_Connected;
   QTimer mc_Timer;
   std::mutex mc_BufferMutex;
   std::list<C_CamMetTreeLoggerData> mc_Buffer;

   void m_HandleUpdateUi(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
