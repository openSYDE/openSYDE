//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for progress log entry (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPPROGRESSLOGENTRY_H
#define C_SYVUPPROGRESSLOGENTRY_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SyvUpProgressLogEntry;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SyvUpProgressLogEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpProgressLogEntry(const stw_types::uint32 ou32_NodeIndex, QWidget * const opc_Parent = NULL);
   ~C_SyvUpProgressLogEntry(void);

   void SetEntryDescription(const QString & orc_Value) const;
   void SetEntryStatus(const QString & orc_Value) const;
   bool DoesIndexMatch(const stw_types::uint32 ou32_NodeIndex) const;
   stw_types::uint32 GetNodeIndex(void) const;

private:
   Ui::C_SyvUpProgressLogEntry * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;

   //Avoid call
   C_SyvUpProgressLogEntry(const C_SyvUpProgressLogEntry &);
   C_SyvUpProgressLogEntry & operator =(const C_SyvUpProgressLogEntry &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
