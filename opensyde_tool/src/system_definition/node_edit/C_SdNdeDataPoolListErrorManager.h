//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list error manager (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTERRORMANAGER_H
#define C_SDNDEDATAPOOLLISTERRORMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListErrorManager :
   public QObject
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListErrorManager(void);
   void Init(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
   void OnErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(const bool & orq_Error);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   bool mq_ErrorInList;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
