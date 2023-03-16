//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool list error manager (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTERRORMANAGER_H
#define C_SDNDEDPLISTERRORMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListErrorManager :
   public QObject
{
   Q_OBJECT

public:
   C_SdNdeDpListErrorManager(void);
   void Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex);
   void OnErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(const bool & orq_Error);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   bool mq_ErrorInList;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
