//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool table error manager (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTTABLEERRORMANAGER_H
#define C_SDNDEDATAPOOLLISTTABLEERRORMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListTableErrorManager :
   public QObject
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListTableErrorManager(void);
   void Init(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
             const stw_types::uint32 & oru32_ListIndex);
   void OnErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   bool mq_ElementsInvalid;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
