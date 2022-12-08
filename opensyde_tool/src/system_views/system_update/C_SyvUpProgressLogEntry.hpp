//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for progress log entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPROGRESSLOGENTRY_H
#define C_SYVUPPROGRESSLOGENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpProgressLogEntry;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvUpProgressLogEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpProgressLogEntry(const uint32_t ou32_NodeIndex, QWidget * const opc_Parent = NULL);
   ~C_SyvUpProgressLogEntry(void) override;

   void SetEntryDescription(const QString & orc_Value) const;
   void SetEntryStatus(const QString & orc_Value) const;
   bool DoesIndexMatch(const uint32_t ou32_NodeIndex) const;
   uint32_t GetNodeIndex(void) const;

private:
   Ui::C_SyvUpProgressLogEntry * mpc_Ui;
   uint32_t mu32_NodeIndex;

   //Avoid call
   C_SyvUpProgressLogEntry(const C_SyvUpProgressLogEntry &);
   C_SyvUpProgressLogEntry & operator =(const C_SyvUpProgressLogEntry &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
